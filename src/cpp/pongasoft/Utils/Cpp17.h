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

#include <type_traits>

namespace pongasoft::Utils::cpp17 {

/**
 * The code here is copied from `<experimental/type_traits>` because it is not implemented with
 * Visual studio build tools 17. The intent is that this code will be removed when available on all
 * platforms.
 */
namespace experimental {
  // 3.3.3, Logical operator traits
  template <class...> using void_t = void;

  // 3.3.4, Detection idiom
  template <class...> using void_t = void;

  struct nonesuch {
    nonesuch()  = delete;
    ~nonesuch() = delete;
    nonesuch      (nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
  };

  template <class _Default, class _AlwaysVoid, template <class...> class _Op, class... _Args>
    struct _DETECTOR {
      using value_t = std::false_type;
      using type = _Default;
    };

  template <class _Default, template <class...> class _Op, class... _Args>
    struct _DETECTOR<_Default, void_t<_Op<_Args...>>, _Op, _Args...> {
      using value_t = std::true_type;
      using type = _Op<_Args...>;
    };


  template <template<class...> class _Op, class... _Args>
    using is_detected = typename _DETECTOR<nonesuch, void, _Op, _Args...>::value_t;
  template <template<class...> class _Op, class... _Args>
    using detected_t = typename _DETECTOR<nonesuch, void, _Op, _Args...>::type;
  template <template<class...> class _Op, class... _Args>
    constexpr bool is_detected_v = is_detected<_Op, _Args...>::value;

  template <class Default, template<class...> class _Op, class... _Args>
    using detected_or = _DETECTOR<Default, void, _Op, _Args...>;
  template <class Default, template<class...> class _Op, class... _Args>
    using detected_or_t = typename detected_or<Default, _Op, _Args...>::type;

  template <class Expected, template<class...> class _Op, class... _Args>
    using is_detected_exact = std::is_same<Expected, detected_t<_Op, _Args...>>;
  template <class Expected, template<class...> class _Op, class... _Args>
    constexpr bool is_detected_exact_v = is_detected_exact<Expected, _Op, _Args...>::value;

  template <class To, template<class...> class _Op, class... _Args>
    using is_detected_convertible = std::is_convertible<detected_t<_Op, _Args...>, To>;
  template <class To, template<class...> class _Op, class... _Args>
    constexpr bool is_detected_convertible_v = is_detected_convertible<To, _Op, _Args...>::value;


}}