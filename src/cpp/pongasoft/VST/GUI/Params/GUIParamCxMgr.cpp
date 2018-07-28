#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::registerGUIRawParam
//------------------------------------------------------------------------
std::unique_ptr<GUIRawParameter>
GUIParamCxMgr::registerGUIRawParam(ParamID iParamID, GUIRawParameter::IChangeListener *iChangeListener)
{
  auto parameter = fParameters->getRawParameter(iParamID);

  if(iChangeListener)
  {
    fParameterConnections[iParamID] = std::move(parameter->connect(iChangeListener));
  }

  return parameter;
}

}
}
}
}