#include "Index.hpp"
#include <assert.h>
#include <vector>
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

  void Index::sa_decode(int64_t n, const uint8_t *bytes, float *x) const
  {
    VINDEX_THROW_MSG("standalone codec not implemented for this type of index");

  }

} // namespace vindex
