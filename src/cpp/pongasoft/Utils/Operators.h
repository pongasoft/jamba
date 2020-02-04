/*
 * Copyright (c) 2020 pongasoft
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

namespace pongasoft::Utils::Operators {

/**
 * Implements all the various equality and relational operators for the type `T` which is assumed to encapsulate some
 * value that can be accessed by dereferencing `T` or in other words `T::operator*()` returns the underlying value.
 *
 * If `T::operator*()` returns a primitive, then it should just work and nothing else needs to be done.
 *
 * If `T::operator*()` returns another class or struct, then the following operators should be defined (you can define
 * only some of them depending on your use):
 *
 * ```
 * struct S {...};
 *
 * struct T {
 *   constexpr S const & operator *() const {...}
 * };
 *
 * // for equality and relational operations between 2 S
 * constexpr bool operator==(const S &lhs, const S &rhs) {...};
 * constexpr bool operator<(const S &lhs, const S &rhs) {...};
 *
 * // for equality and relational operations between S and V
 * template<typename V>
 * constexpr bool operator==(const S &lhs, const V &rhs) {...};
 * template<typename V>
 * constexpr bool operator<(const S &lhs, const V &rhs) {...};
 *
 * ```
 *
 * @note This code is primarily used by the various `XXParam` classes which encapsulates some type.
 */
template<typename T>
class Dereferenceable
{
public:
  // LHS is reference, RHS is reference
  friend constexpr bool operator==(T const &lhs, T const &rhs) { return *lhs == *rhs; }

  friend constexpr bool operator!=(T const &lhs, T const &rhs) { return !(lhs == rhs); }

  friend constexpr bool operator<(T const &lhs, T const &rhs) { return *lhs < *rhs; }

  friend constexpr bool operator>(T const &lhs, T const &rhs) { return rhs < lhs; }

  friend constexpr bool operator<=(T const &lhs, T const &rhs) { return !(rhs < lhs); }

  friend constexpr bool operator>=(T const &lhs, T const &rhs) { return !(lhs < rhs); }

  // LHS is reference, RHS is value
  template<typename V>
  friend constexpr bool operator==(T const &lhs, V const &rhs) { return *lhs == rhs; }

  template<typename V>
  friend constexpr bool operator!=(T const &lhs, V const &rhs) { return !(lhs == rhs); }

  template<typename V>
  friend constexpr bool operator<(T const &lhs, V const &rhs) { return *lhs < rhs; }

  template<typename V>
  friend constexpr bool operator>(T const &lhs, V const &rhs) { return !(lhs < rhs) && lhs != rhs; }

  template<typename V>
  friend constexpr bool operator<=(T const &lhs, V const &rhs) { return !(rhs < lhs); }

  template<typename V>
  friend constexpr bool operator>=(T const &lhs, V const &rhs) { return !(lhs < rhs); }

  // LHS is value, RHS is reference
  template<typename V>
  friend constexpr bool operator==(V const &lhs, T const &rhs) { return rhs == lhs; }

  template<typename V>
  friend constexpr bool operator!=(V const &lhs, T const &rhs) { return !(lhs == rhs); }

  template<typename V>
  friend constexpr bool operator<(V const &lhs, T const &rhs) { return rhs > lhs; }

  template<typename V>
  friend constexpr bool operator>(V const &lhs, T const &rhs) { return rhs < lhs; }

  template<typename V>
  friend constexpr bool operator<=(V const &lhs, T const &rhs) { return !(rhs < lhs); }

  template<typename V>
  friend constexpr bool operator>=(V const &lhs, T const &rhs) { return !(lhs < rhs); }
};

}