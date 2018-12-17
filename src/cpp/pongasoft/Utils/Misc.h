/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
#ifndef __PONGASOFT_UTILS_MISC_H__
#define __PONGASOFT_UTILS_MISC_H__

#include <pongasoft/logging/logging.h>

namespace pongasoft {
namespace Utils {

/**
 * Make sure that the value remains within its bounds
 *
 * @param iLower the lower bound (must be <= iUpper)
 * @param iUpper the upper bound (must be >= iLower)
 */
template <typename T, typename U>
inline static T clamp(const U &iValue, const T &iLower, const T &iUpper)
{
  auto v = static_cast<T>(iValue);
  return v < iLower ? iLower : (v > iUpper ? iUpper : iValue);
}

/**
 * Make sure that the value remains within its bounds. The difference with clamp is that iFrom and iTo do not have
 * to be provided in any specific order.
 */
template <typename T, typename U>
inline static T clampRange(const U &iValue, const T &iFrom, const T &iTo)
{
  if(iFrom < iTo)
    return clamp(iValue, iFrom, iTo);
  else
    return clamp(iValue, iTo, iFrom);
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