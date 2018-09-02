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
  inline IJmbParamDef const *getParamDef() const { return fParamDef.get(); }

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
    fUpdateQueue{std::make_unique<T>(iParamDef->fDefaultValue)}
  {}

  // getParamDef
  inline JmbParamDef<T> const *getParamDefT() const
  {
    return static_cast<JmbParamDef<T> const *>(getParamDef());
  }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread.
   */
  inline void broadcastValue(ParamType const &iValue)
  {
    fUpdateQueue.push(iValue);
  }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread. Use this flavor to avoid copy.
   */
  template<class ElementModifier>
  void broadcast(ElementModifier const &iElementModifier)
  {
    fUpdateQueue.updateAndPush(iElementModifier);
  }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread. Use this flavor to avoid copy. This flavor uses a callback that returns
   * true when the broadcast should happen and false otherwise.
   */
  template<class ElementModifier>
  bool broadcastIf(ElementModifier const &iElementModifier)
  {
    return fUpdateQueue.updateAndPushIf(iElementModifier);
  }

  // hasUpdate
  bool hasUpdate() const override { return !fUpdateQueue.isEmpty(); }

  // writeToMessage - called to package and add the value to the message
  tresult writeToMessage(Message &oMessage) override;

  // writeToStream
  void writeToStream(std::ostream &oStream) const override;

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
    return getParamDefT()->writeToMessage(*update, oMessage);
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTJmbInParameter::writeToStream
//------------------------------------------------------------------------
template<typename T>
void RTJmbOutParameter<T>::writeToStream(std::ostream &oStream) const
{
  getParamDefT()->writeToStream(*fUpdateQueue.last(), oStream);
}

//------------------------------------------------------------------------
// RTJmbOutParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param.
 *
 * @tparam T the underlying type of the param */
template<typename T>
class RTJmbOutParam
{
public:
  RTJmbOutParam(RTJmbOutParameter<T> *iPtr) : fPtr{iPtr} {} // NOLINT (not marked explicit on purpose)

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread. */
  inline void broadcast(T const &iValue) { fPtr->broadcastValue(iValue); }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread. Use this flavor to avoid copy. */
  template<class ElementModifier>
  void broadcast(ElementModifier const &iElementModifier) { fPtr->broadcast(iElementModifier); }

  /**
   * Enqueues the value to be delivered to the GUI (or whoever is listening to messages).
   * Note that this call returns right away. The packaging and delivery will happen in a GUI thread.
   * This method is called by RT thread. Use this flavor to avoid copy. This flavor uses a callback that returns
   * true when the broadcast should happen and false otherwise. */
  template<class ElementModifier>
  bool broadcastIf(ElementModifier const &iElementModifier) { return fPtr->broadcastIf(iElementModifier); }

private:
  RTJmbOutParameter<T> *fPtr;
};

}
}
}