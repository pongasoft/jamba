/*
 * Copyright (c) 2018-2019 pongasoft
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

#include <type_traits>
#include <sstream>
#include <ostream>
#include "Cpp17.h"

namespace pongasoft::Utils {

/**
 * dynamic_cast<U *>(x) does not compile if x is not polymorphic. This class can be used instead and it will, at
 * compilation time, do the "right" thing. The usage is the following: Cast<U*>::dynamic(x). Note that, unlike
 * dynamic_cast, it does not handle & because the code needs to not fail when x is not polymorphic.
 */
template<typename U>
class Cast
{
public:
  template<typename T>
  static inline U dynamic(T *iPtr)
  {
    if constexpr (std::is_polymorphic<T>::value)
    {
      return dynamic_cast<U>(iPtr);
    }
    else
    {
      return nullptr;
    }
  }
};

/**
 * Defines the type for `operator!=`
 */
template<typename T>
using operator_not_eq_t = decltype(std::declval<T const&>() != std::declval<T const&>());

/**
 * Allows to detect whether a type defines `operator!=` at compile time.
 *
 * Example:
 * ```
 * if constexpr (Utils::is_operator_not_eq_defined<MyType>) {
 *   // implementation allows to write a != b (with a/b of MyType)
 * } else {
 *   // no such operator... alternate implementation
 * }
 * ```
 */
template<typename T>
constexpr auto is_operator_not_eq_defined = cpp17::experimental::is_detected_v<operator_not_eq_t, T>;

/**
 * Defines the type for `static_cast<To>(From)`
 */
template<typename From, typename To>
using static_cast_t = decltype(static_cast<To>(std::declval<From>()));

/**
 * Allows to detect (at compilation time) whether the call `static_cast<To>(from)` (where `from` is of type `From`)
 * will compile.
 *
 * Example:
 * ```
 * void f(SomeType &iValue) {
 * if constexpr(Utils::is_static_cast_defined<SomeType, int>) {
 *   auto i = static_cast<int>(iValue); // this will compile!
 *   ...
 * } else {
 *   // static_cast<int>(iValue) does NOT compile so do something different
 * }
 * ```
 */
template<typename From, typename To>
constexpr auto is_static_cast_defined = cpp17::experimental::is_detected_v<static_cast_t, From, To>;

/**
 * Defines the type for `operator<<`
 */
template<typename T>
using operator_write_to_ostream_t = decltype(std::declval<std::ostream &>() << std::declval<T const&>());

/**
 * Allows to detect whether a type defines `ostream << x` at compile time.
 *
 * Example:
 * ```
 * if constexpr (Utils::is_operator_write_to_ostream_defined<MyType>) {
 *   // implementation allows to write std::cout << x // (with x of type MyType)
 * } else {
 *   // no such operator... alternate implementation
 * }
 * ```
 */
template<typename T>
constexpr auto is_operator_write_to_ostream_defined = cpp17::experimental::is_detected_v<operator_write_to_ostream_t, T>;

/**
 * `typeid(T).name()` does not account for `const` or reference. This function adds the `const` and `&` qualifier
 * when necessary */
template<typename T>
std::string typeString()
{
  std::stringstream s;
  s << typeid(T).name();
  if(std::is_const_v<typename std::remove_reference<T>::type>)
    s << " const";
  if(std::is_reference_v<T>)
    s << " &";
  return s.str();
}

}