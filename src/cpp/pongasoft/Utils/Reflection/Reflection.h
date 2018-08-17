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

#pragma once

#include <utility>
#include <ostream>

namespace pongasoft {
namespace Utils {
namespace Reflection {

// This is inspired from this thread
// /https://stackoverflow.com/questions/257288/is-it-possible-to-write-a-template-to-check-for-a-functions-existence#9154394

// See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf.
template <typename...>
using void_t = void;

// Primary template handles all types not supporting the operation.
template <typename, template <typename> class, typename = void_t<>>
struct detect : std::false_type {};

// Specialization recognizes/validates only types supporting the archetype.
template <typename T, template <typename> class Op>
struct detect<T, Op, void_t<Op<T>>> : std::true_type {};

/**
 * This represents the api std::ostream &operator<<(std::ostream &os, const T &t) */
template <typename T>
using ostream_t = decltype(std::declval<std::ostream &>() << std::declval<T const &>());

// this will be "true" if the type T defines the ostream api
template <typename T>
using has_ostream = detect<T, ostream_t>;


}
}
}