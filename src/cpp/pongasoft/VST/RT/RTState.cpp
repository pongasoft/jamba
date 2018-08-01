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
// RTState::RTState
//------------------------------------------------------------------------
void RTState::addRawParameter(std::shared_ptr<RTRawVstParameter> const &iParameter)
{
  ParamID paramID = iParameter->getParamID();

  DCHECK_F(iParameter != nullptr);
  DCHECK_F(fParameters.find(paramID) == fParameters.cend(), "duplicate paramID [%d]", paramID);
  DCHECK_F(!iParameter->getParamDef()->fUIOnly, "only RT parameter allowed");

  fParameters[paramID] = iParameter;
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
        auto item = fParameters.find(paramQueue->getParameterId());
        if(item != fParameters.cend())
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
    oLatestState->set(i, fParameters.at(paramID)->getNormalizedValue());
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
    res |= fParameters.at(saveOrder->fOrder[i])->updateNormalizedValue(iLatestState->fValues[i]);
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
  for(auto &iter : fParameters)
  {
    stateChanged |= iter.second->resetPreviousValue();
  }

  return stateChanged;
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
    if(fParameters.find(paramID) == fParameters.cend())
    {
      result = kResultFalse;
      DLOG_F(ERROR,
             "Expected parameter [%d] used in RTSaveStateOrder not registered",
             paramID);
    }
    if(fParameters.at(paramID)->getParamDef()->fTransient)
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