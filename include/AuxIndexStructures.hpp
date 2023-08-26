#ifndef VINDEX_AUX_INDEX_STRUCTURES_HPP
#define VINDEX_AUX_INDEX_STRUCTURES_HPP

#include <cstring>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <vector>
namespace vindex
{
  /***********************************************************
 * Interrupt callback
 ***********************************************************/

struct InterruptCallback {
    virtual bool want_interrupt() = 0;
    virtual ~InterruptCallback() {}

    static std::mutex lock;
    static std::unique_ptr<InterruptCallback> instance;

    static void clear_instance();
    static void check();
    static bool is_interrupted();
    static size_t get_period_hint(size_t flops);
};
} // namespace vindex

#endif