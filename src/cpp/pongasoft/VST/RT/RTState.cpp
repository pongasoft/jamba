/*
 * Copyright (c) 2018-2020 pongasoft
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
#include "RTState.h"

namespace pongasoft::VST::RT {

//------------------------------------------------------------------------
// RTState::RTState
//------------------------------------------------------------------------
RTState::RTState(Parameters const &iParameters) :
  fPluginParameters{iParameters},
  fStateUpdate{iParameters.newRTState(), true},
  fLatestState{iParameters.newRTState()}
{
}

//------------------------------------------------------------------------
// RTState::add
//------------------------------------------------------------------------
RTRawVstParam RTState::add(RawVstParam iParamDef)
{
  // YP Impl note: this method exports the raw pointer on purpose. The map fVstParameters is storing unique_ptr so that
  // when the map gets deleted, all the parameters get deleted as well. The raw pointer is wrapped inside the RTRawVstParam
  // wrapper and the normal usage is that it will not outlive the map. Using shared_ptr would be the safest approach
  // but in the RT code, we want to make sure that we do not pay the penalty of managing a ref counter (which may
  // use a lock...)
  auto rawPtr = new RTRawVstParameter(std::move(iParamDef));
  std::unique_ptr<RTRawVstParameter> rtParam{rawPtr};
  addRawParameter(std::move(rtParam));
  return rawPtr;
}

//------------------------------------------------------------------------
// RTState::addRawParameter
//------------------------------------------------------------------------
tresult RTState::addRawParameter(std::unique_ptr<RTRawVstParameter> iParameter)
{
  if(!iParameter)
    return kInvalidArgument;

  ParamID paramID = iParameter->getParamID();
  if(fVstParameters.find(paramID) != fVstParameters.cend() ||
     fOutboundMessagingParameters.find(paramID) != fOutboundMessagingParameters.cend() ||
     fInboundMessagingParameters.find(paramID) != fInboundMessagingParameters.cend())
  {
    DLOG_F(ERROR, "duplicate paramID [%d]", paramID);
    return kInvalidArgument;
  }

  if(iParameter->getParamDef()->fOwner != IParamDef::Owner::kRT)
  {
    DLOG_F(ERROR, "only RT owned parameter allowed [%d]", paramID);
    return kInvalidArgument;
  }

  fVstParameters[paramID] = std::move(iParameter);
  fAllRegistrationOrder.emplace_back(paramID);

  return kResultOk;
}

//------------------------------------------------------------------------
// RTState::addOutboundMessagingParameter
//------------------------------------------------------------------------
tresult RTState::addOutboundMessagingParameter(std::unique_ptr<IRTJmbOutParameter> iParameter)
{
  if(!iParameter)
    return kInvalidArgument;

  ParamID paramID = iParameter->getParamID();
  if(fVstParameters.find(paramID) != fVstParameters.cend() ||
     fOutboundMessagingParameters.find(paramID) != fOutboundMessagingParameters.cend() ||
     fInboundMessagingParameters.find(paramID) != fInboundMessagingParameters.cend())
  {
    DLOG_F(ERROR, "duplicate paramID [%d]", paramID);
    return kInvalidArgument;
  }

  if(iParameter->getParamDef()->fOwner != IParamDef::Owner::kRT)
  {
    DLOG_F(ERROR, "only RT owned parameter allowed for outbound messaging [%d]", paramID);
    return kInvalidArgument;
  }

  if(!iParameter->getParamDef()->fShared)
  {
    DLOG_F(ERROR, "param [%d] must be shared", paramID);
    return kInvalidArgument;
  }

  fOutboundMessagingParameters[paramID] = std::move(iParameter);
  fAllRegistrationOrder.emplace_back(paramID);

  return kResultOk;
}

//------------------------------------------------------------------------
// RTState::addInboundMessagingParameter
//------------------------------------------------------------------------
tresult RTState::addInboundMessagingParameter(std::unique_ptr<IRTJmbInParameter> iParameter)
{
  if(!iParameter)
    return kInvalidArgument;

  ParamID paramID = iParameter->getParamID();
  if(fVstParameters.find(paramID) != fVstParameters.cend() ||
     fOutboundMessagingParameters.find(paramID) != fOutboundMessagingParameters.cend() ||
     fInboundMessagingParameters.find(paramID) != fInboundMessagingParameters.cend())
  {
    DLOG_F(ERROR, "duplicate paramID [%d]", paramID);
    return kInvalidArgument;
  }

  if(iParameter->getParamDef()->fOwner != IParamDef::Owner::kGUI)
  {
    DLOG_F(ERROR, "only GUI owned parameter allowed for inbound messaging [%d]", paramID);
    return kInvalidArgument;
  }

  if(!iParameter->getParamDef()->fShared)
  {
    DLOG_F(ERROR, "param [%d] must be shared", paramID);
    return kInvalidArgument;
  }

  fInboundMessagingParameters[paramID] = std::move(iParameter);
  fAllRegistrationOrder.emplace_back(paramID);

  return kResultOk;
}


//------------------------------------------------------------------------
// RTState::applyParameterChanges
//------------------------------------------------------------------------
bool RTState::applyParameterChanges(IParameterChanges &inputParameterChanges)
{
  int32 numParamsChanged = inputParameterChanges.getParameterCount();
  if(numParamsChanged <= 0)
    return false;

  bool stateChanged = false;

  for(int i = 0; i < numParamsChanged; ++i)
  {
    IParamValueQueue *paramQueue = inputParameterChanges.getParameterData(i);
    if(paramQueue != nullptr)
    {
      ParamValue value;
      int32 sampleOffset;
      int32 numPoints = paramQueue->getPointCount();

      // we read the "last" point (ignoring multiple changes for now)
      if(paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultOk)
      {
        auto item = fVstParameters.find(paramQueue->getParameterId());
        if(item != fVstParameters.cend())
        {
          stateChanged |= item->second->updateNormalizedValue(value);
        }
      }
    }
  }

  return stateChanged;
}

//------------------------------------------------------------------------
// RTState::getParamUpdateSampleOffset
//------------------------------------------------------------------------
int32 RTState::getParamUpdateSampleOffset(ProcessData &iData, ParamID iParamID) const
{
  // check for actual changes
  auto inputParameterChanges = iData.inputParameterChanges;
  if(!inputParameterChanges)
    return -1;

  int32 numParamsChanged = inputParameterChanges->getParameterCount();
  if(numParamsChanged <= 0)
    return -1;

  int32 offset = -1;

  for(int i = 0; i < numParamsChanged; ++i)
  {
    IParamValueQueue *paramQueue = inputParameterChanges->getParameterData(i);
    if(paramQueue != nullptr && paramQueue->getParameterId() == iParamID)
    {
      ParamValue value;
      int32 sampleOffset;
      int32 numPoints = paramQueue->getPointCount();

      // we read the "last" point (ignoring multiple changes for now)
      if(paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultOk)
      {
        offset = sampleOffset;
      }
    }
  }

  return offset;
}

//------------------------------------------------------------------------
// RTState::computeLatestState
//------------------------------------------------------------------------
void RTState::computeLatestState(NormalizedState *oLatestState) const
{
  auto const &saveOrder = oLatestState->fSaveOrder;

  for(int i = 0; i < oLatestState->getCount(); i++)
  {
    auto paramID = saveOrder->fOrder[i];
    oLatestState->set(i, fVstParameters.at(paramID)->getNormalizedValue());
  }
}

//------------------------------------------------------------------------
// RTState::computeLatestState
//------------------------------------------------------------------------
void RTState::computeLatestState()
{
  fLatestState.update([this](auto iNormalizedStateRT) {
    this->computeLatestState(iNormalizedStateRT);
  });
}

//------------------------------------------------------------------------
// RTState::readNewState
//------------------------------------------------------------------------
tresult RTState::readNewState(IBStreamer &iStreamer)
{
  bool res = fStateUpdate.updateAndPushIf([this, &iStreamer](auto oNormalizedState) -> bool {
    if(fPluginParameters.readRTState(iStreamer, oNormalizedState) == kResultOk)
    {
      afterReadNewState(oNormalizedState);
      return true;
    }
    return false;
  });
  return res ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
// RTState::writeLatestState
//------------------------------------------------------------------------
tresult RTState::writeLatestState(IBStreamer &oStreamer)
{
  auto normalizedState = fLatestState.get();

  beforeWriteNewState(normalizedState);

  return fPluginParameters.writeRTState(normalizedState, oStreamer);
}

//------------------------------------------------------------------------
// RTState::beforeProcessing
//------------------------------------------------------------------------
bool RTState::beforeProcessing()
{
  bool res = false;

  auto state = fStateUpdate.pop();
  if(state)
  {
    res |= onNewState(state);
  }

  return res;
}

//------------------------------------------------------------------------
// RTState::onNewState
//------------------------------------------------------------------------
bool RTState::onNewState(NormalizedState const *iLatestState)
{
  auto const &saveOrder = iLatestState->fSaveOrder;

  bool res = false;

  for(int i = 0; i < iLatestState->getCount(); i ++)
  {
    res |= fVstParameters.at(saveOrder->fOrder[i])->updateNormalizedValue(iLatestState->fValues[i]);
  }

  return res;
}

//------------------------------------------------------------------------
// RTState::afterProcessing
//------------------------------------------------------------------------
void RTState::afterProcessing()
{
  // when the state has changed we update latest state for writeLatestState
  if(resetPreviousValues())
  {
    computeLatestState();
  }
}

//------------------------------------------------------------------------
// RTState::resetPreviousValues
//------------------------------------------------------------------------
bool RTState::resetPreviousValues()
{
  bool stateChanged = false;
  for(auto &iter : fVstParameters)
  {
    stateChanged |= iter.second->resetPreviousValue();
  }

  return stateChanged;
}

//------------------------------------------------------------------------
// RTState::sendPendingMessages
//------------------------------------------------------------------------
tresult RTState::sendPendingMessages(IMessageProducer *iMessageProducer)
{
  tresult res = kResultOk;

  for(auto &p : fOutboundMessagingParameters)
  {
    std::unique_ptr<IRTJmbOutParameter> &param = p.second;
    if(param->hasUpdate())
    {
      auto message = iMessageProducer->allocateMessage();

      if(message)
      {
        Message m{message.get()};

        // sets the message ID
        m.setMessageID(param->getParamID());

        // serialize the content
        if(param->writeToMessage(m) == kResultOk)
          res |= iMessageProducer->sendMessage(message);
        else
          res = kResultFalse;
      }
      else
        res = kResultFalse;
    }
  }

  return res;
}

//------------------------------------------------------------------------
// RTState::handleMessage
//------------------------------------------------------------------------
tresult RTState::handleMessage(Message const &iMessage)
{
  return fMessageHandler.handleMessage(iMessage);
}

//------------------------------------------------------------------------
// RTState::init
//------------------------------------------------------------------------
tresult RTState::init()
{
  tresult result = kResultOk;

  auto state = fPluginParameters.newRTState();

  for(int i = 0; i < state->getCount(); i++)
  {
    auto paramID = state->fSaveOrder->fOrder[i];
    // param exist
    if(fVstParameters.find(paramID) == fVstParameters.cend())
    {
      result = kResultFalse;
      DLOG_F(ERROR,
             "Expected parameter [%d] used in RTSaveStateOrder not registered",
             paramID);
    }
    else
    {
      if(fVstParameters.at(paramID)->getParamDef()->fTransient)
      {
        result = kResultFalse;
        DLOG_F(ERROR,
               "Parameter [%d] is marked transient => should not be part of save state order",
               paramID);
      }
    }
  }
  DCHECK_F(result == kResultOk, "Issue with parameters... failing in development mode");

  if(result == kResultOk)
  {
    computeLatestState();
  }

  if(fPluginParameters.getRTSaveStateOrder().fVersion == 0)
  {
    DLOG_F(WARNING, "RT Save State version is using the default entry order. Use Parameters::setRTSaveStateOrder to set explicitely.");
  }

  return result;
}

}