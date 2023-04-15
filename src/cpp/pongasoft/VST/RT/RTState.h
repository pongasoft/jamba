/*
 * Copyright (c) 2018-2023 pongasoft
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
#include <pongasoft/Utils/stl.h>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/NormalizedState.h>
#include <pongasoft/VST/MessageProducer.h>

#include "RTParameter.h"
#include "RTJmbOutParameter.h"
#include "RTJmbInParameter.h"

#include <map>

namespace pongasoft {
namespace VST {
namespace Debug { class ParamDisplay; }
namespace RT {

using namespace Utils;

/**
 * Manages the state used by the processor: you add all the parameters that the state manages using the add method.
 * Combined with the RTProcessor class, everything will be handled for you (reading/writing the state, updating previous
 * value, etc...).
 */
class RTState : public IMessageHandler
{
public:
  explicit RTState(Parameters const &iParameters);

  /**
   * This method is called for each parameter managed by RTState. The order in which this method is called is
   * important and reflects the order that will be used when reading/writing state to the stream
   */
  RTRawVstParam add(RawVstParam iParamDef);

  /**
   * This method is used when multiple params of the same type are managed in an array (ex: leds, etc...). The order
   * in which this method is called is important and reflects the order that will be used when
   * reading/writing state to the stream
   */
  template<size_t N>
  RTRawVstParams<N> add(RawVstParams<N> const &iParamDefs);

  /**
   * This method is called for each parameter managed by RTState. The order in which this method is called is
   * important and reflects the order that will be used when reading/writing state to the stream
   */
  template<typename T>
  RTVstParam<T> add(VstParam<T> iParamDef);

  /**
   * This method is used when multiple params of the same type are managed in an array (ex: leds, etc...). The order in
   * which this method is called is important and reflects the order that will be used when reading/writing state to
   * the stream.
   */
  template<typename T, size_t N>
  RTVstParams<T, N> add(VstParams<T, N> const &iParamDefs);

  /**
   * This method should be called to add an rt outbound jmb parameter
   */
  template<typename T>
  RTJmbOutParam<T> addJmbOut(JmbParam<T> iParamDef);

  /**
   * This method used when multiple params of the same type are managed in an array for outbound jmb parameter
   */
  template<typename T, size_t N>
  RTJmbOutParams<T, N> addJmbOut(JmbParams<T, N> const &iParamDefs);

  /**
   * This method should be called to add an rt inbound jmb parameter
   */
  template<typename T>
  RTJmbInParam<T> addJmbIn(JmbParam<T> iParamDef);

  /**
   * This method used when multiple params of the same type are managed in an array for inbound jmb parameter
   */
  template<typename T, size_t N>
  RTJmbInParams<T, N> addJmbIn(JmbParams<T, N> const &iParamDefs);

  /**
   * Call this method after adding all the parameters. If using the RT processor, it will happen automatically. */
  virtual tresult init();

  /**
   * This method should be call at the beginning of the process(ProcessData &data) method before doing anything else.
   * The goal of this method is to update the current state with a state set by the UI (typical use case is to
   * initialize the plugin when being loaded)
   *
   * @return true if the state of the plugin changed
   * */
  virtual bool beforeProcessing();

  /**
   * This method should be called in every frame when there are parameter changes to update this state accordingly
   */
  virtual bool applyParameterChanges(IParameterChanges &inputParameterChanges);

  /**
   * This uses the same algorithm as when the param value is updated (implemented in applyParameterChanges) for
   * consistency. If the param changes more than once in a frame, only the last value is taken into account.
   *
   * @return the offset at which the param changed (-1 if it did not change)
   */
  virtual int32 getParamUpdateSampleOffset(ProcessData &iData, ParamID iParamID) const;

  /**
   * This method should be called at the end of process(ProcessData &data) method. It will update the previous state
   * to the current one and save the latest changes (if necessary) so that it is accessible via writeLatestState.
   */
  virtual void afterProcessing();

  /**
   * This method should be called from Processor::setState to update this state to the state stored in the stream.
   * Note that this method is called from the UI thread so the update is queued until the next frame.
   */
  virtual tresult readNewState(IBStreamer &iStreamer);

  /**
   * This method should be called from Processor::getState to store the latest state to the stream. Note that this
   * method is called from the UI thread and gets the "latest" state as of the end of the last frame.
   */
  virtual tresult writeLatestState(IBStreamer &oStreamer);

  /**
   * @return true if messaging is enabled (which at this moment is whether any JmbParam was added) */
  bool isMessagingEnabled() const { return !fOutboundMessagingParameters.empty(); }

  /**
   * Called (from a GUI timer) to send the messages to the GUI (JmbParam for the moment) */
  virtual tresult sendPendingMessages(IMessageProducer *iMessageProducer);

  /**
   * Called by the UI thread (from RTProcessor) to handle messages.
   */
  tresult handleMessage(Message const &iMessage) override;

  // getAllRegistrationOrder
  std::vector<ParamID> const &getAllRegistrationOrder() const { return fAllRegistrationOrder; }

  // gives access for debug
  friend class Debug::ParamDisplay;

protected:
  // the parameters
  Parameters const &fPluginParameters;

  // contains all the registered vst parameters (unique ID, will be checked on add)
  std::map<ParamID, std::unique_ptr<RTRawVstParameter>> fVstParameters{};

  // contains all the registered outbound message parameters (unique ID, will be checked on add)
  std::map<ParamID, std::unique_ptr<IRTJmbOutParameter>> fOutboundMessagingParameters{};

