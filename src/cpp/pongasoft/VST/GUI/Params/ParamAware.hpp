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
#pragma once

#include "ParamAware.h"
#include "GUIParamCxMgr.h"

namespace pongasoft::VST::GUI::Params {


//------------------------------------------------------------------------
// ParamAware::registerOptionalParam
//------------------------------------------------------------------------
template<typename T>
GUIOptionalParam<T> ParamAware::registerOptionalParam(TagID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  }
  else
    return GUIOptionalParam<T>();
}

//------------------------------------------------------------------------
// ParamAware::registerOptionalCallback
//------------------------------------------------------------------------
template<typename T>
GUIOptionalParam<T> ParamAware::registerOptionalCallback(TagID iParamID,
                                                         Parameters::ChangeCallback iChangeCallback,
                                                         bool iInvokeCallback)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  }
  else
    return GUIOptionalParam<T>();
}

//------------------------------------------------------------------------
// ParamAware::registerOptionalCallback
//------------------------------------------------------------------------
template<typename T>
GUIOptionalParam<T> ParamAware::registerOptionalCallback(TagID iParamID,
                                                         Parameters::ChangeCallback1<GUIOptionalParam<T>> iChangeCallback,
                                                         bool iInvokeCallback)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  }
  else
    return GUIOptionalParam<T>();
}

//------------------------------------------------------------------------
// ParamAware::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> ParamAware::registerVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::registerVstCallback
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> ParamAware::registerVstCallback(ParamID iParamID,
                                               Parameters::ChangeCallback iChangeCallback,
                                               bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::registerVstCallback
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> ParamAware::registerVstCallback(ParamID iParamID,
                                               Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                               bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::registerJmbParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> ParamAware::registerJmbParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::registerJmbCallback
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> ParamAware::registerJmbCallback(ParamID iParamID,
                                               Parameters::ChangeCallback iChangeCallback,
                                               bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::registerJmbCallback
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> ParamAware::registerJmbCallback(ParamID iParamID,
                                               Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                                               bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::unregisterParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> ParamAware::unregisterParam(GUIVstParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// ParamAware::unregisterParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> ParamAware::unregisterParam(GUIJmbParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIJmbParam<T>{};
}

}