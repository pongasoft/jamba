/*
 * Copyright (c) 2018 pongasoft
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
  inline IJmbParamDef const *getParamDef() const { return fParamDef.get(); }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // destructor
  ~IRTJmbInParameter() override = default;

  // hasUpdate
  virtual bool hasUpdate() const = 0;

  // readFromMessage
  virtual tresult readFromMessage(Message const &iMessage) = 0;

  // handleMessage
  tresult handleMessage(Message const &iMessage) override { return readFromMessage(iMessage); }

  // writeToStream
  virtual void writeToStream(std::ostream &oStream) const = 0;

protected:
  std::shared_ptr<IJmbParamDef> fParamDef;
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
    fUpdateQueue{std::make_unique<T>(iParamDef->fDefaultValue), true}
  {}

  // getParamDef
  inline JmbParamDef<T> const *getParamDefT() const
  {
    return static_cast<JmbParamDef<T> const *>(getParamDef());
  }

  // pop
  inline ParamType *pop() { return fUpdateQueue.pop(); }

  // last
  inline ParamType const *last() const { return fUpdateQueue.last(); }

  // popOrLast
  inline ParamType const *popOrLast() { return fUpdateQueue.popOrLast(); }

  // hasUpdate
  bool hasUpdate() const override { return !fUpdateQueue.isEmpty(); }

  // readFromMessage - called to extract the value from the message
  tresult readFromMessage(Message const &iMessage) override;

  // writeToStream
  void writeToStream(std::ostream &oStream) const override;

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
    return getParamDefT()->readFromMessage(iMessage, *oUpdate) == kResultOk;
  });

  return res ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
// RTJmbInParameter::writeToStream
//------------------------------------------------------------------------
template<typename T>
void RTJmbInParameter<T>::writeToStream(std::ostream &oStream) const
{
  getParamDefT()->writeToStream(*last(), oStream);
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

  // pop - nullptr if no update
  inline T *pop() { return fPtr->pop(); }

  // last - last value popped (does not pop!)
  inline T const *last() const { return fPtr->last(); }

  // popOrLast - call pop and no new value then call last
  inline T const *popOrLast() { return fPtr->popOrLast(); }

  // hasUpdate
  bool hasUpdate() const { return fPtr->hasUpdate(); }

  // getValue
  inline T const &getValue() const { return *fPtr->last(); }

  // value - synonym
  inline T const &value() const { return *fPtr->last(); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T const &() const { return *fPtr->last(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T const *operator->() const { return fPtr->last(); }

private:
  RTJmbInParameter<T> *fPtr;
};

}
}
}