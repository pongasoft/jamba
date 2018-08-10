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
 * Base (non templated) class for rt ser parameter
 */
class IRTSerParameter
{
public:
  // Constructor
  explicit IRTSerParameter(std::shared_ptr<ISerParamDef> iParamDef) : fParamDef{std::move(iParamDef)} {}

  // getParamDef
  std::shared_ptr<ISerParamDef> const &getParamDef() const { return fParamDef; }

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // destructor
  virtual ~IRTSerParameter() = default;

  // hasOutboundUpdate
  virtual bool hasOutboundUpdate() const = 0;

  // writeToMessage
  virtual tresult writeToMessage(Message &oMessage) = 0;

protected:
  std::shared_ptr<ISerParamDef> fParamDef;
};

template<typename T>
class RTSerParameter : public IRTSerParameter
{
public:
  using SerParamType = T;

  explicit RTSerParameter(std::shared_ptr<SerParamDef<T>> iParamDef) :
    IRTSerParameter(iParamDef),
    fSerParamDef{std::move(iParamDef)}
  {}


  inline void enqueueUpdate(T const &iValue) { fOutboundQueue.push(iValue); }
  bool hasOutboundUpdate() const override { return !fOutboundQueue.isEmpty(); }
  tresult writeToMessage(Message &oMessage) override;

protected:
  std::shared_ptr<SerParamDef<T>> fSerParamDef;

private:
  Concurrent::WithSpinLock::SingleElementQueue<T> fOutboundQueue{};
};

template<typename T>
tresult RTSerParameter<T>::writeToMessage(Message &oMessage)
{
  SerParamType update;
  if(fOutboundQueue.pop(update))
  {
    return fSerParamDef->writeToMessage(update, oMessage);
  }

  return kResultFalse;
}

template<typename T>
class RTSerOutParam
{
public:
  RTSerOutParam(RTSerParameter<T> *iPtr) : fPtr{iPtr} {} // NOLINT (not marked explicit on purpose)

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  // enqueUpdate
  inline void enqueueUpdate(T const &iValue) { fPtr->enqueueUpdate(iValue); }

private:
  RTSerParameter<T> *fPtr;
};

}
}
}