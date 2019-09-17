/*
 * Copyright (c) 2018-2019 pongasoft
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
#include "GUIParamCxAware.hpp"
#include "GUIParamCxAware.h"

#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft::VST::GUI::Params {

//------------------------------------------------------------------------
// GUIParamCxAware::~GUIParamCxAware
//------------------------------------------------------------------------
GUIParamCxAware::~GUIParamCxAware()
{
  fParamCxMgr = nullptr;
}

//------------------------------------------------------------------------
// GUIParamCxAware::initState
//------------------------------------------------------------------------
void GUIParamCxAware::initState(GUIState *iGUIState)
{
  fParamCxMgr = iGUIState->createParamCxMgr();
}

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
// GUIParamCxAware::registerRawVstCallback
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::registerRawVstCallback(ParamID iParamID,
                                                       Parameters::ChangeCallback iChangeCallback,
                                                       bool iInvokeCallback)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerRawVstCallback
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::registerRawVstCallback(ParamID iParamID,
                                                       Parameters::ChangeCallback1<GUIRawVstParam> iChangeCallback,
                                                       bool iInvokeCallback)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
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

//------------------------------------------------------------------------
// GUIParamCxAware::invokeAll
//------------------------------------------------------------------------
void GUIParamCxAware::invokeAll()
{
  if(fParamCxMgr)
    fParamCxMgr->invokeAll();
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerOptionalDiscreteParam
//------------------------------------------------------------------------
GUIOptionalParam<int32> GUIParamCxAware::registerOptionalDiscreteParam(TagID iParamID,
                                                                       int32 iStepCount,
                                                                       bool iSubscribeToChanges)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalDiscreteParam(iParamID, iStepCount, iSubscribeToChanges ? this : nullptr);
  }
  else
    return GUIOptionalParam<int32>();
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerOptionalDiscreteCallback
//------------------------------------------------------------------------
GUIOptionalParam<int32> GUIParamCxAware::registerOptionalDiscreteCallback(TagID iParamID,
                                                                          int32 iStepCount,
                                                                          Parameters::ChangeCallback iChangeCallback,
                                                                          bool iInvokeCallback)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalDiscreteCallback(iParamID, iStepCount, std::move(iChangeCallback), iInvokeCallback);
  }
  else
    return GUIOptionalParam<int32>();
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerOptionalDiscreteCallback
//------------------------------------------------------------------------
GUIOptionalParam<int32> GUIParamCxAware::registerOptionalDiscreteCallback(TagID iParamID,
                                                                          int32 iStepCount,
                                                                          Parameters::ChangeCallback1<GUIOptionalParam<int32>> iChangeCallback,
                                                                          bool iInvokeCallback)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalDiscreteCallback(iParamID, iStepCount, std::move(iChangeCallback), iInvokeCallback);
  }
  else
    return GUIOptionalParam<int32>();
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerBaseParam
//------------------------------------------------------------------------
IGUIParam GUIParamCxAware::registerBaseParam(TagID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseParam(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return IGUIParam();
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerBaseCallback
//------------------------------------------------------------------------
IGUIParam GUIParamCxAware::registerBaseCallback(TagID iParamID,
                                                Parameters::ChangeCallback iChangeCallback,
                                                bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return IGUIParam();
}


//------------------------------------------------------------------------
// GUIParamCxAware::registerBaseCallback
//------------------------------------------------------------------------
IGUIParam GUIParamCxAware::registerBaseCallback(TagID iParamID,
                                                Parameters::ChangeCallback1<IGUIParam> iChangeCallback,
                                                bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return IGUIParam();
}


}