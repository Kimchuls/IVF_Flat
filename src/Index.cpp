#include "Index.hpp"
#include <assert.h>
#include <vector>
#include "DistanceComputer.hpp"
#include "VIndexAssert.hpp"
namespace vindex
{
  int Index::get_d() { return d; }
  int64_t Index::get_ntotal() { return ntotal; };
  bool Index::get_is_trained() { return is_trained; };
  void Index::train(int64_t, const float *)
  {
    // does nothing
  }

  void Index::add_with_ids(int64_t, const float *, const int64_t *)
  {
    printf("add_with_ids not implemented for this type of index");
    assert(false);
  }

  void Index::assign(int64_t n, const float *x, int64_t *labels, int64_t k) const
  {
    std::vector<float> distances(n * k);
    // search(n, x, k, distances.data(), labels);
  }
  void Index::range_search(
      int64_t,
      const float *,
      float,
      RangeSearchResult *,
      const SearchParameters *params) const
  {
    VINDEX_THROW_MSG("range search not implemented");
  }

  void Index::reconstruct_n(int64_t i0, int64_t ni, float *recons) const
  {
#pragma omp parallel for if (ni > 1000)
    for (int64_t i = 0; i < ni; i++)
    {
      reconstruct(i0 + i, recons + i * d);
    }
  }

  void Index::search_and_reconstruct(
      int64_t n,
      const float *x,
      int64_t k,
      float *distances,
      int64_t *labels,
      float *recons,
      const SearchParameters *params) const
  {
    VINDEX_THROW_IF_NOT(k > 0);

    search(n, x, k, distances, labels, params);
    for (int64_t i = 0; i < n; ++i)
    {
      for (int64_t j = 0; j < k; ++j)
      {
        int64_t ij = i * k + j;
        int64_t key = labels[ij];
        float *reconstructed = recons + ij * d;
        if (key < 0)
        {
          // Fill with NaNs
          memset(reconstructed, -1, sizeof(*reconstructed) * d);
        }
        else
        {
          reconstruct(key, reconstructed);
        }
      }
    }
  }

  size_t Index::remove_ids(const IDSelector & /*sel*/)
  {
    printf("remove_ids not implemented for this type of index");
    assert(false);
    return -1;
  }

  void Index::reconstruct(int64_t, float *) const
  {
    printf("reconstruct not implemented for this type of index");
    assert(false);
  }

  void Index::compute_residual(const float *x, float *residual, int64_t key) const
  {
    reconstruct(key, residual);
    for (size_t i = 0; i < d; i++)
    {
      residual[i] = x[i] - residual[i];
    }
  }

  size_t Index::sa_code_size() const
  {
    VINDEX_THROW_MSG("standalone codec not implemented for this type of index");
  }
  void Index::sa_encode(int64_t, const float *, uint8_t *) const
  {
    VINDEX_THROW_MSG("standalone codec not implemented for this type of index");
  }

  void Index::sa_decode(int64_t n, const uint8_t *bytes, float *x) const
  {
    VINDEX_THROW_MSG("standalone codec not implemented for this type of index");
  }
  void Index::merge_from(Index & /* otherIndex */, int64_t /* add_id */)
  {
    VINDEX_THROW_MSG("merge_from() not implemented");
  }
  void Index::check_compatible_for_merge(const Index & /* otherIndex */) const
  {
    VINDEX_THROW_MSG("check_compatible_for_merge() not implemented");
  }

} // namespace vindex
