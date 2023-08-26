#include "VIndexException.hpp"
namespace vindex
{
  VINDEXException::VINDEXException(const std::string &m) : msg(m) {}

  VINDEXException::VINDEXException(
      const std::string &m,
      const char *funcName,
      const char *file,
      int line)
  {
    int size = snprintf(
        nullptr,
        0,
        "Error in %s at %s:%d: %s",
        funcName,
        file,
        line,
        m.c_str());
    msg.resize(size + 1);
    snprintf(
        &msg[0],
        msg.size(),
        "Error in %s at %s:%d: %s",
        funcName,
        file,
        line,
        m.c_str());
  }

  const char *VINDEXException::what() const noexcept
  {
    return msg.c_str();
  }
} // namespace vindex
