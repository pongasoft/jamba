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

#include <pongasoft/VST/GUI/GUIState.h>
#include <unordered_map>

namespace pongasoft::VST::GUI::Params {

class GUIParamCxMgr
{
public:
  /**
   * @return true if the param actually exists
   */
  inline bool existsVst(ParamID iParamID) const { return fGUIState->existsVst(iParamID); }

  /**
   * @return true if the param actually exists
   */
  inline bool existsJmb(ParamID iParamID) const { return fGUIState->existsJmb(iParamID); }

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return true if the param was present, false otherwise
   */
  bool unregisterParam(TagID iParamID);

  /**
   * Unregisters all parameters */
  void unregisterAll();

  template<typename T>
  bool registerOptionalParam(TagID iParamID,
                             GUIOptionalParam<T> &oParam,
                             Parameters::IChangeListener *iChangeListener = nullptr);

  bool registerOptionalDiscreteParam(TagID iParamID,
                                     GUIOptionalParam<int32> &oParam,
                                     int32 iStepCount,
                                     Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Registers a raw parameter (no conversion)
   */
  inline GUIRawVstParam registerRawVstParam(ParamID iParamID,
                                            Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return __registerRawVstParam(iParamID, iChangeListener);
  }

  /**
   * Registers a raw parameter (no conversion)
   */
  inline GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                               Parameters::ChangeCallback iChangeCallback,
                                               bool iInvokeCallback)
  {
    return maybeInvokeCallback(__registerRawVstParam(iParamID, iChangeCallback),
                               iChangeCallback,
                               iInvokeCallback);
  }

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerVstParam(ParamID iParamID,
                                         Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return __registerVstParam<T>(iParamID, iChangeListener);
  }

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                            Parameters::ChangeCallback iChangeCallback,
                                            bool iInvokeCallback)
  {
    return maybeInvokeCallback(__registerVstParam<T>(iParamID, iChangeCallback),
                               iChangeCallback,
                               iInvokeCallback);
  }

  /**
   * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerVstParam(VstParam<T> const &iParamDef,
                                         Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return __registerVstParam(iParamDef, iChangeListener);
  }

  /**
   * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerVstCallback(VstParam<T> const &iParamDef,
                                            Parameters::ChangeCallback iChangeCallback,
                                            bool iInvokeCallback)
  {
    return maybeInvokeCallback(__registerVstParam(iParamDef, iChangeCallback),
                               iChangeCallback,
                               iInvokeCallback);
  }

  /**
   * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
   *
   * @return false if not found or not proper type
   */
  template<typename T>
  bool registerVstCallback(VstParam<T> iParamDef,
                           Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                           bool iInvokeCallback);

  /**
   * This method registers the listener to be notified of the GUIJmbParam changes. Note that GUIJmbParam is already
   * a wrapper directly accessible from the view and as a result there is no need to call this method unless a
   * listener is provided, hence the listener is required.
   *
   * @return a copy of iParamDef for convenience and symmetry of the APIs
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(GUIJmbParam<T> &iParamDef, Parameters::IChangeListener *iChangeListener)
  {
    DCHECK_F(iChangeListener != nullptr);
    fParamCxs[iParamDef.getParamID()] = std::move(iParamDef.connect(iChangeListener));
    return iParamDef;
  }

  /**
   * This method registers a callback to be invoked on GUIJmbParam changes.
   *
   * @return a copy of iParamDef for convenience and symmetry of the APIs
   */
  template<typename T>
  GUIJmbParam<T> registerJmbCallback(GUIJmbParam<T> &iParamDef,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback)
  {
    DCHECK_F((bool) iChangeCallback);
    fParamCxs[iParamDef.getParamID()] = std::move(iParamDef.connect(iChangeCallback));
    if(iInvokeCallback)
      iChangeCallback();
    return iParamDef;
  }

  /**
   * This method registers a callback to be invoked on GUIJmbParam changes.
   */
  template<typename T>
  void registerJmbCallback(GUIJmbParam<T> &iParamDef,
                           Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                           bool iInvokeCallback)
  {
    DCHECK_F((bool) iChangeCallback);
    auto callback = [&iParamDef, cb2 = std::move(iChangeCallback)] () { cb2(iParamDef); };
    registerJmbCallback(iParamDef, callback, iInvokeCallback);
  }


  /**
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return __registerJmbParam<T>(iParamID, iChangeListener);
  }

  /**
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback)
  {
    return maybeInvokeCallback(__registerJmbParam<T>(iParamID, iChangeCallback),
                               iChangeCallback,
                               iInvokeCallback);
  }

  // getGUIState
  GUIState *getGUIState() const { return fGUIState; };

  /**
   * Invoke all registered callbacks and listeners */
  void invokeAll();

  friend class GUI::GUIState;

protected:
  explicit GUIParamCxMgr(GUIState *iGUIState) : fGUIState{iGUIState}
  {
    DCHECK_F(fGUIState != nullptr);
  }

  /**
   * Registers a raw parameter (no conversion) */
  template<typename Listener>
  std::unique_ptr<GUIRawVstParameter> __registerRawVstParameter(ParamID iParamID, Listener iListener);

  // __registerRawVstParam
  template<typename Listener>
  GUIRawVstParam __registerRawVstParam(ParamID iParamID, Listener iListener);

  // __registerVstParam
  template<typename T, typename Listener>
  GUIVstParam<T> __registerVstParam(ParamID iParamID, Listener iListener);

  // __registerVstParam
  template<typename T, typename Listener>
  GUIVstParam<T> __registerVstParam(VstParam<T> const &iParamDef, Listener iListener);

