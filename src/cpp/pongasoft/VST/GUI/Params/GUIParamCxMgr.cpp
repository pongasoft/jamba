#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::registerRawVstParam
//------------------------------------------------------------------------
std::shared_ptr<GUIRawVstParameter> GUIParamCxMgr::registerRawVstParam(ParamID iParamID,
                                                                       Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  if(iChangeListener)
  {
    fParamCxs[iParamID] = std::move(param->connect(iChangeListener));
  }
  else
  {
    unregisterParam(iParamID);
  }

  return param;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::unregisterParam
//------------------------------------------------------------------------
bool GUIParamCxMgr::unregisterParam(ParamID iParamID)
{
  return fParamCxs.erase(iParamID) == 1;
}

}
}
}
}