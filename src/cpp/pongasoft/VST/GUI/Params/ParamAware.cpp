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
#include "ParamAware.h"
#include "ParamAware.hpp"

#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft::VST::GUI::Params {

//------------------------------------------------------------------------
// ParamAware::~ParamAware
//------------------------------------------------------------------------
ParamAware::~ParamAware()
{
  fParamCxMgr = nullptr;
}

//------------------------------------------------------------------------
// ParamAware::initState
//------------------------------------------------------------------------
void ParamAware::initState(GUIState *iGUIState)
{
  fParamCxMgr = iGUIState->createParamCxMgr();
}

//------------------------------------------------------------------------
// ParamAware::registerRawVstParam
//------------------------------------------------------------------------
GUIRawVstParam ParamAware::registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

//------------------------------------------------------------------------
// ParamAware::registerRawVstCallback
//------------------------------------------------------------------------
GUIRawVstParam ParamAware::registerRawVstCallback(ParamID iParamID,
                                                  Parameters::ChangeCallback iChangeCallback,
                                                  bool iInvokeCallback)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
}

//------------------------------------------------------------------------
// ParamAware::registerRawVstCallback
//------------------------------------------------------------------------
GUIRawVstParam ParamAware::registerRawVstCallback(ParamID iParamID,
                                                  Parameters::ChangeCallback1<GUIRawVstParam> iChangeCallback,
                                                  bool iInvokeCallback)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
}

//------------------------------------------------------------------------
// ParamAware::registerVstBooleanParam
//------------------------------------------------------------------------
GUIVstParam<bool> ParamAware::registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<bool>(iParamID, iSubscribeToChanges);
}

//------------------------------------------------------------------------
// ParamAware::registerVstPercentParam
//------------------------------------------------------------------------
GUIVstParam<Percent> ParamAware::registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<Percent>(iParamID, iSubscribeToChanges);
}

//------------------------------------------------------------------------
// ParamAware::unregisterParam
//------------------------------------------------------------------------
bool ParamAware::unregisterParam(ParamID iParamID)
{
  if(fParamCxMgr)
    return fParamCxMgr->unregisterParam(iParamID);
  return false;
}

//------------------------------------------------------------------------
// ParamAware::unregisterParam
//------------------------------------------------------------------------
GUIRawVstParam ParamAware::unregisterParam(GUIRawVstParam const &iParam)
{
  if(iParam.exists() && fParamCxMgr)
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIRawVstParam{};
}

//------------------------------------------------------------------------
// ParamAware::unregisterAll
//------------------------------------------------------------------------
void ParamAware::unregisterAll()
{
  if(fParamCxMgr)
    fParamCxMgr->unregisterAll();
}

//------------------------------------------------------------------------
// ParamAware::invokeAll
//------------------------------------------------------------------------
void ParamAware::invokeAll()
{
  if(fParamCxMgr)
    fParamCxMgr->invokeAll();
}

//------------------------------------------------------------------------
// ParamAware::registerOptionalDiscreteParam
//------------------------------------------------------------------------
GUIOptionalParam<int32> ParamAware::registerOptionalDiscreteParam(TagID iParamID,
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
// ParamAware::registerOptionalDiscreteCallback
//------------------------------------------------------------------------
GUIOptionalParam<int32> ParamAware::registerOptionalDiscreteCallback(TagID iParamID,
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
// ParamAware::registerOptionalDiscreteCallback
//------------------------------------------------------------------------
GUIOptionalParam<int32> ParamAware::registerOptionalDiscreteCallback(TagID iParamID,
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
// ParamAware::registerBaseParam
//------------------------------------------------------------------------
IGUIParam ParamAware::registerBaseParam(TagID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseParam(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return IGUIParam();
}

//------------------------------------------------------------------------
// ParamAware::registerBaseCallback
//------------------------------------------------------------------------
IGUIParam ParamAware::registerBaseCallback(TagID iParamID,
                                           Parameters::ChangeCallback iChangeCallback,
                                           bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return IGUIParam();
}


//------------------------------------------------------------------------
// ParamAware::registerBaseCallback
//------------------------------------------------------------------------
IGUIParam ParamAware::registerBaseCallback(TagID iParamID,
                                           Parameters::ChangeCallback1<IGUIParam> iChangeCallback,
                                           bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerBaseCallback(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return IGUIParam();
}


}