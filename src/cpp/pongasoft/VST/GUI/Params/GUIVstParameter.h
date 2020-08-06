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

#include "IGUIParameter.h"
#include "GUIRawVstParameter.h"
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>
#include <pongasoft/Utils/Operators.h>

namespace pongasoft::VST::GUI::Params {

/**
 * This class wraps a GUIRawVstParameter to deal with any type T
 */
template<typename T>
class GUIVstParameter : public ITGUIParameter<T>
{
public:
  using ParamType = T;
  using EditorType = typename ITGUIParameter<T>::ITEditor;

public:
  /**
   * Wrapper to edit a single parameter. Usage:
   *
   * // from a CView::onMouseDown callback
   * fMyParamEditor = fParameter.edit(myParamID);
   * fParamEditor->setValue(myValue);
   *
   * // from a CView::onMouseMoved callback
   * fParamEditor->setValue(myValue);
   *
   * // from a CView::onMouseUp/onMouseCancelled callback
   * fMyParamEditor->commit();
   */
  class Editor : public EditorType
  {
  public:
    inline explicit Editor(GUIRawVstParamEditor iRawEditor,
                           std::shared_ptr<IParamConverter<T>> iConverter) :
      fRawEditor{std::move(iRawEditor)},
      fConverter{std::move(iConverter)}
    {
    }

    ~Editor() override { rollback(); }

    // disabling copy
    Editor(Editor const &) = delete;
    Editor& operator=(Editor const &) = delete;

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already
     */
    tresult setValue(ParamType const &iValue) override
    {
      return fRawEditor->setValue(fConverter->normalize(iValue));
    }

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already
     * @return `true` if the value was updated
     */
    bool updateValue(ParamType const &iValue) override
    {
      return fRawEditor->updateValue(fConverter->normalize(iValue));
    }

    /*
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    tresult commit() override
    {
      return fRawEditor->commit();
    }

    // importing superclass commit methods
    using EditorType::commit;

    /**
     * Call this if you want to revert to the original value of the parameter (when the editor is created).
     * This has no effect if commit() has already been called
     */
    tresult rollback() override
    {
      return fRawEditor->rollback();
    }

  private:
    GUIRawVstParamEditor fRawEditor;
    std::shared_ptr<IParamConverter<T>> fConverter;
  };

public:
  // Constructor
  GUIVstParameter(std::shared_ptr<GUIRawVstParameter> iRawParameter,
                  std::shared_ptr<IParamConverter<T>> iConverter) :
    fRawParameter{std::move(iRawParameter)},
    fConverter{std::move(iConverter)}
  {
    DCHECK_NOTNULL_F(fRawParameter.get());
    DCHECK_NOTNULL_F(fConverter.get());
    // DLOG_F(INFO, "VSTParameter::VSTParameter(%d)", fRawParameter->getParamID());
  }

  // Destructor
  ~GUIVstParameter()
  {
    // DLOG_F(INFO, "VSTParameter::~VSTParameter(%d)", fRawParameter->getParamID());
  }

  // getParamID
  ParamID getParamID() const override
  {
    return fRawParameter->getParamID();
  }

  // accessValue
  tresult accessValue(typename ITGUIParameter<T>::ValueAccessor const &iGetter) const override
  {
    iGetter(getValue());
    return kResultOk;
  }

  /**
   * @return the current value of the parameter as a T (using the Denormalizer)
   */
  ParamType getValue() const
  {
    return fConverter->denormalize(fRawParameter->getValue());
  }

  /**
   * @return the current value of the parameter as a normalized value
   */
  ParamValue getNormalizedValue() const
  {
    return fRawParameter->getValue();
  }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(ParamType const &iValue) override
  {
    // Implementation note: because ParamType may not define `operator!=` we use the normalized value instead
    auto currentNormalizedValue = getNormalizedValue();
    auto newNormalizedValued = fConverter->normalize(iValue);
    if(currentNormalizedValue != newNormalizedValued)
    {
      if(setNormalizedValue(newNormalizedValued) == kResultOk)
        return true;
    }
    return false;
  }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setValue(ParamType const &iValue) override
  {
    return fRawParameter->setValue(fConverter->normalize(iValue));
  }

