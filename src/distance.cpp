#include "distance.hpp"

#include <omp.h>

namespace vindex
{
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
