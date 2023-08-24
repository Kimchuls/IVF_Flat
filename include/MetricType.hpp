#ifndef VINDEX_METRIC_TYPE_HPP
#define VINDEX_METRIC_TYPE_HPP
namespace vindex
{
  enum MetricType
  {
    METRIC_INNER_PRODUCT = 0, ///< maximum inner product search
    METRIC_L2 = 1,            ///< squared L2 search
  };
} // namespace vindex

#endif