#include "GUIParamCxAware.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIParamCxAware::registerGUIRawParam
///////////////////////////////////////////
std::unique_ptr<GUIRawParameter> GUIParamCxAware::registerGUIRawParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    ABORT_F("fParamCxMgr should have been registered");

  return fParamCxMgr->registerGUIRawParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

///////////////////////////////////////////
// GUIParamCxAware::registerBooleanParam
///////////////////////////////////////////
GUIParamUPtr<BooleanParamConverter> GUIParamCxAware::registerBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerGUIParam<BooleanParamConverter>(iParamID, iSubscribeToChanges);
}

///////////////////////////////////////////
// GUIParamCxAware::registerPercentParam
///////////////////////////////////////////
GUIParamUPtr<PercentParamConverter> GUIParamCxAware::registerPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerGUIParam<PercentParamConverter>(iParamID, iSubscribeToChanges);
}

}
}
}
}