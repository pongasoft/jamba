#include "RTState.h"

namespace pongasoft {
namespace VST {
namespace RT {

//------------------------------------------------------------------------
// RTState::RTState
//------------------------------------------------------------------------
RTState::RTState(Parameters const &iParameters) :
  fSaveStateOrder{iParameters.getRTSaveStateOrder()},
  fStateUpdate{NormalizedState{fSaveStateOrder.getParamCount()}, true},
  fLatestState{NormalizedState{fSaveStateOrder.getParamCount()}},
  fNormalizedStateRT{fSaveStateOrder.getParamCount()}
{
}

//------------------------------------------------------------------------
// RTState::RTState
//------------------------------------------------------------------------
void RTState::addRawParameter(std::shared_ptr<RTRawParameter> const &iParameter)
{
  ParamID paramID = iParameter->getParamID();

  DCHECK_F(iParameter != nullptr);
  DCHECK_F(fParameters.find(paramID) == fParameters.cend(), "duplicate paramID [%d]", paramID);
  DCHECK_F(!iParameter->getRawParamDef()->fUIOnly, "only RT parameter allowed");

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
void RTState::computeLatestState()
{
  for(int i = 0; i < fNormalizedStateRT.fCount; i++)
  {
    auto paramID = fSaveStateOrder.fOrder[i];
    fNormalizedStateRT.set(i, fParameters.at(paramID)->getNormalizedValue());
  }

  fLatestState.set(fNormalizedStateRT);
}

//------------------------------------------------------------------------
// RTState::readNewState
//------------------------------------------------------------------------
tresult RTState::readNewState(IBStreamer &iStreamer)
{
  // YP Implementation note: It is OK to allocate memory here because this method is called by the GUI!!!
  NormalizedState normalizedState{fSaveStateOrder.getParamCount()};

  uint16 stateVersion;
  if(!iStreamer.readInt16u(stateVersion))
    stateVersion = fSaveStateOrder.fVersion;

  // TODO handle multiple versions
  if(stateVersion != fSaveStateOrder.fVersion)
  {
    DLOG_F(WARNING, "unexpected state version %d", stateVersion);
  }

  for(int i = 0; i < normalizedState.fCount; i++)
  {
    auto paramID = fSaveStateOrder.fOrder[i];
    // readNormalizedValue handles default values
    normalizedState.set(i, fParameters.at(paramID)->getRawParamDef()->readNormalizedValue(iStreamer));
  }

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "RTState::readNewState - v=%d, %s", fSaveStateOrder.fVersion, normalizedState.toString(fSaveStateOrder.fOrder.data()).c_str());
#endif

  fStateUpdate.push(normalizedState);

  return kResultOk;
}

//------------------------------------------------------------------------
// RTState::writeLatestState
//------------------------------------------------------------------------
tresult RTState::writeLatestState(IBStreamer &oStreamer)
{
  // YP Implementation note: It is OK to allocate memory here because this method is called by the GUI!!!
  NormalizedState normalizedState{fSaveStateOrder.getParamCount()};

  fLatestState.get(normalizedState);

  // write version for later upgrade
  oStreamer.writeInt16u(fSaveStateOrder.fVersion);

  for(int i = 0; i < normalizedState.fCount; i ++)
  {
    oStreamer.writeDouble(normalizedState.fValues[i]);
  }

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "RTState::writeLatestState - v=%d, %s", fSaveStateOrder.fVersion, normalizedState.toString(fSaveStateOrder.fOrder.data()).c_str());
#endif

  return kResultOk;
}

//------------------------------------------------------------------------
// RTState::beforeProcessing
//------------------------------------------------------------------------
bool RTState::beforeProcessing()
{
  if(fStateUpdate.pop(fNormalizedStateRT))
  {
    bool res = false;

    for(int i = 0; i < fNormalizedStateRT.fCount; i ++)
    {
      res |= fParameters.at(fSaveStateOrder.fOrder[i])->updateNormalizedValue(fNormalizedStateRT.fValues[i]);
    }

    return res;
  }
  return false;
}

//------------------------------------------------------------------------
// RTState::afterProcessing
//------------------------------------------------------------------------
void RTState::afterProcessing()
{
  bool stateChanged = false;
  for(auto &iter : fParameters)
  {
    stateChanged |= iter.second->resetPreviousValue();
  }

  // when the state has changed we update latest state for writeLatestState
  if(stateChanged)
  {
    computeLatestState();
  }
}

//------------------------------------------------------------------------
// RTState::init
//------------------------------------------------------------------------
tresult RTState::init()
{
  tresult result = kResultOk;
  for(int i = 0; i < fSaveStateOrder.getParamCount(); i++)
  {
    auto paramID = fSaveStateOrder.fOrder[i];
    // param exist
    if(fParameters.find(paramID) == fParameters.cend())
    {
      result = kResultFalse;
      DLOG_F(ERROR,
             "Expected parameter [%d] used in RTSaveStateOrder not registered",
             paramID);
    }
    if(fParameters.at(paramID)->getRawParamDef()->fTransient)
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
    DLOG_F(INFO, "RT Init State - %s", fNormalizedStateRT.toString(fSaveStateOrder.fOrder.data()).c_str());
#endif
  }

  return result;
}


}
}
}