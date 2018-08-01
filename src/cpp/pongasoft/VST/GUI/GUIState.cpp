#include <sstream>
#include "GUIState.h"
#include <pongasoft/VST/GUI/Params/GUIParamCxMgr.h>

namespace pongasoft {
namespace VST {
namespace GUI {

//------------------------------------------------------------------------
// GUIState::addSerParam
//------------------------------------------------------------------------
void GUIState::addSerParam(std::shared_ptr<IGUISerParameter> const &iParameter)
{
  DCHECK_F(iParameter != nullptr);

  ParamID paramID = iParameter->getParamID();

  DCHECK_F(fPluginParameters.getSerParamDef(paramID) != nullptr, "serializable parameter [%d] not registered", paramID);
  DCHECK_F(fSerParams.find(paramID) == fSerParams.cend(), "duplicate paramID [%d]", paramID);
  DCHECK_F(fPluginParameters.getSerParamDef(paramID)->fUIOnly, "only GUI parameter allowed");

  fSerParams[paramID] = iParameter;
}

//------------------------------------------------------------------------
// GUIState::setParamNormalized
//------------------------------------------------------------------------
tresult GUIState::setParamNormalized(NormalizedState const *iNormalizedState)
{
  tresult res = kResultOk;

  for(int i = 0; i < iNormalizedState->getCount(); i++)
  {
    auto paramID = iNormalizedState->fSaveOrder->fOrder[i];
    res |= fVstParameters->setParamNormalized(paramID, iNormalizedState->fValues[i]);
  }

  return res;
}

//------------------------------------------------------------------------
// GUIState::readRTState
//------------------------------------------------------------------------
tresult GUIState::readRTState(IBStreamer &iStreamer)
{
  auto normalizedState = fPluginParameters.readRTState(iStreamer);

  if(normalizedState)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "GUIState::readRTState - %s", normalizedState->toString().c_str());
#endif
    return setParamNormalized(normalizedState.get());
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIState::readGUIState
//------------------------------------------------------------------------
tresult GUIState::readGUIState(IBStreamer &iStreamer)
{
  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();

  if(saveOrder.fVersion >= 0)
  {
    uint16 stateVersion;
    if(!iStreamer.readInt16u(stateVersion))
      stateVersion = 0;

    // TODO handle multiple versions
    if(stateVersion != saveOrder.fVersion)
    {
      DLOG_F(WARNING, "unexpected GUI state version %d", stateVersion);
    }
  }

#ifdef JAMBA_DEBUG_LOGGING
  std::ostringstream state{};
  state << "{v=" << saveOrder.fVersion;
#endif

  tresult res = kResultOk;

  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    auto iter = fSerParams.find(paramID);
    if(iter == fSerParams.cend())
    {
      // not found => regular parameter
      ParamValue value = fPluginParameters.readNormalizedValue(paramID, iStreamer);
      fVstParameters->setParamNormalized(paramID, value);
      
#ifdef JAMBA_DEBUG_LOGGING
      state << ", " << paramID << "=" << value;
#endif
    }
    
    else
    {
      res |= iter->second->readFromStream(iStreamer);

#ifdef JAMBA_DEBUG_LOGGING
      state << ", " << paramID;
#endif
    }
  }

#ifdef JAMBA_DEBUG_LOGGING
  state << "}";
  DLOG_F(INFO, "GUIState::readGUIState - %s", state.str().c_str());
#endif

  return res;
}

//------------------------------------------------------------------------
// GUIState::writeGUIState
//------------------------------------------------------------------------
tresult GUIState::writeGUIState(IBStreamer &oStreamer) const
{
  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();

  if(saveOrder.fVersion >= 0)
    oStreamer.writeInt16u(static_cast<uint16>(saveOrder.fVersion));

#ifdef JAMBA_DEBUG_LOGGING
  std::ostringstream state{};
  state << "{v=" << saveOrder.fVersion;
#endif

  tresult res = kResultOk;

  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    auto iter = fSerParams.find(paramID);
    if(iter == fSerParams.cend())
    {
      ParamValue value = fVstParameters->getParamNormalized(paramID);
      res |= RawParamSerializer::writeToStream(value, oStreamer);

#ifdef JAMBA_DEBUG_LOGGING
      state << ", " << paramID << "=" << value;
#endif
    }
    else
    {
      res |= iter->second->writeToStream(oStreamer);

      #ifdef JAMBA_DEBUG_LOGGING
      state << ", " << paramID;
#endif
    }
  }

#ifdef JAMBA_DEBUG_LOGGING
  state << "}";
  DLOG_F(INFO, "GUIState::writeGUIState - %s", state.str().c_str());
#endif

  return res;
}

//------------------------------------------------------------------------
// GUIState::init
//------------------------------------------------------------------------
tresult GUIState::init(VstParametersSPtr iVstParameters)
{
  fVstParameters = std::move(iVstParameters);
  return kResultOk;
}

//------------------------------------------------------------------------
// GUIState::createParamCxMgr
//------------------------------------------------------------------------
std::unique_ptr<GUIParamCxMgr> GUIState::createParamCxMgr()
{
  return std::unique_ptr<GUIParamCxMgr>(new GUIParamCxMgr(this));
}

//------------------------------------------------------------------------
// GUIState::getSerParameter
//------------------------------------------------------------------------
std::shared_ptr<IGUISerParameter> GUIState::getSerParameter(ParamID iParamID) const
{
  auto iter = fSerParams.find(iParamID);
  if(iter == fSerParams.cend())
    return nullptr;
  return iter->second;
}

}
}
}