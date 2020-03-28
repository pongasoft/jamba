/*
 * Copyright (c) 2019-2020 pongasoft
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
 *
 * @internal
 */
template<typename T>
class GUIValParameter: public ITGUIParameter<T>, public FObject
{
public:
  using ParamType = T;
  using EditorType = typename ITGUIParameter<T>::ITEditor;

  using FObject::update; // fixes overload hiding warning

public:
  // Constructor
  explicit GUIValParameter(ParamID iParamID, ParamType const &iDefaultValue) :
    fParamID{iParamID},
    fValue(iDefaultValue)
  {
//    DLOG_F(INFO, "GUIValParameter(%p)", this);
  };

  // Constructor
  explicit GUIValParameter(ParamID iParamID, ParamType &&iDefaultValue) :
    fParamID{iParamID},
    fValue(std::move(iDefaultValue))
  {
//    DLOG_F(INFO, "GUIValParameter(%p)", this);
  };

  // Destructor
  ~GUIValParameter() override
  {
//    DLOG_F(INFO, "~GUIValParameter(%p)", this);
  }

  // getParamID
  ParamID getParamID() const override { return fParamID; }
  void setParamID(ParamID iParamID) { fParamID = iParamID; }

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
  tresult accessValue(typename ITGUIParameter<T>::ValueAccessor const &iGetter) const override
  {
    iGetter(fValue);
    return kResultOk;
  }

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // getValue
  inline ParamType &getValue() { return fValue; }

  // toUTF8String
  std::string toUTF8String(int32 iPrecision) const override
  {
    return VstUtils::toUTF8String(getValue(), iPrecision);
  }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    return std::make_unique<GUIParamCx>(getParamID(), const_cast<GUIValParameter *>(this), iChangeListener);
  }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    return std::make_unique<FObjectCxCallback>(const_cast<GUIValParameter *>(this), iChangeCallback);
  }

  // asDiscreteParameter
  std::shared_ptr<GUIDiscreteParameter> asDiscreteParameter(int32 iStepCount) override;

protected:
  ParamID fParamID;
  T fValue;
};

/**
 * Simple extension class to treat a Val parameter as a discrete one.
 *
 * @note `GUIValParameter` is internal and not shared across views. As a result this implementation
 *       does not wrap another `GUIValParameter` (like `GUIJmbParameter` implementation does) which suffices
 *       for the current need. If `GUIValParameter` gets promoted to api level at a later date,
 *       then this implementation will need to be changed.
 *
 * @internal
 */
class GUIDiscreteValParameter : public GUIValParameter<int32>
{
public:
  GUIDiscreteValParameter(ParamID iParamID, int32 iDefaultValue, int32 iStepCount) :
    GUIValParameter(iParamID, iDefaultValue),
    fStepCount(iStepCount)
  {
    DCHECK_F(fStepCount > 0);
  }

  // getStepCount
  int32 getStepCount() const override { return fStepCount; }

protected:
  int32 fStepCount;
};

//------------------------------------------------------------------------
// GUIValParameter::asDiscreteParameter
//------------------------------------------------------------------------
template<typename T>
std::shared_ptr<GUIDiscreteParameter> GUIValParameter<T>::asDiscreteParameter(int32 iStepCount)
{
  if(iStepCount > 0)
  {
    if constexpr(Utils::is_static_cast_defined<int32, T> && Utils::is_static_cast_defined<T, int32>)
    {
      return VstUtils::make_sfo<GUIDiscreteValParameter>(fParamID, static_cast<int32>(fValue), iStepCount);
    }
  }
  return nullptr;
}

}
