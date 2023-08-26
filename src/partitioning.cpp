#include "partitioning.hpp"
#include <cassert>
#include <cmath>

#include "AlignedTable.hpp"

namespace vindex
{
  namespace name
  {
        
#define IFV if (false)

template <class C>
typename C::T partition_fuzzy_median3(
        typename C::T* vals,
        typename C::TI* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out) {
    if (q_min == 0) {
        if (q_out) {
            *q_out = C::Crev::neutral();
        }
        return 0;
    }
    if (q_max >= n) {
        if (q_out) {
            *q_out = q_max;
        }
        return C::neutral();
    }

    using T = typename C::T;

    // here we use bissection with a median of 3 to find the threshold and
    // compress the arrays afterwards. So it's a n*log(n) algoirithm rather than
    // qselect's O(n) but it avoids shuffling around the array.

    FAISS_THROW_IF_NOT(n >= 3);

    T thresh_inf = C::Crev::neutral();
    T thresh_sup = C::neutral();
    T thresh = median3(vals[0], vals[n / 2], vals[n - 1]);

    size_t n_eq = 0, n_lt = 0;
    size_t q = 0;

    for (int it = 0; it < 200; it++) {
        count_lt_and_eq<C>(vals, n, thresh, n_lt, n_eq);

        IFV printf(
                "   thresh=%g [%g %g] n_lt=%ld n_eq=%ld, q=%ld:%ld/%ld\n",
                float(thresh),
                float(thresh_inf),
                float(thresh_sup),
                long(n_lt),
                long(n_eq),
                long(q_min),
                long(q_max),
                long(n));

        if (n_lt <= q_min) {
            if (n_lt + n_eq >= q_min) {
                q = q_min;
                break;
            } else {
                thresh_inf = thresh;
            }
        } else if (n_lt <= q_max) {
            q = n_lt;
            break;
        } else {
            thresh_sup = thresh;
        }

        // FIXME avoid a second pass over the array to sample the threshold
        IFV printf(
                "     sample thresh in [%g %g]\n",
                float(thresh_inf),
                float(thresh_sup));
        T new_thresh =
                sample_threshold_median3<C>(vals, n, thresh_inf, thresh_sup);
        if (new_thresh == thresh_inf) {
            // then there is nothing between thresh_inf and thresh_sup
            break;
        }
        thresh = new_thresh;
    }

    int64_t n_eq_1 = q - n_lt;

    IFV printf("shrink: thresh=%g n_eq_1=%ld\n", float(thresh), long(n_eq_1));

    if (n_eq_1 < 0) { // happens when > q elements are at lower bound
        q = q_min;
        thresh = C::Crev::nextafter(thresh);
        n_eq_1 = q;
    } else {
        assert(n_eq_1 <= n_eq);
    }

    int wp = compress_array<C>(vals, ids, n, thresh, n_eq_1);

    assert(wp == q);
    if (q_out) {
        *q_out = q;
    }

    return thresh;
}

  } // namespace name
  
  template <class C>
typename C::T partition_fuzzy(
        typename C::T* vals,
        typename C::TI* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out) {
    // the code below compiles and runs without AVX2 but it's slower than
    // the scalar implementation
    return partitioning::partition_fuzzy_median3<C>(
            vals, ids, n, q_min, q_max, q_out);
}

// explicit template instanciations

template float partition_fuzzy<CMin<float, int64_t>>(
        float* vals,
        int64_t* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);

template float partition_fuzzy<CMax<float, int64_t>>(
        float* vals,
        int64_t* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);

template uint16_t partition_fuzzy<CMin<uint16_t, int64_t>>(
        uint16_t* vals,
        int64_t* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);

template uint16_t partition_fuzzy<CMax<uint16_t, int64_t>>(
        uint16_t* vals,
        int64_t* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);

template uint16_t partition_fuzzy<CMin<uint16_t, int>>(
        uint16_t* vals,
        int* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);

template uint16_t partition_fuzzy<CMax<uint16_t, int>>(
        uint16_t* vals,
        int* ids,
        size_t n,
        size_t q_min,
        size_t q_max,
        size_t* q_out);
} // namespace vindex
