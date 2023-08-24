#ifndef INDEX_FLAT_HPP
#define INDEX_FLAT_HPP
#include "IndexFlatCodes.hpp"
#include <vector>
namespace vindex
{
  class IndexFlat : public IndexFlatCodes
  {
  public:
    explicit IndexFlat(idx_t d, MetricType metric = METRIC_L2);

    // void search(idx_t n, const float *x, idx_t k, float *distances, idx_t *labels, const SearchParameters *params = nullptr) const override;

    // void range_search(idx_t n, const float *x, float radius, RangeSearchResult *result, const SearchParameters *params = nullptr) const override;

    // void reconstruct(idx_t key, float *recons) const override;

    // void compute_distance_subset(idx_t n, const float *x, idx_t k, float *distances, const idx_t *labels) const;

    // get pointer to the floating point data
    // float *get_xb()
    // {
    //   return (float *)codes.data();
    // }
    // const float *get_xb() const
    // {
    //   return (const float *)codes.data();
    // }

    IndexFlat() {}

    // FlatCodesDistanceComputer *get_FlatCodesDistanceComputer() const override;

    /* The stanadlone codec interface (just memcopies in this case) */
    // void sa_encode(idx_t n, const float *x, uint8_t *bytes) const override;

    // void sa_decode(idx_t n, const uint8_t *bytes, float *x) const override;
  };

  class IndexFlatL2 : public IndexFlat
  {
  public:
    // std::vector<float> cached_l2norms;
    explicit IndexFlatL2(idx_t d) : IndexFlat(d, METRIC_L2) {}
    IndexFlatL2() {}

    // override for l2 norms cache.
    // FlatCodesDistanceComputer *get_FlatCodesDistanceComputer() const override;

    // compute L2 norms
    // void sync_l2norms();
    // clear L2 norms
    // void clear_l2norms();
  };
} // namespace vindex
#endif
