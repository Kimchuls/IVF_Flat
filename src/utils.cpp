#include "utils.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>
#include <omp.h>

#include <algorithm>
#include <set>
#include <type_traits>
#include <vector>
#include <sys/types.h>
namespace vindex
{
  double getmillisecs()
  {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1e3 + tv.tv_usec * 1e-3;
  }
} // namespace vindex
