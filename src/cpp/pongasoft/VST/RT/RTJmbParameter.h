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
 * Base (non templated) class for RT Jamba parameters
 */
class IRTJmbParameter
{
public:
  // Constructor
  explicit IRTJmbParameter(std::shared_ptr<IJmbParamDef> iParamDef) : fParamDef{std::move(iParamDef)} {}

  // getParamDef
  std::shared_ptr<IJmbParamDef> const &getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // destructor
  virtual ~IRTJmbParameter() = default;

  // hasOutboundUpdate
  virtual bool hasOutboundUpdate() const = 0;

  // writeToMessage
  virtual tresult writeToMessage(Message &oMessage) = 0;

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
class RTJmbParameter : public IRTJmbParameter
{
public:
  using ParamType = T;

  explicit RTJmbParameter(std::shared_ptr<JmbParamDef<T>> iParamDef) :
    IRTJmbParameter(iParamDef),
    fJmbParamDef{std::move(iParamDef)}
  {}

  /**
   * This is a call that enqueues the value to be delivered to the GUI. Note that this call returns right away. The
   * packaging and delivery will happen in a GUI thread. This method is called by RT thread.
   * @param iValue
   */
  inline void enqueueUpdate(T const &iValue) { fOutboundQueue.push(iValue); }

  // hasOutboundUpdate
  bool hasOutboundUpdate() const override { return !fOutboundQueue.isEmpty(); }

  // writeToMessage - called to package and add the value to the message
  tresult writeToMessage(Message &oMessage) override;

protected:
  std::shared_ptr<JmbParamDef<T>> fJmbParamDef;

private:
  Concurrent::WithSpinLock::SingleElementQueue<T> fOutboundQueue{};
};

//------------------------------------------------------------------------
// RTJmbParameter::writeToMessage
//------------------------------------------------------------------------
template<typename T>
tresult RTJmbParameter<T>::writeToMessage(Message &oMessage)
{
  ParamType update;
  if(fOutboundQueue.pop(update))
  {
    return fJmbParamDef->writeToMessage(update, oMessage);
  }

  return kResultFalse;
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
  RTJmbOutParam(RTJmbParameter<T> *iPtr) : fPtr{iPtr} {} // NOLINT (not marked explicit on purpose)

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  // enqueUpdate
  inline void enqueueUpdate(T const &iValue) { fPtr->enqueueUpdate(iValue); }

private:
  RTJmbParameter<T> *fPtr;
};

}
}
}