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

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

using namespace Steinberg::Vst;

template<typename T>
class GUIValParameter: public FObject
{
public:
  using ParamType = T;

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
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(ParamType const &iValue)
  {
    if(fValue != iValue)
    {
      fValue = iValue;
      changed();
      return true;
    }
    return false;
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
  tresult setValue(ParamType const &iValue)
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

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // getValue
  inline ParamType &getValue() { return fValue; }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener)
  {
    if(getTagID() >= 0)
      return std::make_unique<GUIParamCx>(static_cast<ParamID>(getTagID()), this, iChangeListener);
    else
      return nullptr;
  }

private:
  TagID fTagID;
  T fValue;
};

}
}
}
}
