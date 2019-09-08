/*
 * Copyright (c) 2019 pongasoft
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

#pragma once

#include "../Types.h"
#include <string>
#include <codecvt>
#include <locale>
#include <sstream>
#include <pluginterfaces/base/ftypes.h>
#include <pongasoft/Utils/Metaprogramming.h>

namespace pongasoft::VST::VstUtils {

/**
 * Converts a `VstString16` to a regular `std::string` that is properly utf-8 encoded.
 */
inline std::string toUT8String(VstString16 const &iString)
{
  std::wstring_convert<std::codecvt_utf8_utf16<Steinberg::char16>, Steinberg::char16> converter{};
  return converter.to_bytes(iString);

//  Steinberg::String utf8Str(iString.c_str());
//  utf8Str.toMultiByte(Steinberg::kCP_Utf8);
//  return utf8Str.text8();
}

/**
 * This generic function will determine (at compilation time) whether `T` can be written to an `ostream` and if that
 * is the case, will use this information to build the string. Otherwise returns an empty string.
 */
template<typename T>
std::string toUTF8String(T const &iValue, Steinberg::int32 iPrecision)
{
  if constexpr(Utils::is_operator_write_to_ostream_defined<T>)
  {
    std::ostringstream s;
    if(iPrecision >= 0)
    {
      s.precision(iPrecision);
      s.setf(std::ios::fixed);
    }
    s << iValue;
    return s.str();
  }
  else
    return "";
}

/**
 * The VST SDK uses the concept of `FObject` (which are self contained reference counted objects) but require to be
 * manually managed (by calling `addRef` and `release`). In a few instances (`GUIValParameter` and `GUIJmbParameter`),
 * Jamba creates and manages `std::shared_ptr` while using the underlying raw pointer with VST SDK classes. So
 * in order to marry the 2 worlds, this util method creates a `shared_ptr` whose deleter simply delegates to the
 * `FObject::release` method:
 * - if the internal ref count is 1 and the shared_ptr goes away, `p->release()` will destroy the object
 * - if the internal ref count is > 1 (for example, `FObjectCx` still holds a reference) and the shared_ptr goes away,
 *   then `p->release()` only decrements the counter and whenever `FObjectCx` goes away, it will call `p->release()`
 *   which will then destroy the object
 */
template<typename T, typename... Args>
std::shared_ptr<T> make_sfo(Args&& ...iArgs)
{
  auto ptr = new T(std::forward<Args>(iArgs)...);
  std::shared_ptr<T> sptr(ptr, [](auto p) { p->release(); });
  return sptr;
}

}