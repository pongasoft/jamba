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
#include <pongasoft/Utils/Metaprogramming.h>
#include "GUIParamCx.h"

namespace pongasoft::VST::GUI::Params {

using namespace Steinberg::Vst;

/**
 * This parameter is not tied to any parameter definition/registration and is primarily used by the optional
 * parameter.
 */
template<typename T>
class GUIValParameter: public ITGUIParameter<T>, public FObject
{
public:
  using ParamType = T;
  using EditorType = typename ITGUIParameter<T>::ITEditor;

  using FObject::update; // fixes overload hiding warning

public:
  explicit GUIValParameter(TagID iTagID, ParamType const &iDefaultValue) :
    fTagID{iTagID},
    fValue(iDefaultValue)
  {};

  explicit GUIValParameter(TagID iTagID, ParamType &&iDefaultValue) :
    fTagID{iTagID},
    fValue(std::move(iDefaultValue))
  {};

  // getTagID
  TagID getTagID() const { return fTagID; }
  void setTagID(TagID iTagID) { fTagID = iTagID; }

  /**
   * In order to comply with the api, we return 0 when tag id is undefined... */
  ParamID getParamID() const override
  {
    if(fTagID < 0)
      return 0;
    return static_cast<ParamID>(fTagID);
  }

  inline int32 getStepCount() const override { return 0; }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(ParamType const &iValue) override
  {
    // Implementation note: because this method is declared virtual the compiler must instantiate it
    // even if never called and will generate an error if the ParamType does not define operator!=, so we
    // need to account for this case
    if constexpr(Utils::is_operator_not_eq_defined<ParamType>)
    {
      if(fValue != iValue)
      {
        if(setValue(iValue) == kResultOk)
          return true;
      }
    }
    else
    {
      if(setValue(iValue) == kResultOk)
        return true;
    }
    return false;
  }

  /**
   * @return an editor to edit modify the value
   */
  std::unique_ptr<EditorType> edit() override
  {
    return std::make_unique<DefaultEditorImpl<T>>(this, getValue());
  }

  /**
   * Use this flavor of update if you want to modify the value itself.
   * ValueModifier will be called back with &fValue (of type `T *`).
   * The callback should return true when the value was updated, false otherwise
   */
  template<class ValueModifier>
  bool updateIf(ValueModifier const &iValueModifier)
  {
    if(iValueModifier(&fValue))
    {
      changed();
      return true;
    }
    return false;
  }

  /**
   * Sets the value. The difference with update is that it does not check for equality (case when ParamType is
   * not comparable)
   */
  tresult setValue(ParamType const &iValue) override
  {
    fValue = iValue;
    changed();
    return kResultOk;
  }

  /**
   * Sets the value. The difference with update is that it does not check for equality (case when ParamType is
   * not comparable)
   */
  tresult setValue(ParamType &&iValue)
  {
    fValue = std::move(iValue);
    changed();
    return kResultOk;
  }

  // accessValue
  void accessValue(typename ITGUIParameter<T>::ValueAccessor const &iGetter) const override
  {
    iGetter(fValue);
  }

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // getValue
  inline ParamType &getValue() { return fValue; }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    if(getTagID() >= 0)
      return std::make_unique<GUIParamCx>(static_cast<ParamID>(getTagID()), const_cast<GUIValParameter *>(this), iChangeListener);
    else
      return nullptr;
  }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    if(getTagID() >= 0)
      return std::make_unique<FObjectCxCallback>(const_cast<GUIValParameter *>(this), iChangeCallback);
    else
      return nullptr;
  }

  // asDiscreteParameter
  std::shared_ptr<ITGUIParameter<int32>> asDiscreteParameter(int32 iStepCount) override
  {
    return nullptr;
  }

private:
  TagID fTagID;
  T fValue;
};

}
