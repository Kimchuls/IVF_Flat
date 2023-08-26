#include "InvertedLists.hpp"
#include <assert.h>
#include <cstdio>
#include <memory>

#include "VIndexAssert.hpp"

namespace vindex
{
  /************************************
   * InvertedLists Implementation
   *************************************/
  InvertedLists::InvertedLists(size_t nlist, size_t code_size)
      : nlist(nlist), code_size(code_size), use_iterator(false) {}

  InvertedLists::~InvertedLists() {}
  void InvertedLists::reset()
  {
    for (size_t i = 0; i < nlist; i++)
    {
      resize(i, 0);
    }
  }
  size_t InvertedLists::add_entry(
      size_t list_no,
      int64_t theid,
      const uint8_t *code)
  {
    return add_entries(list_no, 1, &theid, code);
  }
  void InvertedLists::prefetch_lists(const int64_t*, int) const {}

  /************************************
   * ArrayInvertedLists Implementation
   *************************************/
  ArrayInvertedLists::ArrayInvertedLists(size_t nlist, size_t code_size)
      : InvertedLists(nlist, code_size)
  {
    ids.resize(nlist);
    codes.resize(nlist);
  }
  ArrayInvertedLists::~ArrayInvertedLists() {}
  void ArrayInvertedLists::resize(size_t list_no, size_t new_size)
  {
    ids[list_no].resize(new_size);
    codes[list_no].resize(new_size * code_size);
  }

  size_t ArrayInvertedLists::add_entries(
      size_t list_no,
      size_t n_entry,
      const int64_t *ids_in,
      const uint8_t *code)
  {
    if (n_entry == 0)
      return 0;
    assert(list_no < nlist);
    size_t o = ids[list_no].size();
    ids[list_no].resize(o + n_entry);
    memcpy(&ids[list_no][o], ids_in, sizeof(ids_in[0]) * n_entry);
    codes[list_no].resize((o + n_entry) * code_size);
    memcpy(&codes[list_no][o * code_size], code, code_size * n_entry);
    return o;
  }
} // namespace vindex
