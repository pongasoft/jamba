#ifndef __PONGASOFT_UTILS_MISC_H__
#define __PONGASOFT_UTILS_MISC_H__

#include <pongasoft/logging/logging.h>

namespace pongasoft {
namespace Utils {

/**
 * Make sure that the value remains within its bounds
 */
template <typename T, typename U>
inline static T clamp(const U &value, const T &lower, const T &upper)
{
  auto v = static_cast<T>(value);
  return v < lower ? lower : (v > upper ? upper : value);
}

/**
 * Same as clamp except it will actually fail/assert in debug mode. For example can be used to
 * access an array with an index and making sure the index is valid within the array. If it happens in production
 * release then it will no randomly crash the application by accessing random memory.
 */
template <typename T, typename U>
inline static T clampE(const U &value, const T &lower, const T &upper)
{
  auto v = static_cast<T>(value);
  DCHECK_F(v >= lower && v <= upper);
  return v < lower ? lower : (v > upper ? upper : value);
}


}
}

#endif // __PONGASOFT_UTILS_MISC_H__