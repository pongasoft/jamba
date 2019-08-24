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
 * if constexpr (is_operator_not_eq_defined<MyType>) {
 *   // implementation allows to write a != b (with a/b of MyType)
 * } else {
 *   // no such operator... alternate implementation
 * }
 * ```
 */
template<typename T>
constexpr auto is_operator_not_eq_defined = cpp17::experimental::is_detected_v<operator_not_eq_t, T>;
}