  // __registerJmbParam
  template<typename T, typename Listener>
  GUIJmbParam<T> __registerJmbParam(ParamID iParamID, Listener iListener);

  // maybeInvokeCallback
  template<typename TParam>
  inline TParam &&maybeInvokeCallback(TParam &&iParam, Parameters::ChangeCallback &iCallback, bool iInvokeCallback)
  {
    if(iInvokeCallback && iParam.exists() && iCallback)
     iCallback();
    return std::forward<TParam>(iParam);
  }

private:
  // the gui state
  GUIState *fGUIState;

  // Maintains the connections for the listeners... will be automatically discarded in the destructor
  std::unordered_map<ParamID, std::unique_ptr<FObjectCx>> fParamCxs{};
};

//------------------------------------------------------------------------
// GUIParamCxMgr::registerOptionalParam
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxMgr::registerOptionalParam(TagID iParamID,
                                          GUIOptionalParam<T> &oParam,
                                          Parameters::IChangeListener *iChangeListener)
{
  auto previousTagID = oParam.getTagID();

  if(previousTagID != iParamID)
    unregisterParam(previousTagID);

  bool paramChanged = false;

  auto param = fGUIState->findParam(iParamID);

  if(param)
  {
    auto typedParam = param->cast<T>();
    if(typedParam)
    {
      oParam.assign(std::move(typedParam));
      paramChanged = true;
    }
    else
    {
      DLOG_F(WARNING, "param [%d] is not of the requested type [%s]", iParamID, typeid(T).name());
    }
  }

  // no vst or jmb parameter match => using default
  if(!paramChanged)
  {
    oParam.clearAssignment(iParamID);

    if(iParamID == UNDEFINED_PARAM_ID)
      paramChanged = true;

#ifndef NDEBUG
    if(iParamID != UNDEFINED_PARAM_ID)
      DLOG_F(WARNING, "could not find any parameter (vst or jmb) with id [%d]... reverting to default", iParamID);
#endif
  }

  if(iChangeListener)
  {
    fParamCxs[iParamID] = oParam.connect(iChangeListener);
  }
  else
  {
    unregisterParam(iParamID);
  }

  return paramChanged;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerRawVstParameter
//------------------------------------------------------------------------
template<typename Listener>
std::unique_ptr<GUIRawVstParameter> GUIParamCxMgr::__registerRawVstParameter(ParamID iParamID, Listener iListener)
{
  auto param = fGUIState->getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  if(iListener)
  {
    fParamCxs[iParamID] = param->connect(iListener);
  }
  else
  {
    unregisterParam(iParamID);
  }

  return param;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerRawVstParam
//------------------------------------------------------------------------
template<typename Listener>
GUIRawVstParam GUIParamCxMgr::__registerRawVstParam(ParamID iParamID, Listener iListener)
{
  auto param = __registerRawVstParameter(iParamID, iListener);

  if(!param)
  {
    return GUIRawVstParam{};
  }

  return GUIRawVstParam{std::move(param)};
}


//------------------------------------------------------------------------
// GUIParamCxMgr::__registerVstParam
//------------------------------------------------------------------------
template<typename T, typename Listener>
GUIVstParam<T> GUIParamCxMgr::__registerVstParam(VstParam<T> const &iParamDef, Listener iListener)
{
  auto param = __registerRawVstParameter(iParamDef->fParamID, iListener);

  if(param)
    return GUIVstParam<T>{std::make_unique<GUIVstParameter<T>>(std::move(param), iParamDef->fConverter)};
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerVstParam
//------------------------------------------------------------------------
template<typename T, typename Listener>
GUIVstParam<T> GUIParamCxMgr::__registerVstParam(ParamID iParamID, Listener iListener)
{
  if(iListener)
  {
    auto param = fGUIState->getGUIVstParameter<T>(iParamID);

    if(param)
    {
      fParamCxs[iParamID] = param->connect(iListener);
      return GUIVstParam<T>{std::move(param)};
    }
  }

  unregisterParam(iParamID);
  return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerJmbParam
//------------------------------------------------------------------------
template<typename T, typename Listener>
GUIJmbParam<T> GUIParamCxMgr::__registerJmbParam(ParamID iParamID, Listener iListener)
{
  auto param = fGUIState->getJmbParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "jmb param [%d] not found", iParamID);
    return GUIJmbParam<T>{};
  }

  auto res = std::dynamic_pointer_cast<GUIJmbParameter<T>>(param);
  if(res)
  {
    if(iListener)
      fParamCxs[iParamID] = std::move(res->connect(iListener));
    else
      unregisterParam(iParamID);
  }
  else
  {
    DLOG_F(WARNING, "jmb param [%d] is not of the requested type [%s]", iParamID, typeid(T).name());
    unregisterParam(iParamID);
  }

  return res;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstCallback
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxMgr::registerVstCallback(VstParam<T> iParamDef,
                                        Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                        bool iInvokeCallback)
{
  auto paramID = iParamDef->fParamID;

  if(iChangeCallback)
  {
    auto param = fGUIState->getGUIVstParameter<T>(paramID);

    if(param)
    {
      // Implementation note:
      // 1) using std::make_unique results in an error on std::move(callback) because unique_ptr
      // cannot be copied...
      // 2) we need to access the ptr afterwards to call connect
      auto ptr = std::make_shared<GUIVstParam<T>>(std::move(param));

      auto callback = [ptr, cb1 = std::move(iChangeCallback)] () {
        cb1(*ptr);
      };

      fParamCxs[paramID] = ptr->connect(callback);

      if(iInvokeCallback)
        callback();

      return true;
    }
  }

  unregisterParam(paramID);
  return false;
}

}
