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
#pragma once

#include "GUIParamCxAware.h"
#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {


/**
 * Register a Vst parameter simply given its id
 * @return empty param  if not found or not proper type
 */
template<typename T>
GUIVstParam<T> GUIParamCxAware::registerVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIVstParam<T>{};
}

/**
 * Register a callback for a Vst parameter simply given its id
 * @return empty param if not found or not proper type
 */
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

/**
 * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
 * @return empty param  if not found or not proper type
 */
template<typename T>
GUIVstParam<T> GUIParamCxAware::registerParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerVstParam(iParamDef, iSubscribeToChanges ? this : nullptr);
  else
    return GUIVstParam<T>{};
}

/**
 * Convenient call to register a callback for the Vst param simply by using its description. Takes care of the
 * type due to method API
 * @return empty param  if not found or not proper type
 */
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

/**
 * Convenient call to register a callback for the Vst param simply by using its description. Takes care of
 * the type due to method API. Since the callback (Parameters::ChangeCallback1) is providing the param, it is not
 * returned by this api.
 *
 * Example:
 * registerCallback<int>(fParams->fMyParam, [] (GUIVstParam<int> &iParam) { iParam.getValue()...; });
 *
 * Note how the compiler requires the type (which seems that it could be inferred).
 *
 * @return false if not found or not proper type
 */
template<typename T>
bool GUIParamCxAware::registerCallback(VstParam<T> const &iParamDef,
                                       Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                       bool iInvokeCallback)
{
  return fParamCxMgr ? fParamCxMgr->registerVstCallback(iParamDef, std::move(iChangeCallback), iInvokeCallback) : false;
}

/**
 * This method registers this class to be notified of the GUIJmbParam changes. Note that GUIJmbParam is already
 * a wrapper directly accessible from the view and as a result there is no need to call this method unless you
 * want to subscribe to the changes, hence there isn't a second optional parameter
 *
 * @return a copy of iParamDef for convenience and symmetry of the APIs or empty wrapper if initState has not
 *         been called
 */
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerParam(GUIJmbParam<T> &iParamDef)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbParam(iParamDef, this);
  else
    return GUIJmbParam<T>{};
}

/**
 * This method registers the callback to be invoked on GUIJmbParam changes.
 *
 * @return a copy of iParamDef for convenience and symmetry of the APIs or empty wrapper if initState has not
 *         been called
 */
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

/**
 * This method registers the callback to be invoked on GUIJmbParam changes. Since the callback
 * (Parameters::ChangeCallback1) is providing the param, it is not returned by this api.
 *
 * Example:
 * registerCallback<Range>(fState->fMyParam, [] (GUIJmbParam<Range> &iParam) { iParam.getValue()...; });
 *
 * @return false if initState has not been called
 */
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

/**
 * Registers the Jmb param only given its id and return the wrapper to the param.
 *
 * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
 */
template<typename T>
GUIJmbParam<T> GUIParamCxAware::registerJmbParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(fParamCxMgr)
    return fParamCxMgr->registerJmbParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
  else
    return GUIJmbParam<T>{};
}

/**
 * Registers a callback for this Jmb param only given its id and return the wrapper to the param.
 *
 * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
 */
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

/**
 * Removes the registration of the provided param (closing the connection/stopping to listen)
 *
 * @return an empty param so that you can write param = unregisterParam(param)
 */
template<typename T>
GUIVstParam<T> GUIParamCxAware::unregisterParam(GUIVstParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIVstParam<T>{};
}

/**
* Removes the registration of the provided param (closing the connection/stopping to listen)
*
* @return an empty param so that you can write param = unregisterParam(param)
*/
template<typename T>
GUIJmbParam<T> GUIParamCxAware::unregisterParam(GUIJmbParam<T> const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIJmbParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxAware::__internal__registerVstControl
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxAware::__internal__registerVstControl(int32_t iParamID, T &oControlValue, GUIVstParam<T> &oGUIVstParam)
{
  if(!fParamCxMgr)
    return false; // not set yet

  if(iParamID < 0)
  {
    if(oGUIVstParam.exists())
    {
      oControlValue = oGUIVstParam.getValue();
      oGUIVstParam = unregisterParam(oGUIVstParam);
    }
  }
  else
  {
    if(!oGUIVstParam.exists() || oGUIVstParam.getParamID() != iParamID)
    {
      oGUIVstParam = registerVstParam<T>(static_cast<ParamID>(iParamID));
      return true;
    }
  }

  return false;
}


}
}
}
}