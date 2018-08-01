#pragma once

#include <base/source/fobject.h>
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

class IGUISerParameter
{
public:

  // Constructor
  explicit IGUISerParameter(std::shared_ptr<ISerParamDef> iParamDef) :
    fParamDef{std::move(iParamDef)}
  {}

  // getParamDef
  std::shared_ptr<ISerParamDef> getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // readFromStream
  virtual tresult readFromStream(IBStreamer &iStreamer) = 0;

  // writeToStream
  virtual tresult writeToStream(IBStreamer &oStreamer) const = 0;

protected:
  std::shared_ptr<ISerParamDef> fParamDef;
};


template<typename ParamSerializer>
class GUISerParameter : public IGUISerParameter, public FObject
{
public:
  using SerParamType = typename ParamSerializer::ParamType;

  // Constructor
  explicit GUISerParameter(std::shared_ptr<SerParamDef<ParamSerializer>> iParamDef) :
    IGUISerParameter(iParamDef),
    FObject{},
    fValue{iParamDef->fDefaultValue}
  {}

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool setValue(SerParamType const &iValue)
  {
    if(fValue != iValue)
    {
      fValue = iValue;
      changed();
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
  inline SerParamType const &getValue() { return fValue; }

protected:
  SerParamType fValue;
};

//------------------------------------------------------------------------
// GUISerParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of iterators which helps in writing simpler and natural code (the param
 * behaves like ParamSerializer::ParamType in many ways).
 *
 * @tparam ParamSerializer the serializer (see ParamSerializers.h for an explanation of what is expected) */
template<typename ParamSerializer>
class GUISerParam
{
  using SerParamType = typename ParamSerializer::ParamType;

public:
  explicit GUISerParam(std::shared_ptr<GUISerParameter<ParamSerializer>> iPtr) :
    fPtr{std::move(iPtr)}
  {}

  inline tresult readFromStream(IBStreamer &iStreamer) { return fPtr->readFromStream(iStreamer); };

  inline tresult writeToStream(IBStreamer &oStreamer) const { return fPtr->writeToStream(oStreamer); };

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes.
   */
  inline void setValue(SerParamType const &iNewValue) { fPtr->setValue(iNewValue); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator SerParamType const &() const { return fPtr->getValue(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline SerParamType const *operator->() const { return &fPtr->getValue(); }

private:
  std::shared_ptr<GUISerParameter<ParamSerializer>> fPtr;
};

}
}
}
}