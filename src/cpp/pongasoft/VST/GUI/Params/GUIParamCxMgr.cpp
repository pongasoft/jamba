#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::registerRawVstParam
//------------------------------------------------------------------------
std::unique_ptr<GUIRawVstParameter>
GUIParamCxMgr::registerRawVstParam(ParamID iParamID, Parameters::IChangeListener *iChangeListener)
{
  auto parameter = fGUIState->getRawVstParameter(iParamID);

  DCHECK_F(parameter != nullptr, "param [%d] not found", iParamID);

  if(iChangeListener)
  {
    fParamCxs[iParamID] = std::move(parameter->connect(iChangeListener));
  }

  return parameter;
}

}
}
}
}