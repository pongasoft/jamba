#include "GUIParamCxAware.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIParamCxAware::registerRawVstParam
///////////////////////////////////////////
std::unique_ptr<GUIRawVstParameter> GUIParamCxAware::registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    ABORT_F("fParamCxMgr should have been registered");

  return fParamCxMgr->registerRawVstParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

///////////////////////////////////////////
// GUIParamCxAware::registerVstBooleanParam
///////////////////////////////////////////
GUIVstParam<BooleanParamConverter> GUIParamCxAware::registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<BooleanParamConverter>(iParamID, iSubscribeToChanges);
}

///////////////////////////////////////////
// GUIParamCxAware::registerVstPercentParam
///////////////////////////////////////////
GUIVstParam<PercentParamConverter> GUIParamCxAware::registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<PercentParamConverter>(iParamID, iSubscribeToChanges);
}

}
}
}
}