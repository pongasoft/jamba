#include "GUIParameters.h"
#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParameters::registerVstParameters
//------------------------------------------------------------------------
void GUIParameters::registerVstParameters(Vst::ParameterContainer &iParameterContainer) const
{
  fPluginParameters.registerVstParameters(iParameterContainer);
}

//------------------------------------------------------------------------
// GUIParameters::createParamCxMgr
//------------------------------------------------------------------------
std::unique_ptr<GUIParamCxMgr> GUIParameters::createParamCxMgr() const
{
  return std::unique_ptr<GUIParamCxMgr>(new GUIParamCxMgr(shared_from_this()));
}

//------------------------------------------------------------------------
// GUIParameters::readState
//------------------------------------------------------------------------
tresult GUIParameters::readState(Parameters::SaveStateOrder const &iSaveStateOrder,
                                 IBStreamer &iStreamer,
                                 NormalizedState *oNormalizedState)
{
  uint16 stateVersion;
  if(!iStreamer.readInt16u(stateVersion))
    stateVersion = iSaveStateOrder.fVersion;

  // TODO handle multiple versions
  if(stateVersion != iSaveStateOrder.fVersion)
  {
    DLOG_F(WARNING, "unexpected state version %d", stateVersion);
  }
  
  int i = 0;
  for(auto paramID : iSaveStateOrder.fOrder)
  {
    auto param = fPluginParameters.getRawParamDef(paramID);
    ParamValue defaultNormalizedValue = param ? param->fDefaultNormalizedValue : 0.0;
    ParamValue value = fHostParameters.setParamNormalized(paramID, iStreamer, defaultNormalizedValue);
    if(oNormalizedState)
      oNormalizedState->set(i++, value);
  }

  return kResultOk;
}

//------------------------------------------------------------------------
// GUIParameters::readRTState
//------------------------------------------------------------------------
tresult GUIParameters::readRTState(IBStreamer &iStreamer, NormalizedState *oNormalizedState)
{
  return readState(fPluginParameters.getRTSaveStateOrder(), iStreamer, oNormalizedState);
}


//------------------------------------------------------------------------
// GUIParameters::readGUIState
//------------------------------------------------------------------------
tresult GUIParameters::readGUIState(IBStreamer &iStreamer, NormalizedState *oNormalizedState)
{
  return readState(fPluginParameters.getGUISaveStateOrder(), iStreamer, oNormalizedState);
}

//------------------------------------------------------------------------
// GUIParameters::writeGUIState
//------------------------------------------------------------------------
tresult GUIParameters::writeGUIState(IBStreamer &oStreamer, NormalizedState *oNormalizedState) const
{
  auto sso = fPluginParameters.getGUISaveStateOrder();

  oStreamer.writeInt16u(sso.fVersion);

  int i = 0;
  for(auto paramID : sso.fOrder)
  {
    ParamValue value = fHostParameters.getParamNormalized(paramID);
    oStreamer.writeDouble(value);
    if(oNormalizedState)
      oNormalizedState->set(i++, value);
  }

  return kResultOk;
}

}
}
}
}