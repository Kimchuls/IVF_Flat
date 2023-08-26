#ifndef VINDEX_UTILS_HPP
#define VINDEX_UTILS_HPP
#include <stdint.h>
namespace vindex
{
  double getmillisecs();
  /// a balanced assignment has a IF of 1
  double imbalance_factor(int n, int k, const int64_t *assign);
  /// same, takes a histogram as input
  double imbalance_factor(int k, const int *hist);
} // namespace vindex

#endif