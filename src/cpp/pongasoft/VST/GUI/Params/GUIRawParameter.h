#ifndef __PONGASOFT_VST_GUI_RAW_PARAMETER_H__
#define __PONGASOFT_VST_GUI_RAW_PARAMETER_H__

#include "HostParameters.h"

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
class GUIRawParameter
{
public:
  /**
   * Interface to implement to receive parameter changes
   */
  class IChangeListener
  {
  public:
    virtual void onParameterChange(ParamID iParamID, ParamValue iNormalizedValue) = 0;
  };

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
    Editor(ParamID iParamID, HostParameters const &iHostParameters);

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
    HostParameters const &fHostParameters;

    ParamValue fInitialParamValue;
    bool fIsEditing;
  };

public:
  /**
   * Wrapper class which maintains the connection between a parameter and its listener. The connection will be
   * terminated if close() is called or automatically when the destructor is called.
   */
  class Connection : public Steinberg::FObject
  {
  public:
    Connection(ParamID iParamID, HostParameters const &iHostParameters, IChangeListener *iChangeListener);

    /**
     * Call to stop listening for changes. Also called automatically from the destructor.
     */
    void close();

    /**
     * Automatically closes the connection and stops listening */
    inline ~Connection() override
    {
      // DLOG_F(INFO, "~RawParameter::Connection(%d)", fParamID);
      close();
    }

    /**
     * This is being called when the parameter receives a message... do not call explicitely
     */
    void PLUGIN_API update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage) SMTG_OVERRIDE;

    // disabling copy
    Connection(Connection const &) = delete;
    Connection& operator=(Connection const &) = delete;

  private:
    ParamID fParamID;
    Vst::Parameter *fParameter;
    HostParameters const &fHostParameters;
    IChangeListener *const fChangeListener;
    bool fIsConnected;
  };

public:
  // Constructor
  GUIRawParameter(ParamID iParamID, HostParameters const &iHostParameters);

  // Destructor
  ~GUIRawParameter()
  {
    // DLOG_F(INFO, "RawParameter::~RawParameter(%d)", fParamID);
  }

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
    return fHostParameters.getParamNormalized(fParamID);
  }

  /**
   * Populates the oString with a string representation of this parameter
   */
  void toString(String128 oString)
  {
    auto parameter = fHostParameters.getParameterObject(fParamID);
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
    Editor editor(fParamID, fHostParameters);
    editor.setValue(iValue);
    return editor.commit();
  }

  /**
   * @return an editor to modify the parameter (see Editor)
   */
  std::unique_ptr<Editor> edit()
  {
    return std::make_unique<Editor>(fParamID, fHostParameters);
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
   * @return a connection that will listen to parameter changes (see Connection)
   */
  std::unique_ptr<Connection> connect(IChangeListener *iChangeListener)
  {
    return std::make_unique<Connection>(fParamID, fHostParameters, iChangeListener);
  }

private:
  ParamID fParamID;
  HostParameters const &fHostParameters;
};


}
}
}
}

#endif //__PONGASOFT_VST_GUI_RAW_PARAMETER_H__
