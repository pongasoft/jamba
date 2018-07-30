#ifndef __PONGASOFT_VST_GUI_PARAMETER_H__
#define __PONGASOFT_VST_GUI_PARAMETER_H__

#include "GUIRawParameter.h"
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>

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

class GUISerializableParameter
{
public:

  // Constructor
  explicit GUISerializableParameter(std::shared_ptr<SerializableParamDef> iParamDef) :
    fParamID{iParamDef->fParamID}
  {}

  // getParamID
  ParamID getParamID() const { return fParamID; }

  // readFromStream
  virtual tresult readFromStream(IBStreamer &iStreamer) = 0;

  // writeToStream
  virtual tresult writeToStream(IBStreamer &oStreamer) const = 0;

protected:
  ParamID fParamID;
};

template<typename ParamSerializer>
class GUIAnyParameter : public GUISerializableParameter
{
public:
  using SerializableParamType = typename ParamSerializer::ParamType;

  // Constructor
  explicit GUIAnyParameter(std::shared_ptr<AnyParamDef<ParamSerializer>> iParamDef) :
    GUISerializableParameter(iParamDef),
    fValue{iParamDef->fDefaultValue}
  {}

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool updateValue(SerializableParamType const &iValue)
  {
    if(fValue != iValue)
    {
      fValue = iValue;
      return true;
    }
    return false;
  }

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer) override
  {
    fValue = ParamSerializer::readFromStream(iStreamer, fValue);
    return kResultOk;
  }

  // writeToStream
  tresult writeToStream(IBStreamer &oStreamer) const override
  {
    return ParamSerializer::writeToStream(fValue, oStreamer);
  }

  // getValue
  inline SerializableParamType const &getValue() { return fValue; }

protected:
  SerializableParamType fValue;
};

//------------------------------------------------------------------------
// GUIParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of iterators which helps in writing simpler and natural code (the param
 * behaves like ParamSerializer::ParamType in many ways).
 *
 * @tparam ParamSerializer the serializer (see ParamSerializers.h for an explanation of what is expected) */
template<typename ParamSerializer>
class GUIParam
{
  using SerializableParamType = typename ParamSerializer::ParamType;

public:
  explicit GUIParam(std::shared_ptr<GUIAnyParameter<ParamSerializer>> iPtr) :
    fPtr{std::move(iPtr)}
  {}

  inline tresult readFromStream(IBStreamer &iStreamer) { return fPtr->readFromStream(iStreamer); };

  inline tresult writeToStream(IBStreamer &oStreamer) const { return fPtr->writeToStream(oStreamer); };

  inline bool updateValue(SerializableParamType const &iValue) { return fPtr->update(iValue); }

  /**
   * This method is typically called during the processing method when the plugin needs to update the value. In general
   * the change needs to be propagated to the VST sdk (using addToOutput). Use this version of the call if you want to
   * control when the update actually happens.
   */
  inline void update(SerializableParamType const &iNewValue) { fPtr->update(iNewValue); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator SerializableParamType const &() const { return fPtr->getValue(); }

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline SerializableParamType const *operator->() const { return &fPtr->getValue(); }

private:
  std::shared_ptr<GUIAnyParameter<ParamSerializer>> fPtr;
};

}
}
}
}

#endif // __PONGASOFT_VST_GUI_PARAMETER_H__
