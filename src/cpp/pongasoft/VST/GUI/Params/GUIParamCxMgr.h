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
#ifndef __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
#define __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__

#include <pongasoft/VST/GUI/GUIState.h>
#include <unordered_map>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

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
  bool unregisterParam(ParamID iParamID);

  /**
   * Unregisters all parameters */
  void unregisterAll();

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
                                               Parameters::ChangeCallback iChangeCallback)
  {
    return __registerRawVstParam(iParamID, std::move(iChangeCallback));
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
                                            Parameters::ChangeCallback iChangeCallback)
  {
    return __registerVstParam<T>(iParamID, std::move(iChangeCallback));
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
                                            Parameters::ChangeCallback iChangeCallback)
  {
    return __registerVstParam(iParamDef, std::move(iChangeCallback));
  }

  /**
   * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
   *
   * @return false if not found or not proper type
   */
  template<typename T>
  bool registerVstCallback(VstParam<T> iParamDef,
                           Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback);

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
  GUIJmbParam<T> registerJmbCallback(GUIJmbParam<T> &iParamDef, Parameters::ChangeCallback iChangeCallback)
  {
    DCHECK_F((bool) iChangeCallback);
    fParamCxs[iParamDef.getParamID()] = std::move(iParamDef.connect(std::move(iChangeCallback)));
    return iParamDef;
  }

  /**
   * This method registers a callback to be invoked on GUIJmbParam changes.
   */
  template<typename T>
  void registerJmbCallback(GUIJmbParam<T> &iParamDef, Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback)
  {
    DCHECK_F((bool) iChangeCallback);
    auto callback = [&iParamDef, cb2 = std::move(iChangeCallback)] () { cb2(iParamDef); };
    registerJmbCallback(iParamDef, callback);
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
  GUIJmbParam<T> registerJmbCallback(ParamID iParamID, Parameters::ChangeCallback iChangeCallback)
  {
    return __registerJmbParam<T>(iParamID, std::move(iChangeCallback));
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

private:
  // the gui state
  GUIState *fGUIState;

  // Maintains the connections for the listeners... will be automatically discarded in the destructor
  std::unordered_map<ParamID, std::unique_ptr<FObjectCx>> fParamCxs;
};

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
    return GUIVstParam<T>{std::make_unique<GUIVstParameter<T>>(std::move(param), iParamDef)};
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
    return nullptr;
  }

  auto res = dynamic_cast<GUIJmbParameter<T> *>(param);
  if(res)
  {
    if(iListener)
      fParamCxs[iParamID] = std::move(res->connect(iListener));
    else
      unregisterParam(iParamID);
  }
  else
  {
    DLOG_F(WARNING, "jmb param [%d] is not of the requested type", iParamID);
    unregisterParam(iParamID);
  }

  return res;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstCallback
//------------------------------------------------------------------------
template<typename T>
bool GUIParamCxMgr::registerVstCallback(VstParam<T> iParamDef,
                                        Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback)
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

      return true;
    }
  }

  unregisterParam(paramID);
  return false;
}

}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
