#pragma once
#include "Index.hpp"
namespace vindex
{
  class IDSelector
  {
  public:
    virtual bool is_member(int64_t id) const = 0;
    virtual ~IDSelector() {}
  };
} // namespace vindex
