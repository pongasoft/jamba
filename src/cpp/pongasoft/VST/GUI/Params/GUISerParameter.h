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
#pragma once

#include <base/source/fobject.h>
#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamDef.h>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/Messaging.h>
#include <pongasoft/VST/MessageHandler.h>
#include "GUIParamCx.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Base class for a Serializable (Ser) GUI parameter. This type of parameter is used when it cannot be mapped to a
 * Vst parameter whose internal representation must be a value in the range [0.0, 1.0]. For example a string (like
 * a user input label to name a component) does not fit in the Vst parameter category. By implementating
 * the serializable api (readFromStream/writeToStream), any type can be part of the state.
 */
class IGUISerParameter : public IMessageHandler
{
public:
  // Constructor
  explicit IGUISerParameter(std::shared_ptr<ISerParamDef> iParamDef) :
    fParamDef{std::move(iParamDef)}
  {}

  // getParamDef
  std::shared_ptr<ISerParamDef> const &getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // readFromStream
  virtual tresult readFromStream(IBStreamer &iStreamer) = 0;

  // writeToStream
  virtual tresult writeToStream(IBStreamer &oStreamer) const = 0;

  // handleMessage
  tresult handleMessage(Message const &iMessage) override = 0;

protected:
  std::shared_ptr<ISerParamDef> fParamDef;
};

/**
 * This is the templated version providing serializer methods, very similar to the GUIVstParameter concept.
 *
 * @tparam T the underlying type of the param */
template<typename T>
class GUISerParameter : public IGUISerParameter, public FObject
{
public:
  using SerParamType = T;

  // Constructor
  explicit GUISerParameter(std::shared_ptr<SerParamDef<T>> iParamDef) :
    IGUISerParameter(iParamDef),
    FObject(),
    fSerParamDef{std::move(iParamDef)},
    fValue{fSerParamDef->fDefaultValue}
  {}

  // getSerParamDef (correct type for this subclass)
  inline std::shared_ptr<SerParamDef<T>> const &getSerParamDef() const
  {
    return fSerParamDef;
  }

  /**
   * Update the parameter with a value.
   *
   * @return true if the value was actually updated, false if it is the same
   */
  bool update(SerParamType const &iValue)
  {
    if(fValue != iValue)
    {
      fValue = iValue;
      changed();
      return true;
    }
    return false;
  }

  /**
   * Sets the value. The difference with update is that it does not check for equality (case when SerParamType is
   * not comparable)
   */
  void setValue(SerParamType const &iValue)
  {
    fValue = iValue;
    changed();
  }

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer) override
  {
    tresult res = fSerParamDef->readFromStream(iStreamer, fValue);
    if(res == kResultOk)
      changed();
    return res;
  }

  // writeToStream
  tresult writeToStream(IBStreamer &oStreamer) const override
  {
    return fSerParamDef->writeToStream(fValue, oStreamer);
  }

  // readFromMessage
  inline tresult readFromMessage(Message const &iMessage)
  {
    tresult res = fSerParamDef->readFromMessage(iMessage, fValue);
    if(res == kResultOk)
      changed();
    return res;
  }

  // writeToMessage
  inline tresult writeToMessage(Message &oMessage) const
  {
    return fSerParamDef->writeToMessage(fValue, oMessage);
  }

  // handleMessage
  tresult handleMessage(Message const &iMessage) override { return readFromMessage(iMessage); }

  // getValue
  inline SerParamType const &getValue() { return fValue; }

  /**
   * @return a connection that will listen to parameter changes (see GUIParamCx)
   */
  std::unique_ptr<GUIParamCx> connect(Parameters::IChangeListener *iChangeListener)
  {
    return std::make_unique<GUIParamCx>(getParamID(), this, iChangeListener);
  }

protected:
  std::shared_ptr<SerParamDef<T>> fSerParamDef;
  SerParamType fValue;
};


//------------------------------------------------------------------------
// GUISerParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of iterators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class GUISerParam
{
public:
  GUISerParam(GUISerParameter<T> *iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{iPtr}
  {}

  // exists
  inline bool exists() const { return fPtr != nullptr; }

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  /**
   * This method is typically called by a view to change the value of the parameter. Listeners will be notified
   * of the changes.
   */
  inline bool update(T const &iNewValue) { return fPtr->update(iNewValue); }

  /**
   * The difference with update is that it does not check for equality (case when SerParamType is not comparable)
   */
  inline void setValue(T const &iNewValue) { fPtr->setValue(iNewValue); }

  // getValue
  inline T const &getValue() const { return fPtr->getValue(); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T const &() const { return fPtr->getValue(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T const *operator->() const { return &fPtr->getValue(); }

  // readFromStream
  inline tresult readFromStream(IBStreamer &iStreamer) { return fPtr->readFromStream(iStreamer); };

  // writeToStream
  inline tresult writeToStream(IBStreamer &oStreamer) const { return fPtr->writeToStream(oStreamer); };

  // readFromMessage
  inline tresult readFromMessage(Message const &iMessage) { return fPtr->readFromMessage(iMessage); }

  // writeToMessage
  inline tresult writeToMessage(Message &oMessage) const { return fPtr->writeToMessage(oMessage);}

  // connect
  inline std::unique_ptr<GUIParamCx> connect(Parameters::IChangeListener *iChangeListener) { return fPtr->connect(iChangeListener); }

private:
  GUISerParameter<T> *fPtr;
};

}
}
}
}