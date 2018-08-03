#include "GUIParamCxAware.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIParamCxAware::registerRawVstParam
///////////////////////////////////////////
std::shared_ptr<GUIRawVstParameter> GUIParamCxAware::registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    return nullptr;

  return fParamCxMgr->registerRawVstParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

///////////////////////////////////////////
// GUIParamCxAware::registerVstBooleanParam
///////////////////////////////////////////
GUIVstParam<bool> GUIParamCxAware::registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<bool>(iParamID, iSubscribeToChanges);
}

///////////////////////////////////////////
// GUIParamCxAware::registerVstPercentParam
///////////////////////////////////////////
GUIVstParam<Percent> GUIParamCxAware::registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<Percent>(iParamID, iSubscribeToChanges);
}

}
}
}
}