  /**
   * Sets the value of this parameter as a normalized value. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setNormalizedValue(ParamValue const &iNormalizedValue)
  {
    return fRawParameter->setValue(iNormalizedValue);
  }

  /**
   * @copydoc IGUIParameter::resetToDefault() */
  tresult resetToDefault() override
  {
    return fRawParameter->resetToDefault();
  }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const override { return fConverter->getStepCount(); }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString)
  {
    fRawParameter->toString(oString);
  }

  /**
   * Returns a string representation of this parameter
   */
  Steinberg::String toString()
  {
    return fRawParameter->toString();
  }

  // toUTF8String
  std::string toUTF8String(int32 iPrecision) const override
  {
    return fConverter->toString(getValue(), iPrecision);
  }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<EditorType> edit() override
  {
    return std::make_unique<Editor>(fRawParameter->edit(), fConverter);
  }

  /**
   * Importing other edit method from superclass
   */
  using ITGUIParameter<T>::edit;

  /**
   * @return an object maintaining the connection between the parameter and the listener
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    return fRawParameter->connect(iChangeListener);
  }

  /**
   * @return an object maintaining the connection between the parameter and the callback
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    return fRawParameter->connect(std::move(iChangeCallback));
  }

  // asDiscreteParameter
  std::shared_ptr<GUIDiscreteParameter> asDiscreteParameter(int32 iStepCount) override
  {
    return fRawParameter->asDiscreteParameter(iStepCount);
  }

private:
  std::shared_ptr<GUIRawVstParameter> fRawParameter;
  std::shared_ptr<IParamConverter<T>> fConverter;
};

//------------------------------------------------------------------------
// GUIVstParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class GUIVstParam: public Utils::Operators::Dereferenceable<GUIVstParam<T>>
{
public:
  using ParamType = T;
  using EditorType = typename GUIVstParameter<T>::ITEditor;
  using Editor = std::unique_ptr<EditorType>;

public:
  /**
   * The purpose of this class is to copy the value so that it can be accessed via `->` thus allowing to
   * write `param->x` to access the underlying type (`T`) when it is a struct or a class
   */
  class Value {
  public:
    constexpr T const *operator ->() const { return &fValue; }
    friend class GUIVstParam<T>;
  private:
    explicit Value(T const &iValue) : fValue{iValue} {}
    T fValue;
  };

public:
  // Constructor
  GUIVstParam(std::shared_ptr<GUIVstParameter<T>> iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{std::move(iPtr)}
  {}

  //! Assignment operator: `fMyParam = registerParam(...);`
  GUIVstParam<T> &operator=(GUIVstParam<T> const &iOther) = default;

  // exists
  inline bool exists() const { return (bool) fPtr; }

  // getParamID
  inline ParamID getParamID() const { DCHECK_F(exists()); return fPtr->getParamID(); }

  /**
   * @return the current value of the parameter as a T (using the Denormalizer)
   */
  inline T getValue() const { DCHECK_F(exists()); return fPtr->getValue(); }

  //! Synonym to `getValue()`
  inline T value() const { DCHECK_F(exists()); return fPtr->getValue(); }

  /**
   * @return the current value of the parameter as a normalized value
   */
  inline ParamValue getNormalizedValue() const { DCHECK_F(exists()); return fPtr->getNormalizedValue(); }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(T const &iValue) { DCHECK_F(exists()); return fPtr->update(iValue); }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setValue(T const &iValue) { DCHECK_F(exists()); return fPtr->setValue(iValue); }

  /**
   * Sets the value of this parameter as a normalized value. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setNormalizedValue(ParamValue const &iNormalizedValue) { DCHECK_F(exists()); return fPtr->setNormalizedValue(iNormalizedValue); }

  /**
   * Resets the param to its default value */
  inline tresult resetToDefault() { DCHECK_F(exists()); return fPtr->resetToDefault(); }

  /**
   * Shortcut to copy the value from another param to this one. Implementation note: uses normalized value as this
   * is faster and avoid math precision loss in normalize/denormalize
   */
  template<typename V>
  tresult copyValueFrom(GUIVstParam<V> const &iParam) { DCHECK_F(exists()); return setNormalizedValue(iParam.getNormalizedValue()); }

  /**
   * Shortcut to copy the value from another param to this one (raw value)
   */
  tresult copyValueFrom(GUIRawVstParam const &iParam) { DCHECK_F(exists()); return setNormalizedValue(iParam.getValue()); }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const { DCHECK_F(exists()); return fPtr->getStepCount(); }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString) { DCHECK_F(exists()); fPtr->toString(oString); }

