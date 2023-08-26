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
  double imbalance_factor(int k, const int *hist)
  {
    double tot = 0, uf = 0;

    for (int i = 0; i < k; i++)
    {
      tot += hist[i];
      uf += hist[i] * (double)hist[i];
    }
    uf = uf * k / (tot * tot);

    return uf;
  }

  double imbalance_factor(int n, int k, const int64_t *assign)
  {
    std::vector<int> hist(k, 0);
    for (int i = 0; i < n; i++)
    {
      hist[assign[i]]++;
    }

    return imbalance_factor(k, hist.data());
  }
} // namespace vindex
