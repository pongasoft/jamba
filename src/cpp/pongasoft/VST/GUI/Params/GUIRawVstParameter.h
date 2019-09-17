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

#include "IGUIParameter.h"
#include <pongasoft/VST/Parameters.h>
#include "VstParameters.h"
#include "GUIParamCx.h"

#include <string>

namespace pongasoft::VST::GUI::Params {

template<typename T>
class GUIVstParameter;

/**
 * Encapsulates a vst parameter and how to access it (read/write) as well as how to "connect" to it in order to be
 * notified of changes. This "raw" version deals with ParamValue which is the underlying type used by the vst sdk
 * which is always a number in the range [0.0, 1.0]. The class VSTParameter deals with other types and automatic
 * normalization/denormalization.
 */
class GUIRawVstParameter : public ITGUIParameter<ParamValue>
{
public:
  using ParamType = ParamValue;
  using EditorType = ITGUIParameter<ParamValue>::ITEditor;

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
    Editor(ParamID iParamID, VstParametersSPtr iVstParameters);

    ~Editor() override { rollback(); }

    // disabling copy
    Editor(Editor const &) = delete;
    Editor& operator=(Editor const &) = delete;

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already
     */
    tresult setValue(ParamValue const &iValue) override;

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already.
     * @return `true` if the value was updated
     */
    bool updateValue(ParamValue const &iValue) override;

    /*
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    tresult commit() override;

    // importing superclass commit methods
    using EditorType::commit;

    /**
     * Call this if you want to revert to the original value of the parameter (when the editor is created).
     * This has no effect if commit() has already been called
     */
    tresult rollback() override;

  private:
    ParamID fParamID;
    VstParametersSPtr fVstParameters;

    ParamValue fInitialParamValue;
    bool fIsEditing;
  };

public:
  // Constructor
  GUIRawVstParameter(ParamID iParamID,
                     VstParametersSPtr iVstParameters,
                     std::shared_ptr<RawVstParamDef> iParamDef);

  // Destructor
  ~GUIRawVstParameter() = default;
//  {
//    DLOG_F(INFO, "RawParameter::~RawParameter(%d)", fParamID);
//  }

  // getParamID
  inline ParamID getParamID() const override
  {
    return fParamID;
  }

  // accessValue
  tresult accessValue(ValueAccessor const &iGetter) const override
  {
    iGetter(getValue());
    return kResultOk;
  }

  /**
   * @return the current raw value of the parameter
   */
  inline ParamValue getValue() const
  {
    return fVstParameters->getParamNormalized(fParamID);
  }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const override
  {
    auto parameterInfo = fVstParameters->getParameterInfo(fParamID);
    if(parameterInfo)
      return parameterInfo->stepCount;
    else
      return 0;
  }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString)
  {
    fParamDef->toString(getValue(), oString);
  }

  /**
   * Returns a string representation of this parameter
   */
  String toString()
  {
    String128 s;
    toString(s);
    return String(s);
  }

  // toUTF8String
  std::string toUTF8String(int32 iPrecision) const override
  {
    return fParamDef->toUTF8String(getValue(), iPrecision);
  }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(ParamValue const &iValue) override
  {
    auto const previousValue = getValue();
    if(previousValue != iValue)
    {
      setValue(iValue);
      return true;
    }
    return false;
  }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  inline tresult setValue(ParamValue const &iValue) override
  {
    Editor editor(fParamID, fVstParameters);
    editor.setValue(iValue);
    return editor.commit();
  }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<EditorType> edit() override
  {
    return std::make_unique<Editor>(fParamID, fVstParameters);
  }

  /**
 * Importing other edit method from superclass
 */
  using ITGUIParameter<ParamValue>::edit;

  /**
   * @return an object maintaining the connection between the parameter and the listener
   */
  std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const override
  {
    return fVstParameters->connect(fParamID, iChangeListener);
  }

  /**
   * @return an object maintaining the connection between the parameter and the callback
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const override
  {
    return fVstParameters->connect(fParamID, std::move(iChangeCallback));
  }

  /**
   * Converts to a typed parameter
   * @return `nullptr` if the underlying parameter is not of proper type
   */
  template<typename T>
  std::shared_ptr<GUIVstParameter<T>> asVstParameter();

  /**
   * This implementation will adapt this parameter to be interpreted as a discrete parameter. If it is already
   * a discrete parameter, then `iStepCount` is ignored. Otherwise, if `iStepCount` is > 0, then the parameter
   * will be adapted to be handled as a discrete parameter with `iStepCount` steps even if it is not. For example,
   * if you have a parameter representing a gain value (which is not a discrete value), it can still be used with a
   * `DiscreteButtonView` so you can split the range into discrete values and have a button for each.
   */
  std::shared_ptr<ITGUIParameter<int32>> asDiscreteParameter(int32 iStepCount) override;

private:
  ParamID fParamID;
  VstParametersSPtr fVstParameters;
  std::shared_ptr<RawVstParamDef> fParamDef;
};

//-------------------------------------------------------------------------------
// GUIRawVstParam - wrapper to make writing the code much simpler and natural
//-------------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code (the param
 * behaves like T in many ways). */
class GUIRawVstParam
{
public:
  // Constructor
  GUIRawVstParam(std::shared_ptr<GUIRawVstParameter> iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{std::move(iPtr)}
  {}

  // exists
  inline bool exists() const { return (bool) fPtr; }

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  /**
   * @return the current value of the parameter as a T (using the Denormalizer)
   */
  inline ParamValue getValue() const { return fPtr->getValue(); }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setValue(ParamValue const &iValue) { return fPtr->setValue(iValue); }

  /**
   * Shortcut to copy the value from another param to this one.
   */
  tresult copyValueFrom(GUIRawVstParam const &iParam) { return setValue(iParam.getValue()); }

  /**
   * @return number of steps (for discrete param) or 0 for continuous
   */
  inline int32 getStepCount() const { return fPtr->getStepCount(); }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString) { fPtr->toString(oString); }

  /**
   * Returns a string representation of this parameter
   */
  String toString() { return fPtr->toString(); }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<GUIRawVstParameter::EditorType> edit() { return fPtr->edit(); }

  /**
   * Shortcut to create an editor and set the value to it
   *
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<GUIRawVstParameter::EditorType> edit(ParamValue const &iValue) { return fPtr->edit(iValue); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator ParamValue() const { return fPtr->getValue(); } // NOLINT

  // allow to write param = 3 instead of param.setValue(3)
  inline void operator=(ParamValue const &iValue) { fPtr->setValue(iValue); }

  // allow to write param1 == param2
  inline bool operator==(const GUIRawVstParam &rhs) const { return fPtr->getValue() == rhs.fPtr->getValue(); }

  // allow to write param1 != param2
  inline bool operator!=(const GUIRawVstParam &rhs) const { return fPtr->getValue() != rhs.fPtr->getValue(); }

  /**
   * @return an object maintaining the connection between the parameter and the listener
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const { return fPtr->connect(iChangeListener); }

  /**
   * @return an object maintaining the connection between the parameter and the callback
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const { return fPtr->connect(std::move(iChangeCallback)); }

private:
  std::shared_ptr<GUIRawVstParameter> fPtr;
};

//------------------------------------------------------------------------
// shortcut notations
//------------------------------------------------------------------------
using GUIRawVstParamEditor = std::unique_ptr<GUIRawVstParameter::EditorType>;

}