  /**
   * Returns a string representation of this parameter
   */
  Steinberg::String toString() { DCHECK_F(exists()); return fPtr->toString(); }

  /**
   * @copydoc IGUIParameter::toUTF8String() */
  inline std::string toUTF8String(int32 iPrecision) const { DCHECK_F(exists()); return fPtr->toUTF8String(iPrecision); }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  Editor edit() { DCHECK_F(exists()); return fPtr->edit(); }

  /**
   * Shortcut to create an editor and set the value to it
   *
   * @return an editor to modify the parameter (see Editor)
   */
  Editor edit(T const &iValue) { DCHECK_F(exists()); return fPtr->edit(iValue); }

  //! allow writing *param to access the underlying value (or in other words, `*param` is the same `param.value()`)
  constexpr T operator *() const { DCHECK_F(exists()); return fPtr->getValue(); }

  //! allow writing param->x to access the underlying value when T is a struct or class
  constexpr Value operator ->() const { DCHECK_F(exists()); return Value{fPtr->getValue()}; }

  //! Allow to use the param as the underlying ParamType (ex: `if(param)` in the case `ParamType` is `bool`))
  [[deprecated("Since 4.1.0 -  use operator* or .value() instead (ex: if(*param) {...} or if(param.value()) {...}")]]
  inline operator T() const { DCHECK_F(exists()); return fPtr->getValue(); } // NOLINT

  //! Allow to write param = 3.0
  inline GUIVstParam<T> &operator=(T const &iValue) { DCHECK_F(exists()); fPtr->setValue(iValue); return *this; }

  //! Allow to write param1 == param2
//  friend constexpr bool operator==(GUIVstParam<T> const &lhs, GUIVstParam<T> const &rhs) { return lhs.fPtr->getNormalizedValue() == rhs.fPtr->getNormalizedValue(); }

  /**
   * @return an object maintaining the connection between the parameter and the listener
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const { DCHECK_F(exists()); return fPtr->connect(iChangeListener); }

  /**
   * @return an object maintaining the connection between the parameter and the callback
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const { DCHECK_F(exists()); return fPtr->connect(std::move(iChangeCallback)); }

private:
  std::shared_ptr<GUIVstParameter<T>> fPtr;
};


//------------------------------------------------------------------------
// GUIRawVstParameter::asVstParameter
//------------------------------------------------------------------------
template<typename T>
std::shared_ptr<GUIVstParameter<T>> GUIRawVstParameter::asVstParameter()
{
  auto vstParamDef = std::dynamic_pointer_cast<VstParamDef<T>>(fParamDef);
  if(vstParamDef && vstParamDef->fConverter)
    return std::make_shared<GUIVstParameter<T>>(std::dynamic_pointer_cast<GUIRawVstParameter>(shared_from_this()),
                                                vstParamDef->fConverter);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// shortcut notations
//------------------------------------------------------------------------
template<typename T>
using GUIVstParamEditor = std::unique_ptr<typename GUIVstParameter<T>::EditorType>;

using GUIVstBooleanParam = GUIVstParam<bool>;
using GUIVstPercentParam = GUIVstParam<Percent>;

}
