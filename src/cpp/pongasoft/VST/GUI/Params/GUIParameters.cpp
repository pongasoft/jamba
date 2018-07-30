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

}
}
}
}