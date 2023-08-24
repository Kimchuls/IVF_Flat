#ifndef VINDEX_EXCEPTION_HPP
#define VINDEX_EXCEPTION_HPP
#include <exception>
#include <string>
namespace vindex
{
  class VINDEXException : public std::exception {
   public:
    explicit VINDEXException(const std::string& msg);

    VINDEXException(
            const std::string& msg,
            const char* funcName,
            const char* file,
            int line);

    /// from std::exception
    const char* what() const noexcept override;

    std::string msg;
};
} // namespace vindex

#endif