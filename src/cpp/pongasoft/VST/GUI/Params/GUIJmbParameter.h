/*
 * Copyright (c) 2018-2020 pongasoft
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
#include <pongasoft/Utils/Metaprogramming.h>
#include <pongasoft/Utils/Operators.h>
#include "IGUIParameter.h"
#include "GUIParamCx.h"

namespace pongasoft::VST::GUI::Params {

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
  {
//    DLOG_F(INFO, "GUIJmbParameter(%p)", this);
  }

  // Destructor
  ~GUIJmbParameter() override
  {
//    DLOG_F(INFO, "~GUIJmbParameter(%p)", this);
  }

  // getParamID
  ParamID getParamID() const override { return getJmbParamID(); }

  inline int32 getStepCount() const override { return 0; }

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
    // Implementation note: because this method is declared virtual the compiler must instantiate it
    // even if never called and will generate an error if the ParamType does is not copy assignable, so we
    // need to account for this case
    if constexpr(std::is_copy_assignable_v<ParamType>)
    {
      fValue = iValue;
      changed();
      return kResultOk;
    }
    else
    {
      DLOG_F(ERROR, "%s is not copy_assignable. Call updateIf instead.", typeid(ParamType).name());
      return kResultFalse;
    }
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
  tresult resetToDefault() override
  {
    return setValue(getParamDefT()->fDefaultValue);
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

  // toUTF8String
  std::string toUTF8String(int32 iPrecision) const override
  {
    return getParamDefT()->toUTF8String(getValue(), iPrecision);
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

  // edit
  std::unique_ptr<EditorType> edit() override
  {
    return std::make_unique<DefaultEditorImpl<T>>(this, getValue());
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

  // asDiscreteParameter
  std::shared_ptr<GUIDiscreteParameter> asDiscreteParameter(int32 iStepCount) override;

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

/**
 *  Wraps a `GUIJmbParameter<T>` to interpret it as a discrete parameter using the converter
 */
template<typename T>
class GUIDiscreteJmbParameter : public GUIDiscreteParameter
{
public:
  using EditorType = typename GUIDiscreteParameter::ITEditor;

public:
  GUIDiscreteJmbParameter(std::shared_ptr<GUIJmbParameter<T>> iJmbParameter,
                          std::shared_ptr<IDiscreteConverter<T>> iConverter) :
    fJmbParameter{std::move(iJmbParameter)},
    fConverter{iConverter}
  {
    DCHECK_F(fJmbParameter != nullptr);
    DCHECK_F(fConverter != nullptr);
  }

  // getParamID
  ParamID getParamID() const override
  {
    return fJmbParameter->getParamID();
  }

  // getStepCount
  int32 getStepCount() const override
  {
    return fConverter->getStepCount();
  }

  // connect
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    return fJmbParameter->connect(iChangeListener);
  }

  // connect
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    return fJmbParameter->connect(iChangeCallback);
  }

  // asDiscreteParameter
  std::shared_ptr <GUIDiscreteParameter> asDiscreteParameter(int32 iStepCount) override
  {
    return fJmbParameter->asDiscreteParameter(iStepCount);
  }

  /**
   * Because converting the current Jmb value to an `int32` may fail this api returns `kResultOk` if it works in which
   * case, `oDiscreteValue` contains the value. Otherwise it returns `kResultFalse` and `oDiscreteValue` is left
   * untouched.
   */
  tresult getValue(int32 &oDiscreteValue) const
  {
    auto res = fConverter->convertToDiscreteValue(fJmbParameter->getValue(), oDiscreteValue);
  #ifndef NDEBUG
    if(res == kResultFalse)
      DLOG_F(WARNING, "Cannot convert current value of Jmb param [%d] to a discrete value", getParamID());
  #endif
    return res;
  }

  // accessValue
  tresult accessValue(ValueAccessor const &iGetter) const override
  {
    int32 discreteValue;
    if(getValue(discreteValue) == kResultOk)
    {
      iGetter(discreteValue);
      return kResultOk;
    }
    return kResultFalse;
  }

  // update
  bool update(int32 const &iDiscreteValue) override
  {
    int32 currentDiscreteValue;
    tresult res = getValue(currentDiscreteValue);

    if(res == kResultOk)
    {
      if(iDiscreteValue != currentDiscreteValue)
      {
        if(setValue(iDiscreteValue) == kResultOk)
          return true;
      }
    }

    return false;
  }

  // setValue
  tresult setValue(int32 const &iDiscreteValue) override
  {
    int32 currentDiscreteValue;
    tresult res = getValue(currentDiscreteValue);
    if(res == kResultOk)
    {
      if(iDiscreteValue != currentDiscreteValue)
      {
        res = kResultFalse;
        if constexpr(std::is_copy_assignable_v<T>)
        {
          T jmbValue = fJmbParameter->getValue();
          if(fConverter->convertFromDiscreteValue(iDiscreteValue, jmbValue) == kResultOk)
            res = fJmbParameter->setValue(jmbValue);
          else
            DLOG_F(WARNING, "Cannot convert discrete value [%d] to a [%s] value of Jmb param [%d]",
              iDiscreteValue,
              typeid(T).name(),
              getParamID());
        }
      }
    }

    return res;
  }

  // resetToDefault
  tresult resetToDefault() override
  {
    return fJmbParameter->resetToDefault();
  }

  // edit
  std::unique_ptr<EditorType> edit() override
  {
    int32 currentDiscreteValue;
    tresult res = getValue(currentDiscreteValue);
    if(res == kResultOk)
      return std::make_unique<DefaultEditorImpl<int32>>(this, currentDiscreteValue);
    else
      return nullptr;
  }

  // toUTF8String
  std::string toUTF8String(int32 iPrecision) const override
  {
    return fJmbParameter->toUTF8String(iPrecision);
  }

