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

#include <pongasoft/Utils/Concurrent/Concurrent.h>
#include <pongasoft/VST/ParamDef.h>
#include <pongasoft/VST/MessageHandler.h>

namespace pongasoft {
namespace VST {
namespace RT {

using namespace Utils;

/**
 * Base (non templated) class for RT Jamba (Inbound) parameters
 */
class IRTJmbInParameter : public IMessageHandler
{
public:
  // Constructor
  explicit IRTJmbInParameter(std::shared_ptr<IJmbParamDef> iParamDef) : fParamDef{std::move(iParamDef)} {}

  // getParamDef
  std::shared_ptr<IJmbParamDef> const &getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // destructor
  ~IRTJmbInParameter() override = default;

  // hasUpdate
  virtual bool hasUpdate() const = 0;

  // applyUpdate
  virtual bool applyUpdate() = 0;

  /**
   * This is typically called by the RT processing at the beginning of processing to check if an update
   * happened since the last frame. */
  inline bool hasChanged() {return fChanged; }

  // resetChanged
  inline void resetChanged() { fChanged = false; }

  // readFromMessage
  virtual tresult readFromMessage(Message const &iMessage) = 0;

  // handleMessage
  tresult handleMessage(Message const &iMessage) override { return readFromMessage(iMessage); }

  // writeToStream
  virtual void writeToStream(std::ostream &oStream) const = 0;

protected:
  std::shared_ptr<IJmbParamDef> fParamDef;
  bool fChanged = false;
};

/**
 * Templated class for RT Jamba Inbound parameter. The GUI thread calls readFromMessage to extract the value
 * and store it in the queue. The RT thread will later extract the value from the queue and store it locally.
 *
 * @tparam T
 */
template<typename T>
class RTJmbInParameter : public IRTJmbInParameter
{
public:
  using ParamType = T;

  explicit RTJmbInParameter(std::shared_ptr<JmbParamDef<T>> iParamDef) :
    IRTJmbInParameter(iParamDef),
    fJmbParamDef{std::move(iParamDef)},
    fValue{fJmbParamDef->fDefaultValue}
  {}

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // hasUpdate
  bool hasUpdate() const override { return !fUpdateQueue.isEmpty(); }

  // applyUpdate
  bool applyUpdate() override;

  // readFromMessage - called to extract the value from the message
  tresult readFromMessage(Message const &iMessage) override;

  // writeToStream
  void writeToStream(std::ostream &oStream) const override;

protected:
  std::shared_ptr<JmbParamDef<T>> fJmbParamDef;
  ParamType fValue;

private:
  Concurrent::LockFree::SingleElementQueue<T> fUpdateQueue{};
};

//------------------------------------------------------------------------
// RTJmbInParameter::readFromMessage
//------------------------------------------------------------------------
template<typename T>
tresult RTJmbInParameter<T>::readFromMessage(Message const &iMessage)
{
  bool res = fUpdateQueue.updateAndPushIf([this, &iMessage](auto oUpdate) -> bool {
    return fJmbParamDef->readFromMessage(iMessage, *oUpdate) == kResultOk;
  });

  return res ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
// RTJmbInParameter::applyUpdate
//------------------------------------------------------------------------
template<typename T>
bool RTJmbInParameter<T>::applyUpdate()
{
  fChanged = fUpdateQueue.pop(fValue);
  return fChanged;
}

//------------------------------------------------------------------------
// RTJmbInParameter::writeToStream
//------------------------------------------------------------------------
template<typename T>
void RTJmbInParameter<T>::writeToStream(std::ostream &oStream) const
{
  fJmbParamDef->writeToStream(fValue, oStream);
}

//------------------------------------------------------------------------
// RTJmbInParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class RTJmbInParam
{
public:
  RTJmbInParam(RTJmbInParameter<T> *iPtr) : fPtr{iPtr} {} // NOLINT (not marked explicit on purpose)

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  /**
   * This is typically called by the RT processing at the beginning of processing to check if an update
   * happened since the last frame. */
  inline bool hasChanged() const { return fPtr->hasChanged(); }

  // resetChanged
  inline void resetChanged() { fPtr->resetChanged(); }

  // getValue
  inline T const &getValue() const { return fPtr->getValue(); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T const &() const { return fPtr->getValue(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T const *operator->() const { return &fPtr->getValue(); }

private:
  RTJmbInParameter<T> *fPtr;
};

}
}
}