  // contains all the registered inbound message parameters (unique ID, will be checked on add)
  std::map<ParamID, std::unique_ptr<IRTJmbInParameter>> fInboundMessagingParameters{};

  // order in which the parameters were registered
  std::vector<ParamID> fAllRegistrationOrder{};

  // handles messages (receive messages)
  MessageHandler fMessageHandler{};

protected:
  // add raw parameter to the structures
  tresult addRawParameter(std::unique_ptr<RTRawVstParameter> iParameter);

  // add outbound messaging parameter
  tresult addOutboundMessagingParameter(std::unique_ptr<IRTJmbOutParameter> iParameter);

  // add inbound messaging parameter
  tresult addInboundMessagingParameter(std::unique_ptr<IRTJmbInParameter> iParameter);

  /**
   * Called from the RT thread from beforeProcessing to set the new state. Can be overridden
   * @return true if the state has changed, false otherwise
   */
  virtual bool onNewState(NormalizedState const *iLatestState);

  /**
   * Called from the RT thread from afterProcessing to reset previous values (copy current value to previous).
   * Can be overridden.
   *
   * @return true if the state has changed, false otherwise */
  virtual bool resetPreviousValues();

  /**
   * Called from the RT thread from afterProcessing to compute the latest state. Can be overridden
   */
  virtual void computeLatestState(NormalizedState *oLatestState) const;

  /**
   * Gives a chance to subclasses to tweak and/or display the state after being read */
  virtual void afterReadNewState(NormalizedState const *iState) {};

  /**
   * Gives a chance to subclasses to tweak and/or display the state before being written */
  virtual void beforeWriteNewState(NormalizedState const *iState) {};

private:
  // computeLatestState
  void computeLatestState();

private:
  // this queue is used to propagate a Processor::setState call (made from the UI thread) to this state
  // the check happens in beforeProcessing
  Concurrent::LockFree::SingleElementQueue<NormalizedState> fStateUpdate;

  // this atomic value always hold the most current (and consistent) version of this state so that the UI thread
  // can access it in Processor::getState. It is updated in afterProcessing.
  Concurrent::LockFree::AtomicValue<NormalizedState> fLatestState;
};

//------------------------------------------------------------------------
// RTState::add
//------------------------------------------------------------------------
template<typename T>
RTVstParam<T> RTState::add(VstParam<T> iParamDef)
{
  // YP Impl note: this method exports the raw pointer on purpose. The map fVstParameters is storing unique_ptr so that
  // when the map gets deleted, all the parameters get deleted as well. The raw pointer is wrapped inside the RTVstParam
  // wrapper and the normal usage is that it will not outlive the map. Using shared_ptr would be the safest approach
  // but in the RT code, we want to make sure that we do not pay the penalty of managing a ref counter (which may
  // use a lock...)
  auto rawPtr = new RTVstParameter<T>(std::move(iParamDef));
  std::unique_ptr<RTRawVstParameter> rtParam{rawPtr};
  addRawParameter(std::move(rtParam));
  return rawPtr;
}

//------------------------------------------------------------------------
// RTState::add
//------------------------------------------------------------------------
template<size_t N>
RTRawVstParams<N> RTState::add(RawVstParams<N> const &iParamDefs)
{
  return stl::transform<RTRawVstParam, RawVstParam>(iParamDefs, [this](auto &p) { return add(p); });
}

//------------------------------------------------------------------------
// RTState::add
//------------------------------------------------------------------------
template<typename T, size_t N>
RTVstParams<T, N> RTState::add(VstParams<T, N> const &iParamDefs)
{
  return stl::transform<VstParam<T>, RTVstParam<T>>(iParamDefs, [this](auto &p) { return add(p); });
}

//------------------------------------------------------------------------
// RTState::addJmbOut
//------------------------------------------------------------------------
template<typename T>
RTJmbOutParam<T> RTState::addJmbOut(JmbParam<T> iParamDef)
{
  // YP Impl note: see add for similar impl note
  auto rawPtr = new RTJmbOutParameter<T>(std::move(iParamDef));
  std::unique_ptr<IRTJmbOutParameter> rtParam{rawPtr};
  addOutboundMessagingParameter(std::move(rtParam));
  return rawPtr;
}

//------------------------------------------------------------------------
// RTState::addJmbOut
//------------------------------------------------------------------------
template<typename T, size_t N>
RTJmbOutParams<T, N> RTState::addJmbOut(JmbParams<T, N> const &iParamDefs)
{
  return stl::transform<RTJmbOutParam<T>, JmbParam<T>>(iParamDefs, [this](auto &p) { return addJmbOut(p); });
}

//------------------------------------------------------------------------
// RTState::addJmbIn
//------------------------------------------------------------------------
template<typename T>
RTJmbInParam<T> RTState::addJmbIn(JmbParam<T> iParamDef)
{
  // YP Impl note: see add for similar impl note
  auto rawPtr = new RTJmbInParameter<T>(iParamDef);
  std::unique_ptr<IRTJmbInParameter> rtParam{rawPtr};
  addInboundMessagingParameter(std::move(rtParam));
  fMessageHandler.registerHandler(iParamDef->fParamID, rawPtr);
  return rawPtr;
}

//------------------------------------------------------------------------
// RTState::addJmbIn
//------------------------------------------------------------------------
template<typename T, size_t N>
RTJmbInParams<T, N> RTState::addJmbIn(JmbParams<T, N> const &iParamDefs)
{
  return stl::transform<RTJmbInParam<T>, JmbParam<T>>(iParamDefs, [this](auto &p) { return addJmbIn(p); });
}

}
}
}
