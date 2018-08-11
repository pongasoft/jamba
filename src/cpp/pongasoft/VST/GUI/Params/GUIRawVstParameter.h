/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
#ifndef __PONGASOFT_VST_GUI_RAW_PARAMETER_H__
#define __PONGASOFT_VST_GUI_RAW_PARAMETER_H__

#include <pongasoft/VST/Parameters.h>
#include "VstParameters.h"
#include "GUIParamCx.h"

#include <string>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Encapsulates a vst parameter and how to access it (read/write) as well as how to "connect" to it in order to be
 * notified of changes. This "raw" version deals with ParamValue which is the underlying type used by the vst sdk
 * which is always a number in the range [0.0, 1.0]. The class VSTParameter deals with other types and automatic
 * normalization/denormalization.
 */
class GUIRawVstParameter
{
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
  class Editor
  {
  public:
    Editor(ParamID iParamID, VstParametersSPtr iVstParameters);

    // disabling copy
    Editor(Editor const &) = delete;
    Editor& operator=(Editor const &) = delete;

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already
     */
    tresult setValue(ParamValue iValue);

    /*
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    tresult commit();

    /*
     * Shortcut to set the value prior to commit
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    inline tresult commit(ParamValue iValue)
    {
      setValue(iValue);
      return commit();
    }

    /**
     * Call this if you want to revert to the original value of the parameter (when the editor is created).
     * This has no effect if commit() has already been called
     */
    tresult rollback();

    /**
     * Destructor which calls rollback by default
     */
    inline ~Editor()
    {
      // DLOG_F(INFO, "~RawParameter::Editor(%d)", fParamID);
      rollback();
    }

  private:
    ParamID fParamID;
    VstParametersSPtr fVstParameters;

    ParamValue fInitialParamValue;
    bool fIsEditing;
  };

public:
  // Constructor
  GUIRawVstParameter(ParamID iParamID, VstParametersSPtr iVstParameters);

  // Destructor
  ~GUIRawVstParameter() = default;
//  {
//    DLOG_F(INFO, "RawParameter::~RawParameter(%d)", fParamID);
//  }

  // getParamID
  inline ParamID getParamID() const
  {
    return fParamID;
  }

  /**
   * @return the current raw value of the parameter
   */
  inline ParamValue getValue() const
  {
    return fVstParameters->getParamNormalized(fParamID);
  }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString)
  {
    auto parameter = fVstParameters->getParameterObject(fParamID);
    if(parameter)
      parameter->toString(getValue(), oString);
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

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  inline tresult setValue(ParamValue iValue)
  {
    Editor editor(fParamID, fVstParameters);
    editor.setValue(iValue);
    return editor.commit();
  }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<Editor> edit()
  {
    return std::make_unique<Editor>(fParamID, fVstParameters);
  }

  /**
   * Shortcut to create an editor and set the value to it
   *
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<Editor> edit(ParamValue iValue)
  {
    auto editor = edit();
    editor->setValue(iValue);
    return editor;
  }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<GUIParamCx> connect(Parameters::IChangeListener *iChangeListener)
  {
    return std::make_unique<GUIParamCx>(fParamID, fVstParameters->getParameterObject(fParamID), iChangeListener);
  }

private:
  ParamID fParamID;
  VstParametersSPtr fVstParameters{};
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
  GUIRawVstParam() : fPtr{nullptr} {}

  // move constructor
  explicit GUIRawVstParam(std::unique_ptr<GUIRawVstParameter> &&iPtr) : fPtr{std::move(iPtr)} {}

  // delete copy constructor
  GUIRawVstParam(GUIRawVstParam &iPtr) = delete;

  // move copy constructor
  GUIRawVstParam(GUIRawVstParam &&iPtr) noexcept : fPtr{std::move(iPtr.fPtr)} {}

  // move assignment constructor
  GUIRawVstParam &operator=(GUIRawVstParam &&iPtr) noexcept { fPtr = std::move(iPtr.fPtr); return *this; }

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
  std::unique_ptr<GUIRawVstParameter::Editor> edit() { return fPtr->edit(); }

  /**
   * Shortcut to create an editor and set the value to it
   *
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<GUIRawVstParameter::Editor> edit(ParamValue const &iValue) { return fPtr->edit(iValue); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator ParamValue() const { return fPtr->getValue(); } // NOLINT

  // allow to write param = 3 instead of param.setValue(3)
  inline void operator=(ParamValue const &iValue) { fPtr->setValue(iValue); }

  // allow to write param1 == param2
  inline bool operator==(const GUIRawVstParam &rhs) const { return fPtr->getValue() == rhs.fPtr->getValue(); }

  // allow to write param1 != param2
  inline bool operator!=(const GUIRawVstParam &rhs) const { return fPtr->getValue() != rhs.fPtr->getValue(); }

private:
  std::unique_ptr<GUIRawVstParameter> fPtr;
};

//------------------------------------------------------------------------
// shortcut notations
//------------------------------------------------------------------------
using GUIRawVstParamEditor = std::unique_ptr<GUIRawVstParameter::Editor>;

}
}
}
}

#endif //__PONGASOFT_VST_GUI_RAW_PARAMETER_H__
