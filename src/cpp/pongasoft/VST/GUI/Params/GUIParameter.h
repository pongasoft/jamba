#ifndef __PONGASOFT_VST_GUI_PARAMETER_H__
#define __PONGASOFT_VST_GUI_PARAMETER_H__

#include "GUIRawParameter.h"
#include <pongasoft/VST/ParamConverters.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * This class wraps a GUIRawParameter to deal with any type using a ParamConverter
 */
template<typename ParamConverter>
class GUIParameter
{
public:
  using ParamType = typename ParamConverter::ParamType;
  typedef GUIParameter<ParamConverter> class_type;

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
    inline explicit Editor(std::unique_ptr<GUIRawParameter::Editor> iRawEditor) :
      fRawEditor{std::move(iRawEditor)}
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
      return fRawEditor->setValue(ParamConverter::normalize(iValue));
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
    std::unique_ptr<GUIRawParameter::Editor> fRawEditor;
  };

public:
  // Constructor
  explicit GUIParameter(std::unique_ptr<GUIRawParameter> iRawParameter) :
    fRawParameter{std::move(iRawParameter)}
  {
    DCHECK_NOTNULL_F(fRawParameter.get());
    // DLOG_F(INFO, "VSTParameter::VSTParameter(%d)", fRawParameter->getParamID());
  }

  // Destructor
  ~GUIParameter()
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
    return ParamConverter::denormalize(fRawParameter->getValue());
  }

  /**
   * Sets the value of this parameter. Note that this is "transactional" and if you want to make
   * further changes that spans multiple calls (ex: onMouseDown / onMouseMoved / onMouseUp) you should use an editor
   */
  tresult setValue(ParamType iValue)
  {
    return fRawParameter->setValue(ParamConverter::normalize(iValue));
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
    return std::make_unique<Editor>(fRawParameter->edit());
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
  std::unique_ptr<GUIRawParameter> fRawParameter;
};

//------------------------------------------------------------------------
// GUIParamSPtr - shortcut notation
//------------------------------------------------------------------------
template<typename ParamConverter>
using GUIParamUPtr = std::unique_ptr<GUIParameter<ParamConverter>>;

template<typename ParamConverter>
using GUIParamEditorUPtr = std::unique_ptr<typename GUIParameter<ParamConverter>::Editor>;

using GUIBooleanParamUPtr = std::unique_ptr<GUIParameter<BooleanParamConverter>>;
using GUIPercentParamUPtr = std::unique_ptr<GUIParameter<PercentParamConverter>>;


}
}
}
}

#endif // __PONGASOFT_VST_GUI_PARAMETER_H__
