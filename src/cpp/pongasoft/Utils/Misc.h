/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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