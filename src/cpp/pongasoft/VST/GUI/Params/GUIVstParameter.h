#ifndef __PONGASOFT_VST_GUI_PARAMETER_H__
#define __PONGASOFT_VST_GUI_PARAMETER_H__

#include "GUIRawVstParameter.h"
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * This class wraps a GUIRawVstParameter to deal with any type T
 */
template<typename T>
class GUIVstParameter
{
public:
  using ParamType = T;

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
    inline explicit Editor(std::shared_ptr<GUIRawVstParameter::Editor> iRawEditor,
                           std::shared_ptr<VstParamDef<T>> iVstParamDef) :
      fRawEditor{std::move(iRawEditor)},
      fVstParamDef{std::move(iVstParamDef)}
    {
    }

    // disabling copy
    Editor(Editor const &) = delete;
    Editor& operator=(Editor const &) = delete;

    /**
     * Change the value of the parameter. Note that nothing happens if you have called commit or rollback already
     */
    inline tresult setValue(ParamType iValue)
    {
      return fRawEditor->setValue(fVstParamDef->normalize(iValue));
    }

    /*
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    inline tresult commit()
    {
      return fRawEditor->commit();
    }

    /*
     * Shortcut to set the value prior to commit
     * Call when you are done with the modifications.
     * This has no effect if rollback() has already been called
     */
    inline tresult commit(ParamType iValue)
    {
      setValue(iValue);
      return commit();
    }

    /**
     * Call this if you want to revert to the original value of the parameter (when the editor is created).
     * This has no effect if commit() has already been called
     */
    inline tresult rollback()
    {
      return fRawEditor->rollback();
    }

  private:
    std::shared_ptr<GUIRawVstParameter::Editor> fRawEditor;
    std::shared_ptr<VstParamDef<T>> fVstParamDef;
  };

public:
  // Constructor
  GUIVstParameter(std::shared_ptr<GUIRawVstParameter> iRawParameter,
                  std::shared_ptr<VstParamDef<T>> iVstParamDef) :
    fRawParameter{std::move(iRawParameter)},
    fVstParamDef{std::move(iVstParamDef)}
  {
    DCHECK_NOTNULL_F(fRawParameter.get());
    // DLOG_F(INFO, "VSTParameter::VSTParameter(%d)", fRawParameter->getParamID());
  }

  // Destructor
  ~GUIVstParameter()
  {
    // DLOG_F(INFO, "VSTParameter::~VSTParameter(%d)", fRawParameter->getParamID());
  }

  // getParamID
  ParamID getParamID() const
  {
    return fRawParameter->getParamID();
  }

  /**
   * @return the current value of the parameter as a T (using the Denormalizer)
   */
  ParamType getValue() const
  {
    return fVstParamDef->denormalize(fRawParameter->getValue());
  }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setValue(ParamType iValue)
  {
    return fRawParameter->setValue(fVstParamDef->normalize(iValue));
  }

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
  String toString()
  {
    return fRawParameter->toString();
  }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<Editor> edit()
  {
    return std::make_unique<Editor>(fRawParameter->edit(), fVstParamDef);
  }

  /**
   * Shortcut to create an editor and set the value to it
   *
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<Editor> edit(ParamType iValue)
  {
    auto editor = edit();
    editor->setValue(iValue);
    return editor;
  }

private:
  std::shared_ptr<GUIRawVstParameter> fRawParameter;
  std::shared_ptr<VstParamDef<T>> fVstParamDef;
};

//------------------------------------------------------------------------
// shortcut notations
//------------------------------------------------------------------------
template<typename T>
using GUIVstParam = std::shared_ptr<GUIVstParameter<T>>;

template<typename T>
using GUIVstParamEditor = std::unique_ptr<typename GUIVstParameter<T>::Editor>;

using GUIVstBooleanParam = std::shared_ptr<GUIVstParameter<bool>>;
using GUIVstPercentParam = std::shared_ptr<GUIVstParameter<Percent>>;

}
}
}
}

#endif // __PONGASOFT_VST_GUI_PARAMETER_H__
