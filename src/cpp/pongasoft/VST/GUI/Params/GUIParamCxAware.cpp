/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
#include "GUIParamCxAware.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxAware::registerRawVstParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

//------------------------------------------------------------------------
// GUIParamCxAware::__internal__registerRawVstControl
//------------------------------------------------------------------------
bool GUIParamCxAware::__internal__registerRawVstControl(int32_t iParamID,
                                                        ParamValue &oControlValue,
                                                        GUIRawVstParam &oGUIRawVstParam)
{
  if(!fParamCxMgr)
    return false; // not set yet

  if(iParamID < 0)
  {
    if(oGUIRawVstParam.exists())
    {
      oControlValue = oGUIRawVstParam.getValue();
      oGUIRawVstParam = unregisterParam(oGUIRawVstParam);
    }
  }
  else
  {
    if(!oGUIRawVstParam.exists() || oGUIRawVstParam.getParamID() != iParamID)
    {
      oGUIRawVstParam = registerRawVstParam(static_cast<ParamID>(iParamID));
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerRawVstCallback
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::registerRawVstCallback(ParamID iParamID, Parameters::ChangeCallback iChangeCallback)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstCallback(iParamID, std::move(iChangeCallback));
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerVstBooleanParam
//------------------------------------------------------------------------
GUIVstParam<bool> GUIParamCxAware::registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<bool>(iParamID, iSubscribeToChanges);
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerVstPercentParam
//------------------------------------------------------------------------
GUIVstParam<Percent> GUIParamCxAware::registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<Percent>(iParamID, iSubscribeToChanges);
}


//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
bool GUIParamCxAware::unregisterParam(ParamID iParamID)
{
  if(fParamCxMgr)
    return fParamCxMgr->unregisterParam(iParamID);
  return false;
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::unregisterParam(GUIRawVstParam const &iParam)
{
  if(iParam.exists() && fParamCxMgr)
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIRawVstParam{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterAll
//------------------------------------------------------------------------
void GUIParamCxAware::unregisterAll()
{
  if(fParamCxMgr)
    fParamCxMgr->unregisterAll();
}

}
}
}
}