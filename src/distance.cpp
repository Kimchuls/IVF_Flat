#include "distance.hpp"
#include <cstdio>
#include <omp.h>
#include <cmath>

#include "platform_macros.hpp"

namespace vindex
{
  VINDEX_PRAGMA_IMPRECISE_FUNCTION_BEGIN
  float fvec_norm_L2sqr(const float *x, size_t d)
  {
    // the double in the _ref is suspected to be a typo. Some of the manual
    // implementations this replaces used float.
    float res = 0;
    VINDEX_PRAGMA_IMPRECISE_LOOP
    for (size_t i = 0; i != d; ++i)
    {
      res += x[i] * x[i];
    }

    return res;
  }
  VINDEX_PRAGMA_IMPRECISE_FUNCTION_END
  void fvec_renorm_L2(size_t d, size_t nx, float *__restrict x)
  {
#pragma omp parallel for schedule(guided)
    for (int64_t i = 0; i < nx; i++)
    {
      float *__restrict xi = x + i * d;

      float nr = fvec_norm_L2sqr(xi, d);

      if (nr > 0)
      {
        size_t j;
        const float inv_nr = 1.0 / sqrtf(nr);
        for (j = 0; j < d; j++)
          xi[j] *= inv_nr;
      }
    }
  }
} // namespace vindex
