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
#include "GUIVstParameter.h"
#include "GUIJmbParameter.h"
#include "GUIValParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

using namespace Steinberg::Vst;

template<typename T, typename TGUIParameter>
class InternalEditorImpl
{
public:
  explicit InternalEditorImpl(TGUIParameter *iGUIParameter) :
    fGUIParameter{iGUIParameter},
    fInitialValue{iGUIParameter->getValue()}
  {
  }

  bool update(T const &iValue)
  {
    if(fDoneEditing)
      return false;
    return fGUIParameter->update(iValue);
  }

  tresult setValue(T const &iValue)
  {
    if(fDoneEditing)
      return kResultFalse;
    fGUIParameter->update(iValue);
    return kResultOk;
  }

  tresult commit()
  {
    if(fDoneEditing)
      return kResultFalse;
    fDoneEditing = true;
    return kResultOk;
  }

  tresult rollback()
  {
    auto res = setValue(fInitialValue);
    fDoneEditing = true;
    return res;
  }

private:
  TGUIParameter *fGUIParameter;
  T fInitialValue;
  bool fDoneEditing{false};
};

template<typename T, typename TGUIVstParameter = GUIVstParameter<T>>
class GUIOptionalParam
{
public:
  using class_type = GUIOptionalParam<T, TGUIVstParameter>;
  using ParamType = T;

  class Editor
  {
  public:
    bool update(ParamType const &iValue)
    {
      if(fVstEditorPtr) return fVstEditorPtr->update(iValue);
      if(fJmbEditorPtr) return fJmbEditorPtr->update(iValue);
      if(fValEditorPtr) return fValEditorPtr->update(iValue);
      return false;
    }

    tresult setValue(ParamType const &iValue)
    {
      if(fVstEditorPtr) return fVstEditorPtr->setValue(iValue);
      if(fJmbEditorPtr) return fJmbEditorPtr->setValue(iValue);
      if(fValEditorPtr) return fValEditorPtr->setValue(iValue);
      return kResultFalse;
    }

    tresult commit()
    {
      if(fVstEditorPtr) return fVstEditorPtr->commit();
      if(fJmbEditorPtr) return fJmbEditorPtr->commit();
      if(fValEditorPtr) return fValEditorPtr->commit();
      return kResultFalse;
    }

    tresult commit(ParamType const &iValue)
    {
      auto res = setValue(iValue);
      if(res == kResultOk)
        return commit();
      else
        return res;
    }

    tresult rollback()
    {
      if(fVstEditorPtr) return fVstEditorPtr->rollback();
      if(fJmbEditorPtr) return fJmbEditorPtr->rollback();
      if(fValEditorPtr) return fValEditorPtr->rollback();
      return kResultFalse;
    }

    friend class GUIOptionalParam<T, TGUIVstParameter>;

    Editor(Editor const &) = delete;

  private:
    explicit Editor(std::unique_ptr<typename TGUIVstParameter::Editor> iVstEditorPtr) : fVstEditorPtr{std::move(iVstEditorPtr)} {}
    explicit Editor(std::unique_ptr<InternalEditorImpl<T, GUIJmbParameter<T>>> iJmbEditorPtr) : fJmbEditorPtr{std::move(iJmbEditorPtr)} {}
    explicit Editor(std::unique_ptr<InternalEditorImpl<T, GUIValParameter<T>>> iValEditorPtr) : fValEditorPtr{std::move(iValEditorPtr)} {}

  private:
    std::unique_ptr<typename TGUIVstParameter::Editor> fVstEditorPtr{};
    std::unique_ptr<InternalEditorImpl<T, GUIJmbParameter<T>>> fJmbEditorPtr{};
    std::unique_ptr<InternalEditorImpl<T, GUIValParameter<T>>> fValEditorPtr{};
  };

public:
  GUIOptionalParam() : fValPtr{UNDEFINED_TAG_ID, T{}} {}

  explicit GUIOptionalParam(T const &iDefaultValue) : fValPtr{UNDEFINED_TAG_ID, iDefaultValue} {}

