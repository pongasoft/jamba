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
// GUIParameters::setParamNormalized
//------------------------------------------------------------------------
tresult GUIParameters::setParamNormalized(NormalizedState const *iNormalizedState)
{
  tresult res = kResultOk;

  for(int i = 0; i < iNormalizedState->getCount(); i++)
  {
    auto paramID = iNormalizedState->fSaveOrder->fOrder[i];
    res |= fHostParameters.setParamNormalized(paramID, iNormalizedState->fValues[i]);
  }

  return res;
}

//------------------------------------------------------------------------
// GUIParameters::readRTState
//------------------------------------------------------------------------
tresult GUIParameters::readRTState(IBStreamer &iStreamer)
{
  auto normalizedState = fPluginParameters.readRTState(iStreamer);

  if(normalizedState)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "GUIParameters::readRTState - %s", normalizedState->toString().c_str());
#endif
    return setParamNormalized(normalizedState.get());
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIParameters::readGUIState
//------------------------------------------------------------------------
tresult GUIParameters::readGUIState(IBStreamer &iStreamer)
{
  auto normalizedState = fPluginParameters.readGUIState(iStreamer);

  if(normalizedState)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "GUIParameters::readGUIState - %s", normalizedState->toString().c_str());
#endif
    return setParamNormalized(normalizedState.get());
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIParameters::writeGUIState
//------------------------------------------------------------------------
tresult GUIParameters::writeGUIState(IBStreamer &oStreamer) const
{
  // YP Implementation note: It is OK to allocate memory here because this method is called by the GUI!!!
  auto normalizedState = fPluginParameters.newGUIState();

  auto const &saveOrder = normalizedState->fSaveOrder;

  for(int i = 0; i < normalizedState->getCount(); i++)
  {
    auto paramID = saveOrder->fOrder[i];
    normalizedState->set(i, fHostParameters.getParamNormalized(paramID));
  }

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIParameters::writeGUIState - %s", normalizedState->toString().c_str());
#endif

  return fPluginParameters.writeGUIState(normalizedState.get(), oStreamer);
}

}
}
}
}