protected:
  std::shared_ptr<GUIJmbParameter<T>> fJmbParameter;
  std::shared_ptr<IDiscreteConverter<T>> fConverter;
};

//------------------------------------------------------------------------
// GUIJmbParameter<T>::asDiscreteParameter
//------------------------------------------------------------------------
template<typename T>
std::shared_ptr<GUIDiscreteParameter> GUIJmbParameter<T>::asDiscreteParameter(int32 iStepCount)
{
  // Step 1: check if the Jmb param provide a discrete converter
  auto converter = getParamDefT()->getDiscreteConverter();

  if(converter && converter->getStepCount() > 0)
    return std::make_shared<GUIDiscreteJmbParameter<T>>(std::dynamic_pointer_cast<GUIJmbParameter<T>>(ITGUIParameter<T>::shared_from_this()),
                                                        std::move(converter));
  else
  {
    // Step 2: no discrete converter, so we check if T can be automatically converted to an int32 (case where
    // T is a numeric value, an enum, or a type that offers a "constructor(int32)" and "operator int32()" methods)
    if(iStepCount > 0)
    {
      if constexpr(Utils::is_static_cast_defined<int32, T> && Utils::is_static_cast_defined<T, int32>)
      {
        converter = std::make_shared<StaticCastDiscreteConverter<T>>(iStepCount);
        return std::make_shared<GUIDiscreteJmbParameter<T>>(std::dynamic_pointer_cast<GUIJmbParameter<T>>(ITGUIParameter<T>::shared_from_this()),
                                                            std::move(converter));
      }
      else
      {
        DLOG_F(WARNING, "Jmb param [%d] (type [%s]) cannot be converted to a discrete parameter",
               getParamID(),
               Utils::typeString<T>().c_str());
      }
    }
    return nullptr;
  }
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
class GUIJmbParam: public Utils::Operators::Dereferenceable<GUIJmbParam<T>>
{
public:
  using ParamType = T;
  using EditorType = typename GUIJmbParameter<T>::EditorType;
  using Editor = std::unique_ptr<EditorType>;

public:
  GUIJmbParam(std::shared_ptr<GUIJmbParameter<T>> iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{std::move(iPtr)}
  {}

  //! Assignment operator: `fMyParam = registerParam(...);`
  GUIJmbParam<T> &operator=(GUIJmbParam<T> const &iOther) = default;

  // exists
  inline bool exists() const { return fPtr != nullptr; }

  // getParamID
  inline ParamID getParamID() const { DCHECK_F(exists()); return fPtr->getParamID(); }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes.
   */
  inline bool update(T const &iNewValue) { DCHECK_F(exists()); return fPtr->update(iNewValue); }

  /**
   * Use this flavor of update if you want to modify the value itself. ValueModifier will be called
   * back with &fValue. The callback should return true when the value was updated, false otherwise
   */
  template<class ValueModifier>
  inline bool updateIf(ValueModifier const &iValueModifier) { DCHECK_F(exists()); return fPtr->updateIf(iValueModifier); }

  /**
   * The difference with update is that it does not check for equality (case when T is not comparable)
   */
  inline void setValue(T const &iNewValue) { DCHECK_F(exists()); fPtr->setValue(iNewValue); }

  /**
   * The difference with update is that it does not check for equality (case when T is not comparable)
   */
  inline void setValue(T &&iNewValue) { DCHECK_F(exists()); fPtr->setValue(std::move(iNewValue)); }

  /**
   * @return the editor to change the parameter (commit/rollback) */
  inline Editor edit() { DCHECK_F(exists()); return fPtr->edit(); }

  /**
   * @param iValue first value to set
   * @return the editor to change the parameter (commit/rollback) */
  inline Editor edit(T const &iValue) { DCHECK_F(exists()); return fPtr->edit(iValue); }

  /**
   * Resets the param to its default value */
  inline tresult resetToDefault() { DCHECK_F(exists()); return fPtr->resetToDefault(); }

  // getValue
  inline T const & getValue() const { DCHECK_F(exists()); return fPtr->getValue(); }

  //! Synonym to `getValue()`
  inline T const & value() const { DCHECK_F(exists()); return fPtr->getValue(); }

  // allow to use the param as the underlying `ParamType` (ex: `if(param)` in the case `ParamType` is `bool`))
  [[deprecated("Since 4.1.0 -  use operator* or .value() instead (ex: if(*param) {...} or if(param.value()) {...}")]]
  inline operator T const &() const { DCHECK_F(exists()); return fPtr->getValue(); } // NOLINT

  //! allow writing *param to access the underlying value (or in other words, `*param` is the same `param.value()`)
  constexpr T const & operator *() const { DCHECK_F(exists()); return fPtr->getValue(); }

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  constexpr T const * operator->() const { DCHECK_F(exists()); return &fPtr->getValue(); }

  //! Allow to write param = 3.0
  inline GUIJmbParam<T> &operator=(T const &iValue) { DCHECK_F(exists()); fPtr->setValue(iValue); return *this; }

  // broadcast
  inline tresult broadcast() const { DCHECK_F(exists()); return fPtr->broadcast(); }

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

  /**
   * @copydoc IGUIParameter::toUTF8String() */
  inline std::string toUTF8String(int32 iPrecision) const { DCHECK_F(exists()); return fPtr->toUTF8String(iPrecision); }

  // connect
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) { DCHECK_F(exists()); return fPtr->connect(iChangeListener); }

  // connect
  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) { DCHECK_F(exists()); return fPtr->connect(std::move(iChangeCallback)); }

private:
  std::shared_ptr<GUIJmbParameter<T>> fPtr;
};

}