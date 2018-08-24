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

namespace pongasoft {
namespace VST {
namespace RT {

using namespace Utils;

/**
 * Base (non templated) class for RT Jamba (Outbound) parameters
 */
class IRTJmbOutParameter
{
public:
  // Constructor
  explicit IRTJmbOutParameter(std::shared_ptr<IJmbParamDef> iParamDef) : fParamDef{std::move(iParamDef)} {}

  // getParamDef
  std::shared_ptr<IJmbParamDef> const &getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // destructor
  virtual ~IRTJmbOutParameter() = default;

  // hasUpdate
  virtual bool hasUpdate() const = 0;

  // writeToMessage
  virtual tresult writeToMessage(Message &oMessage) = 0;

  // writeToStream
  virtual void writeToStream(std::ostream &oStream) const = 0;

protected:
  std::shared_ptr<IJmbParamDef> fParamDef;
};

/**
 * Templated class for RT Jamba parameter. The RT code calls enqueueUpdate when a new value needs to be propagated
 * to its peer (GUI). A GUI timer will then pop the value from the queue, serialize it, wrap it in a message and
 * send it to the GUI.
 *
 * @tparam T
 */
template<typename T>
class RTJmbOutParameter : public IRTJmbOutParameter
{
public:
  using ParamType = T;

  explicit RTJmbOutParameter(std::shared_ptr<JmbParamDef<T>> iParamDef) :
  IRTJmbOutParameter(iParamDef),
    fJmbParamDef{std::move(iParamDef)},
    fValue{fJmbParamDef->fDefaultValue}
    {}

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // getValue (non const / direct access)
  inline ParamType &getValue() { return fValue; }

  // setValue
  inline void setValue(ParamType const &iValue) { fValue = iValue; }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread.
   */
  inline void broadcast(ParamType const &iValue)
  {
    setValue(iValue);
    broadcast();
  }

  /**
   * Enqueues the current value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread.
   */
  inline void broadcast() { fUpdateQueue.push(fValue); }

  // hasUpdate
  bool hasUpdate() const override { return !fUpdateQueue.isEmpty(); }

  // writeToMessage - called to package and add the value to the message
  tresult writeToMessage(Message &oMessage) override;

  // writeToStream
  void writeToStream(std::ostream &oStream) const override;

protected:
  std::shared_ptr<JmbParamDef<T>> fJmbParamDef;
  ParamType fValue;

private:
  Concurrent::LockFree::SingleElementQueue<T> fUpdateQueue{};
};

//------------------------------------------------------------------------
// RTJmbOutParameter::writeToMessage
//------------------------------------------------------------------------
template<typename T>
tresult RTJmbOutParameter<T>::writeToMessage(Message &oMessage)
{
  auto update = fUpdateQueue.pop();
  if(update)
  {
    return fJmbParamDef->writeToMessage(*update, oMessage);
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTJmbInParameter::writeToStream
//------------------------------------------------------------------------
template<typename T>
void RTJmbOutParameter<T>::writeToStream(std::ostream &oStream) const
{
  fJmbParamDef->writeToStream(fValue, oStream);
}

//------------------------------------------------------------------------
// RTJmbOutParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class RTJmbOutParam
{
public:
  RTJmbOutParam(RTJmbOutParameter<T> *iPtr) : fPtr{iPtr} {} // NOLINT (not marked explicit on purpose)

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  // getValue
  inline T const &getValue() const { return fPtr->getValue(); }

  // getValue (non const / direct access)
  inline T &getValue() { return fPtr->getValue(); }

  // setValue
  inline void setValue(T const &iValue) { fPtr->setValue(iValue); }

  // broadcast
  inline void broadcast(T const &iValue) { fPtr->broadcast(iValue); }

  // broadcast
  inline void broadcast() { fPtr->broadcast(); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T const &() const { return fPtr->getValue(); } // NOLINT

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator T &() { return fPtr->getValue(); } // NOLINT

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T const *operator->() const { return &fPtr->getValue(); }

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline T *operator->() { return &fPtr->getValue(); }

private:
  RTJmbOutParameter<T> *fPtr;
};

}
}
}