  // delete copy constructor
  GUIOptionalParam(class_type &iPtr) = delete;

  // move copy constructor
  GUIOptionalParam(class_type &&iPtr) noexcept = delete;

  // move assignment constructor
  GUIOptionalParam &operator=(class_type &&iPtr) noexcept = delete;

  // getTagID
  inline TagID getTagID() const
  {
    if(fVstPtr) return fVstPtr->getParamID();
    if(fJmbPtr) return fJmbPtr->getParamID();
    return fValPtr.getTagID();
  }

  /**
   * @return the current value of the parameter as a T
   */
  ParamType getValue() const
  {
    if(fVstPtr) return fVstPtr->getValue();
    if(fJmbPtr) return fJmbPtr->getValue();
    return fValPtr.getValue();
  }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes if the value actually changes.
   */
  inline bool update(ParamType const &iNewValue)
  {
    if(fVstPtr) return fVstPtr->update(iNewValue);
    if(fJmbPtr) return fJmbPtr->update(iNewValue);
    return fValPtr.update(iNewValue);
  }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes whether the value changes or not.
   */
  tresult setValue(ParamType const &iValue)
  {
    if(fVstPtr) return fVstPtr->setValue(iValue);
    if(fJmbPtr) return fJmbPtr->setValue(iValue);
    return fValPtr.setValue(iValue);
  }

  std::unique_ptr<Editor> edit()
  {
    if(fVstPtr) return std::unique_ptr<Editor>(new Editor(fVstPtr->edit()));
    if(fJmbPtr) return std::unique_ptr<Editor>(new Editor(std::make_unique<InternalEditorImpl<T, GUIJmbParameter<T>>>(fJmbPtr)));
    return std::unique_ptr<Editor>(new Editor(std::make_unique<InternalEditorImpl<T, GUIValParameter<T>>>(&fValPtr)));
  }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const
  {
    if(fVstPtr) return fVstPtr->getStepCount();
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
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener)
  {
    if(fVstPtr) return fVstPtr->connect(iChangeListener);
    if(fJmbPtr) return fJmbPtr->connect(iChangeListener);
    return fValPtr.connect(iChangeListener);
  }

public:
  friend class GUIParamCxMgr;

protected:
  bool assign(std::unique_ptr<TGUIVstParameter> &&iPtr)
  {
    DCHECK_F(iPtr != nullptr);

    fJmbPtr = nullptr;

    if(fVstPtr && fVstPtr->getParamID() == iPtr->getParamID())
      return false;
    fVstPtr = std::move(iPtr);
    return true;
  }

  bool assign(GUIJmbParameter<T> *iPtr)
  {
    DCHECK_F(iPtr != nullptr);

    fVstPtr = nullptr;

    if(fJmbPtr == iPtr)
      return false;

    fJmbPtr = iPtr;
    return true;
  }

  bool clearAssignment(TagID iTagID, bool iKeepCurrentValue = true)
  {
    fValPtr.setTagID(iTagID);

    if(iKeepCurrentValue)
    {
      fValPtr.update(getValue());
    }

    bool res = fVstPtr != nullptr || fJmbPtr != nullptr;
    fVstPtr = nullptr;
    fJmbPtr = nullptr;

    return res;
  }
private:
  // YP Note: should be using std::variant.. sadly at this time, it is a C++17 feature
  // AND requires XCode10.4 on macOS
  std::unique_ptr<TGUIVstParameter> fVstPtr{};
  GUIJmbParameter<T> *fJmbPtr{};
  
  // Represents a simple value (not tied to a vst or jmb parameter)
  GUIValParameter<T> fValPtr;
};

using GUIRawOptionalParam = GUIOptionalParam<ParamValue, GUIRawVstParameter>;

template<typename T, typename TGUIVstParameter = GUIVstParameter<T>>
using GUIOptionalParamEditor = std::unique_ptr<typename GUIOptionalParam<T, TGUIVstParameter>::Editor>;

using GUIRawOptionalParamEditor = std::unique_ptr<typename GUIOptionalParam<ParamValue, GUIRawVstParameter>::Editor>;

}
}
}
}