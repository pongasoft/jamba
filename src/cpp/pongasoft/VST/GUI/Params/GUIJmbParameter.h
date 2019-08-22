/*
 * Copyright (c) 2018-2019 pongasoft
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

#include <base/source/fobject.h>
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/Messaging.h>
#include <pongasoft/VST/MessageHandler.h>
#include <pongasoft/VST/MessageProducer.h>
#include "IGUIParameter.h"
#include "GUIParamCx.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Base class for a Jamba (Jmb) GUI parameter. This type of parameter is used when it cannot be mapped to a
 * Vst parameter whose internal representation must be a value in the range [0.0, 1.0]. For example a string (like
 * a user input label to name a component) does not fit in the Vst parameter category. By implementing
 * the serializable api (readFromStream/writeToStream), any type can be part of the state.
 */
class IGUIJmbParameter : public IMessageHandler
{
public:
  // Constructor
  explicit IGUIJmbParameter(std::shared_ptr<IJmbParamDef> iParamDef) : fParamDef{std::move(iParamDef)} {}

  // getParamDef
  inline IJmbParamDef const *getParamDef() const { return fParamDef.get(); }

  // getJmbParamID
  ParamID getJmbParamID() const { return fParamDef->fParamID; }

  // readFromStream
  virtual tresult readFromStream(IBStreamer &iStreamer) = 0;

  // writeToStream
  virtual tresult writeToStream(IBStreamer &oStreamer) const = 0;

  // readFromMessage
  virtual tresult readFromMessage(Message const &iMessage) = 0;

  // writeToMessage
  virtual tresult writeToMessage(Message &oMessage) const = 0;

  // writeToStream
  virtual void writeToStream(std::ostream &oStream) const = 0;

  // handleMessage
  tresult handleMessage(Message const &iMessage) override { return readFromMessage(iMessage); }

  // broadcast
  tresult broadcast() const;

  // setMessageProducer
  void setMessageProducer(IMessageProducer *iMessageProducer) { fMessageProducer = iMessageProducer; }

protected:
  std::shared_ptr<IJmbParamDef> fParamDef;
  IMessageProducer *fMessageProducer{};
};

/**
 * This is the templated version providing serializer methods, very similar to the GUIVstParameter concept.
 *
 * @tparam T the underlying type of the param */
template<typename T>
class GUIJmbParameter : public ITGUIParameter<T>, public IGUIJmbParameter, public FObject
{
public:
  using ParamType = T;
  using EditorType = typename ITGUIParameter<T>::ITEditor;

  using FObject::update; // fixes overload hiding warning

  // Constructor
  explicit GUIJmbParameter(std::shared_ptr<JmbParamDef<T>> iParamDef) :
    IGUIJmbParameter(iParamDef),
    FObject(),
    fValue{iParamDef->fDefaultValue}
  {}

  // getParamID
  ParamID getParamID() const override { return getJmbParamID(); }

  // getParamDef
  inline JmbParamDef<T> const *getParamDefT() const
  {
    return static_cast<JmbParamDef<T> const *>(getParamDef());
  }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(ParamType const &iValue) override
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

  /**
   * Resets the param to its default value */
  void resetToDefault()
  {
    setValue(getParamDefT()->fDefaultValue);
  }

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer) override
  {
    tresult res = getParamDefT()->readFromStream(iStreamer, fValue);
    if(res == kResultOk)
      changed();
    return res;
  }

  // writeToStream
  tresult writeToStream(IBStreamer &oStreamer) const override
  {
    return getParamDefT()->writeToStream(fValue, oStreamer);
  }

  // writeToStream
  void writeToStream(std::ostream &oStream) const override
  {
    getParamDefT()->writeToStream(fValue, oStream);
  }

  // readFromMessage
  tresult readFromMessage(Message const &iMessage) override
  {
    tresult res = getParamDefT()->readFromMessage(iMessage, fValue);
    if(res == kResultOk)
      changed();
    return res;
  }

  // writeToMessage
  tresult writeToMessage(Message &oMessage) const override
  {
    return getParamDefT()->writeToMessage(fValue, oMessage);
  }

  // getValue
  inline ParamType const &getValue() const override { return fValue; }

  // getValue
  inline ParamType &getValue() { return fValue; }

  // edit
  std::unique_ptr<EditorType> edit() override
  {
    return std::make_unique<DefaultEditorImpl<T>>(this);
  }

  /**
   * Importing other edit method from superclass
   */
  using ITGUIParameter<T>::edit;

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    return std::make_unique<GUIParamCx>(getJmbParamID(), const_cast<GUIJmbParameter *>(this), iChangeListener);
  }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    return std::make_unique<FObjectCxCallback>(const_cast<GUIJmbParameter *>(this), std::move(iChangeCallback));
  }

protected:
  ParamType fValue;
};

/**
 * Convenient function to cast a generic `IGUIParameter` to a `GUIJmbParameter<T>`. If not the proper type it will
 * return `nullptr`
 */
template<typename T>
static std::shared_ptr<GUIJmbParameter<T>> castToJmb(std::shared_ptr<IGUIParameter> iParam)
{
  return std::dynamic_pointer_cast<GUIJmbParameter<T>>(iParam);
}


//------------------------------------------------------------------------
// GUIJmbParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class GUIJmbParam
{
public:
  GUIJmbParam(std::shared_ptr<GUIJmbParameter<T>> iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{iPtr}
  {}

  // exists
  inline bool exists() const { return fPtr != nullptr; }

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes.
   */
  inline bool update(T const &iNewValue) { return fPtr->update(iNewValue); }

  /**
   * Use this flavor of update if you want to modify the value itself. ValueModifier will be called
   * back with &fValue. The callback should return true when the value was updated, false otherwise
   */
  template<class ValueModifier>
  inline bool updateIf(ValueModifier const &iValueModifier) { return fPtr->updateIf(iValueModifier); }

  /**
   * The difference with update is that it does not check for equality (case when T is not comparable)
   */
  inline void setValue(T const &iNewValue) { fPtr->setValue(iNewValue); }

  /**
   * The difference with update is that it does not check for equality (case when T is not comparable)
   */
  inline void setValue(T &&iNewValue) { fPtr->setValue(std::move(iNewValue)); }

  /**
   * Resets the param to its default value */
  inline void resetToDefault() { fPtr->resetToDefault(); }

  // getValue
  inline T const &getValue() const { return fPtr->getValue(); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T const &() const { return fPtr->getValue(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T const *operator->() const { return &fPtr->getValue(); }

  // broadcast
  inline tresult broadcast() const { return fPtr->broadcast(); }

  // broadcast
  inline void broadcast(T const &iValue)
  {
    setValue(iValue);
    broadcast();
  }

  // broadcast
  inline void broadcast(T &&iValue)
  {
    setValue(std::move(iValue));
    broadcast();
  }

  // connect
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) { return fPtr->connect(iChangeListener); }

  // connect
  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) { return fPtr->connect(std::move(iChangeCallback)); }

private:
  std::shared_ptr<GUIJmbParameter<T>> fPtr;
};

}
}
}
}