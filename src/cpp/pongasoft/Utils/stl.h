/*
 * Copyright (c) 2023 pongasoft
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

#ifndef PONGASOFT_JAMBA_STL_H
#define PONGASOFT_JAMBA_STL_H

#include <array>

namespace pongasoft::stl {

// Implementation
namespace impl {
template<typename T, typename U, typename F, std::size_t N, std::size_t... I>
constexpr std::array<U, N> transformArray(std::array<T, N> const &a, F &&f, std::index_sequence<I...>)
{
  return { f(a[I])... };
}

}

/**
 * Transforms an array containing elements of type `T` into an array containing elements of type `U` by applying the
 * function 'f' (which should convert an element of type `T` into an element of type `U` */
template<typename T, typename U, typename F, std::size_t N, typename Indices = std::make_index_sequence<N>>
constexpr std::array<U, N> transform(std::array<T, N> const &a, F &&f)
{
  return impl::transformArray<T, U>(a, std::forward<F>(f), Indices{});
}

}

#endif //PONGASOFT_JAMBA_STL_H