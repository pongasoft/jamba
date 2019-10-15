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

#include <pluginterfaces/vst/vsttypes.h>
#include <pongasoft/logging/logging.h>
#include "IGUIParameter.h"
#include "GUIVstParameter.h"
#include "GUIJmbParameter.h"
#include "GUIValParameter.h"

namespace pongasoft::VST::GUI::Params {

using namespace Steinberg::Vst;

/**
 * Represents an optional parameter (Jmb, Vst or no param at all). Like `GUIVstParam` and `GUIJmbParam`, this class
 * is a wrapper arount the underlying parameter. This parameter is used in very generic views that need to handle
 * a Vst or Jmb parameter, as well as no parameter at all (which is for example the case when adding a brand new view
 * in the %VSTGUI editor) and still be functional.
 *
 * You can obtain an instance of an optional parameter by calling any `ParamAware::registerOptionalXXX`
 * (resp. `ParamAware::registerOptionalDiscreteXX`) method.
 *
 * @note Because `GUIOptionalParam` can represent a Vst parameter or a Jmb parameter, there are some restrictions on `T` itself
 *       (which do not exist if you use `GUIJmbParam`):
 *       - `T` must have empty constructor: `T()`
 *       - `T` have a copy constructor: `T(T const &)`
 *       - `T` must be copy assignable: `T& operator=(T const &)`
 */
template<typename T>
class GUIOptionalParam
{
  static_assert(std::is_default_constructible_v<T>, "T must have a default/empty constructor: T()");
  static_assert(std::is_copy_constructible_v<T>, "T must have a copy constructor: T(T const &)");
  static_assert(std::is_copy_assignable_v<T>, "T must be copy assignable: T& operator=(T const &)");

public:
  using class_type = GUIOptionalParam<T>;
  using ParamType = T;
  using EditorType = typename ITGUIParameter<T>::ITEditor;

public:
  // Constructor
  GUIOptionalParam() :
    fParameter{VstUtils::make_sfo<GUIValParameter<T>>(UNDEFINED_PARAM_ID, T{})} {}

  // Constructor
  explicit GUIOptionalParam(T const &iDefaultValue) :
    fParameter{VstUtils::make_sfo<GUIValParameter<T>>(UNDEFINED_PARAM_ID, iDefaultValue)} {}

  // Constructor
  explicit GUIOptionalParam(std::shared_ptr<ITGUIParameter<T>> iParameter) : fParameter{std::move(iParameter) } {
    DCHECK_F(fParameter != nullptr);
  }

  /**
   * Always return `true` because by definition an optional parameter **always** exist. */
  inline bool exists() const { return true; }

  /**
   * @copydoc IGUIParameter::getParamID() */
  inline ParamID getParamID() const { return fParameter->getParamID(); }

  /**
   * @return the current value of the parameter as a `T`
   */
  inline ParamType getValue() const
  {
    ParamType res;
    fParameter->accessValue([&res](auto const &iValue) { res = iValue; });
    return res;
  }

  /**
   * @copydoc ITGUIParameter::update(ParamType const &) */
  inline bool update(ParamType const &iValue)
  {
    return fParameter->update(iValue);
  }

  /**
   * @copydoc ITGUIParameter::setValue() */
  inline tresult setValue(ParamType const &iValue)
  {
    return fParameter->setValue(iValue);
  }

  /**
   * @copydoc ITGUIParameter::edit() */
  inline std::unique_ptr<EditorType> edit()
  {
    return fParameter->edit();
  }

  /**
   * @copydoc ITGUIParameter::edit(ParamType const &) */
  inline std::unique_ptr<EditorType> edit(ParamType const &iValue)
  {
    return fParameter->edit(iValue);
  }

  /**
   * @copydoc IGUIParameter::getStepCount() */
  inline int32 getStepCount() const { return fParameter->getStepCount(); }

  /**
   * Allow to use the param as the underlying `ParamType`.
   *
   * Example: `if(param)` in the case `T` is `bool`
   */
  inline operator T() const { return getValue(); } // NOLINT

  /**
   * Allow to write `param = 3` instead of `param.update(3)` for example */
  inline void operator=(T const &iValue) { update(iValue); }

  /**
   * Allow to write `param1 == param2` to compare the underlying values */
  inline bool operator==(const class_type &rhs) const { return getValue() == rhs.getValue(); }

  /**
   * Allow to write `param1 != param2` to compare the underlying values */
  inline bool operator!=(const class_type &rhs) const { return getValue() != rhs.getValue(); }

  /**
   * @copydoc IGUIParameter::connect(Parameters::IChangeListener *) const */
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const
  {
    return fParameter->connect(iChangeListener);
  }

  /**
   * @copydoc IGUIParameter::connect(Parameters::ChangeCallback) const */
  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const
  {
    return fParameter->connect(iChangeCallback);
  }

private:
  std::shared_ptr<ITGUIParameter<T>> fParameter;
};


template<typename T>
using GUIOptionalParamEditor = std::unique_ptr<typename GUIOptionalParam<T>::EditorType>;

// TODO remove???
using GUIRawOptionalParam = GUIOptionalParam<ParamValue>;
using GUIRawOptionalParamEditor = GUIOptionalParamEditor<ParamValue>;

}