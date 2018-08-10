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
#include "RTState.h"

namespace pongasoft {
namespace VST {
namespace RT {

//------------------------------------------------------------------------
// RTState::RTState
//------------------------------------------------------------------------
RTState::RTState(Parameters const &iParameters) :
  fPluginParameters{iParameters},
  fStateUpdate{iParameters.newRTState(), true},
  fLatestState{iParameters.newRTState()},
  fNormalizedStateRT{iParameters.newRTState()}
{
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
     fOutboundMessagingParameters.find(paramID) != fOutboundMessagingParameters.cend())
  {
    DLOG_F(ERROR, "duplicate paramID [%d]", paramID);
    return kInvalidArgument;
  }

  if(iParameter->getParamDef()->fOwner == IParamDef::Owner::kGUI)
  {
    DLOG_F(ERROR, "only RT parameter allowed [%d] (owned by GUI)", paramID);
    return kInvalidArgument;
  }

  fVstParameters[paramID] = std::move(iParameter);

  return kResultOk;
}

//------------------------------------------------------------------------
// RTState::addOutboundMessagingParameter
//------------------------------------------------------------------------
tresult RTState::addOutboundMessagingParameter(std::unique_ptr<IRTJmbParameter> iParameter)
{
  if(!iParameter)
    return kInvalidArgument;

  ParamID paramID = iParameter->getParamID();
  if(fVstParameters.find(paramID) != fVstParameters.cend() ||
     fOutboundMessagingParameters.find(paramID) != fOutboundMessagingParameters.cend())
  {
    DLOG_F(ERROR, "duplicate paramID [%d]", paramID);
    return kInvalidArgument;
  }

  if(iParameter->getParamDef()->fOwner == IParamDef::Owner::kGUI)
  {
    DLOG_F(ERROR, "only RT parameter allowed [%d] (owned by GUI)", paramID);
    return kInvalidArgument;
  }

  if(!iParameter->getParamDef()->fShared)
  {
    DLOG_F(ERROR, "param [%d] must be shared", paramID);
    return kInvalidArgument;
  }

  fOutboundMessagingParameters[paramID] = std::move(iParameter);

  return kResultOk;
}

//------------------------------------------------------------------------
// IRTState::applyParameterChanges
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
  computeLatestState(fNormalizedStateRT.get());

  // atomically copy it for access later by GUI thread in writeLatestState
  fLatestState.set(fNormalizedStateRT.get());
}

//------------------------------------------------------------------------
// RTState::readNewState
//------------------------------------------------------------------------
tresult RTState::readNewState(IBStreamer &iStreamer)
{
  auto normalizedState = fPluginParameters.readRTState(iStreamer);

  if(normalizedState)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "RTState::readNewState - %s", normalizedState->toString().c_str());
#endif

    // atomically copy it for access later by RT thread in before processing
    fStateUpdate.push(normalizedState.get());
    return kResultOk;
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTState::writeLatestState
//------------------------------------------------------------------------
tresult RTState::writeLatestState(IBStreamer &oStreamer)
{
  // YP Implementation note: It is OK to allocate memory here because this method is called by the GUI!!!
  auto normalizedState = fPluginParameters.newRTState();

  fLatestState.get(normalizedState.get());

  tresult res = fPluginParameters.writeRTState(normalizedState.get(), oStreamer);

  if(res == kResultOk)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "RTState::writeLatestState - %s", normalizedState->toString().c_str());
#endif
  }
  return res;
}

//------------------------------------------------------------------------
// RTState::beforeProcessing
//------------------------------------------------------------------------
bool RTState::beforeProcessing()
{
  if(fStateUpdate.pop(fNormalizedStateRT.get()))
  {
    return onNewState(fNormalizedStateRT.get());
  }
  return false;
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
    std::unique_ptr<IRTJmbParameter> &param = p.second;
    if(param->hasOutboundUpdate())
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
// RTState::init
//------------------------------------------------------------------------
tresult RTState::init()
{
  tresult result = kResultOk;
  for(int i = 0; i < fNormalizedStateRT->getCount(); i++)
  {
    auto paramID = fNormalizedStateRT->fSaveOrder->fOrder[i];
    // param exist
    if(fVstParameters.find(paramID) == fVstParameters.cend())
    {
      result = kResultFalse;
      DLOG_F(ERROR,
             "Expected parameter [%d] used in RTSaveStateOrder not registered",
             paramID);
    }
    if(fVstParameters.at(paramID)->getParamDef()->fTransient)
    {
      result = kResultFalse;
      DLOG_F(ERROR,
             "Parameter [%d] is marked transient => should not be part of save state order",
             paramID);
    }
  }
  DCHECK_F(result == kResultOk, "Issue with parameters... failing in development mode");

  if(result == kResultOk)
  {
    computeLatestState();
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "RT Init State - %s", fNormalizedStateRT->toString().c_str());
#endif
  }

  return result;
}


}
}
}