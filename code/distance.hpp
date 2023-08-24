#pragma once
#include <cstdio>

namespace vindex
{
  float fvec_norm_L2sqr(const float *x, size_t d);
  void fvec_renorm_L2(size_t d, size_t nx, float *x);
} // namespace vindex
