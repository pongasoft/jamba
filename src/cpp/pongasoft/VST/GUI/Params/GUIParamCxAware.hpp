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

#include "GUIParamCxAware.h"
#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {


//------------------------------------------------------------------------
// GUIParamCxAware::registerOptionalParam
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxAware::registerOptionalParam(TagID iParamID, GUIOptionalParam<T> &oParam, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
  {
    return fParamCxMgr->registerOptionalParam(iParamID, oParam, iSubscribeToChanges ? this : nullptr);
  }
  else
    return false;
}


//------------------------------------------------------------------------
// GUIParamCxAware::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxAware::registerVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerVstCallback
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxAware::registerVstCallback(ParamID iParamID,
                                                    Parameters::ChangeCallback iChangeCallback,
                                                    bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxAware::registerParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstParam(iParamDef, iSubscribeToChanges ? this : nullptr);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerCallback
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T>
GUIParamCxAware::registerCallback(VstParam<T> const &iParamDef,
                                  Parameters::ChangeCallback iChangeCallback,
                                  bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstCallback(iParamDef, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerCallback
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxAware::registerCallback(VstParam<T> const &iParamDef,
                                       Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                       bool iInvokeCallback)
{
  return fParamCxMgr ? fParamCxMgr->registerVstCallback(iParamDef, std::move(iChangeCallback), iInvokeCallback) : false;
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerParam(GUIJmbParam<T> &iParamDef)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbParam(iParamDef, this);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerCallback
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerCallback(GUIJmbParam<T> &iParamDef,
                                                 Parameters::ChangeCallback iChangeCallback,
                                                 bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbCallback(iParamDef, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerCallback
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxAware::registerCallback(GUIJmbParam<T> &iParamDef,
                                       Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                                       bool iInvokeCallback)
{
  if(fParamCxMgr)
  {
    fParamCxMgr->registerJmbCallback(iParamDef, std::move(iChangeCallback), iInvokeCallback);
    return true;
  }
  else
    return false;
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerJmbParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerJmbParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerJmbCallback
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerJmbCallback(ParamID iParamID,
                                                    Parameters::ChangeCallback iChangeCallback,
                                                    bool iInvokeCallback)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbCallback<T>(iParamID, std::move(iChangeCallback), iInvokeCallback);
  else
    return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxAware::unregisterParam(GUIVstParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxAware::unregisterParam(GUIJmbParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIJmbParam<T>{};
}

}
}
}
}