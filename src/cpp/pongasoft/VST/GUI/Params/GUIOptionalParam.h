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
 * TODO doc.
 * Because `GUIOptional` can represent a Vst parameter or a Jmb parameter, there are some restrictions on `T` itself
 * (which do not exist if you use `GUIJmbParam<T>`):
 * - `T` must have empty constructor: `T()`
 * - `T` have a copy constructor: `T(T const &)`
 * - `T` must be copy assignable: `T& operator=(T const &)`
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
  GUIOptionalParam() :
    fParameter{std::make_shared<GUIValParameter<T>>(UNDEFINED_TAG_ID, T{})} {}

  explicit GUIOptionalParam(T const &iDefaultValue) :
    fParameter{std::make_shared<GUIValParameter<T>>(UNDEFINED_TAG_ID, iDefaultValue)} {}

  // delete copy constructor
  GUIOptionalParam(class_type &iPtr) = delete;

  // move copy constructor
  GUIOptionalParam(class_type &&iPtr) noexcept = delete;

  // move assignment constructor
  GUIOptionalParam &operator=(class_type &&iPtr) noexcept = delete;

  // getTagID
  inline TagID getTagID() const
  {
    auto valParameter = dynamic_cast<GUIValParameter<T> *>(fParameter.get());
    if(valParameter)
      return valParameter->getTagID();
    else
      return fParameter->getParamID();
  }

  /**
   * @return the current value of the parameter as a T
   */
  inline ParamType getValue() const
  {
    ParamType res;
    fParameter->accessValue([&res](auto const &iValue) { res = iValue; });
    return res;
  }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes if the value actually changes.
   */
  inline bool update(ParamType const &iNewValue)
  {
    return fParameter->update(iNewValue);
  }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes whether the value changes or not.
   */
  inline tresult setValue(ParamType const &iValue)
  {
    return fParameter->setValue(iValue);
  }

  /**
   * Returns an editor to modify the parameter before commit or rollback
   */
  inline std::unique_ptr<EditorType> edit()
  {
    return fParameter->edit();
  }

  /**
   * Returns an editor to modify the parameter before commit or rollback
   */
  inline std::unique_ptr<EditorType> edit(ParamType const &iValue)
  {
    return fParameter->edit(iValue);
  }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const
  {
    return 0;
  }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case T is bool))
  inline operator T() const { return getValue(); } // NOLINT

  // allow to write param = 3 instead of param.update(3)
  inline void operator=(T const &iValue) { update(iValue); }

  // allow to write param1 == param2
  inline bool operator==(const class_type &rhs) const { return getValue() == rhs.getValue(); }

  // allow to write param1 != param2
  inline bool operator!=(const class_type &rhs) const { return getValue() != rhs.getValue(); }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const
  {
    return fParameter->connect(iChangeListener);
  }

  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const
  {
    return fParameter->connect(iChangeCallback);
  }

public:
  friend class GUIParamCxMgr;

protected:
  void assign(std::shared_ptr<ITGUIParameter<T>> iParameter)
  {
    DCHECK_F(iParameter != nullptr);
    fParameter = std::move(iParameter);
  }

  void clearAssignment(TagID iTagID)
  {
    fParameter = std::make_shared<GUIValParameter<T>>(iTagID, getValue());
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