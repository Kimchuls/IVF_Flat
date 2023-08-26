#include "DirectMap.hpp"
#include <cassert>
#include <cstdio>

#include "VIndexAssert.hpp"
#include "IDSelector.hpp"

namespace vindex
{
  DirectMap::DirectMap() : type(NoMap) {}
  void DirectMap::clear()
  {
    array.clear();
    hashtable.clear();
  }
  void DirectMap::check_can_add(const int64_t *ids)
  {
    if (type == Array && ids)
    {
      VINDEX_THROW_MSG("cannot have array direct map and add with ids");
    }
  }
  /********************* DirectMapAdd implementation */

  DirectMapAdd::DirectMapAdd(DirectMap &direct_map, size_t n, const int64_t *xids)
      : direct_map(direct_map), type(direct_map.type), n(n), xids(xids)
  {
    if (type == DirectMap::Array)
    {
      VINDEX_THROW_IF_NOT(xids == nullptr);
      ntotal = direct_map.array.size();
      direct_map.array.resize(ntotal + n, -1);
    }
    else if (type == DirectMap::Hashtable)
    {
      // can't parallel update hashtable so use temp array
      all_ofs.resize(n, -1);
    }
  }
  void DirectMapAdd::add(size_t i, int64_t list_no, size_t ofs)
  {
    if (type == DirectMap::Array)
    {
      direct_map.array[ntotal + i] = lo_build(list_no, ofs);
    }
    else if (type == DirectMap::Hashtable)
    {
      all_ofs[i] = lo_build(list_no, ofs);
    }
  }
  DirectMapAdd::~DirectMapAdd()
  {
    if (type == DirectMap::Hashtable)
    {
      for (int i = 0; i < n; i++)
      {
        int64_t id = xids ? xids[i] : ntotal + i;
        direct_map.hashtable[id] = all_ofs[i];
      }
    }
  }
} // namespace vindex
