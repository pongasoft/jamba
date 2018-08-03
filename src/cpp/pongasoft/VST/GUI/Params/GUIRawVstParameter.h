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


}
}
}
}

#endif //__PONGASOFT_VST_GUI_RAW_PARAMETER_H__
