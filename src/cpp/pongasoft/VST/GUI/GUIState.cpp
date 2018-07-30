#include <sstream>
#include "GUIState.h"

namespace pongasoft {
namespace VST {
namespace GUI {

//------------------------------------------------------------------------
// GUIParameters::addSerializableParameter
//------------------------------------------------------------------------
void GUIState::addSerializableParameter(std::shared_ptr<GUISerializableParameter> const &iParameter)
{
  ParamID paramID = iParameter->getParamID();

  DCHECK_F(iParameter != nullptr);
  DCHECK_F(fPluginParameters.getSerializableParamDef(paramID) != nullptr, "serializable parameter [%d] not registered", paramID);
  DCHECK_F(fSerializableParameters.find(paramID) == fSerializableParameters.cend(), "duplicate paramID [%d]", paramID);
  DCHECK_F(fPluginParameters.getSerializableParamDef(paramID)->fUIOnly, "only GUI parameter allowed");

  fSerializableParameters[paramID] = iParameter;
}

//------------------------------------------------------------------------
// GUIParameters::setParamNormalized
//------------------------------------------------------------------------
tresult GUIState::setParamNormalized(NormalizedState const *iNormalizedState)
{
  tresult res = kResultOk;

  for(int i = 0; i < iNormalizedState->getCount(); i++)
  {
    auto paramID = iNormalizedState->fSaveOrder->fOrder[i];
    res |= fHostParameters->setParamNormalized(paramID, iNormalizedState->fValues[i]);
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
    auto iter = fSerializableParameters.find(paramID);
    if(iter == fSerializableParameters.cend())
    {
      // not found => regular parameter
      ParamValue defaultValue = fPluginParameters.getRawParamDef(paramID)->fDefaultValue;
      ParamValue value = RawParamSerializer::readFromStream(iStreamer, defaultValue);
      fHostParameters->setParamNormalized(paramID, value);
      
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
// GUIParameters::writeGUIState
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
    auto iter = fSerializableParameters.find(paramID);
    if(iter == fSerializableParameters.cend())
    {
      ParamValue value = fHostParameters->getParamNormalized(paramID);
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
// GUIParameters::init
//------------------------------------------------------------------------
tresult GUIState::init(HostParameters const &iHostParameters)
{
  fHostParameters = std::make_unique<HostParameters>(iHostParameters);
  return kResultOk;
}

}
}
}