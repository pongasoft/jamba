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
#include <vector>

namespace pongasoft::VST::GUI::Params {

/**
 * Maintains the connections established between parameters and its listeners/callbacks. All the connections are
 * properly closed when this class is destroyed.
 *
 * @internal
 */
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
   * Unregisters all parameters */
  void unregisterAll();

  //------------------------------------------------------------------------
  // Base Param
  //------------------------------------------------------------------------

  // registerBaseParam / ParamID / IChangeListener
  IGUIParam registerBaseParam(ParamID iParamID, Parameters::IChangeListener *iChangeListener = nullptr);

  // registerBaseCallback / ParamID / ChangeCallback
  inline IGUIParam registerBaseCallback(ParamID iParamID,
                                        Parameters::ChangeCallback iChangeCallback,
                                        bool iInvokeCallback)
  {
    return __registerCallback(registerBaseParam(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerBaseCallback / ParamID / ChangeCallback1<IGUIParam>
  inline IGUIParam registerBaseCallback(ParamID iParamID,
                                        Parameters::ChangeCallback1<IGUIParam> iChangeCallback,
                                        bool iInvokeCallback)
  {
    return __registerCallback1(registerBaseParam(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Optional Param
  //------------------------------------------------------------------------

  // registerOptionalParam / ParamID / IChangeListener
  template<typename T>
  GUIOptionalParam<T> registerOptionalParam(ParamID iParamID,
                                            Parameters::IChangeListener *iChangeListener = nullptr);

  // registerOptionalCallback / ParamID / ChangeCallback
  template<typename T>
  inline GUIOptionalParam<T> registerOptionalCallback(ParamID iParamID,
                                                      Parameters::ChangeCallback iChangeCallback,
                                                      bool iInvokeCallback) {
    return __registerCallback(registerOptionalParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerOptionalCallback / ParamID / ChangeCallback1<GUIOptionalParam<T>>
  template<typename T>
  inline GUIOptionalParam<T> registerOptionalCallback(ParamID iParamID,
                                                      Parameters::ChangeCallback1<GUIOptionalParam<T>> iChangeCallback,
                                                      bool iInvokeCallback) {
    return __registerCallback1(registerOptionalParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Optional Discrete Param
  //------------------------------------------------------------------------

  // registerOptionalDiscreteParam / ParamID / IChangeListener
  GUIOptionalParam<int32> registerOptionalDiscreteParam(ParamID iParamID,
                                                        int32 iStepCount,
                                                        Parameters::IChangeListener *iChangeListener = nullptr);

  // registerOptionalDiscreteCallback / ParamID / ChangeCallback
  inline GUIOptionalParam<int32> registerOptionalDiscreteCallback(ParamID iParamID,
                                                                  int32 iStepCount,
                                                                  Parameters::ChangeCallback iChangeCallback,
                                                                  bool iInvokeCallback) {
    return __registerCallback(registerOptionalDiscreteParam(iParamID, iStepCount), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerOptionalDiscreteCallback / ParamID / ChangeCallback1<GUIOptionalParam<int32>>
  inline GUIOptionalParam<int32> registerOptionalDiscreteCallback(ParamID iParamID,
                                                                  int32 iStepCount,
                                                                  Parameters::ChangeCallback1<GUIOptionalParam<int32>> iChangeCallback,
                                                                  bool iInvokeCallback) {
    return __registerCallback1(registerOptionalDiscreteParam(iParamID, iStepCount), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Raw Vst Param
  //------------------------------------------------------------------------

   // registerRawVstParam / ParamID / IChangeListener
  GUIRawVstParam registerRawVstParam(ParamID iParamID,
                                     Parameters::IChangeListener *iChangeListener = nullptr);

  // registerRawVstCallback / ParamID / ChangeCallback
  inline GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                               Parameters::ChangeCallback iChangeCallback,
                                               bool iInvokeCallback)
  {
    return __registerCallback(registerRawVstParam(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerRawVstCallback / ParamID / ChangeCallback1<GUIRawVstParam>
  inline GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                               Parameters::ChangeCallback1<GUIRawVstParam> iChangeCallback,
                                               bool iInvokeCallback)
  {
    return __registerCallback1(registerRawVstParam(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Vst Param (Typed)
  //------------------------------------------------------------------------

  // registerVstParam / ParamID / IChangeListener
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  // registerVstParam / ParamID / ChangeCallback
  template<typename T>
  inline GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                            Parameters::ChangeCallback iChangeCallback,
                                            bool iInvokeCallback)
  {
    return __registerCallback(registerVstParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerVstParam / ParamID / ChangeCallback1<GUIVstParam<T>>
  template<typename T>
  inline GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                            Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                            bool iInvokeCallback)
  {
    return __registerCallback1(registerVstParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Jmb Param (Typed)
  //------------------------------------------------------------------------

  // registerJmbParam / ParamID / IChangeListener
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  // registerJmbParam / ParamID / ChangeCallback
  template<typename T>
  inline GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                            Parameters::ChangeCallback iChangeCallback,
                                            bool iInvokeCallback)
  {
    return __registerCallback(registerJmbParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // registerJmbParam / ParamID / ChangeCallback1<GUIJmbParam<T>>
  template<typename T>
  inline GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                            Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                                            bool iInvokeCallback)
  {
    return __registerCallback1(registerJmbParam<T>(iParamID), std::move(iChangeCallback), iInvokeCallback);
  }

  // getGUIState
  inline GUIState *getGUIState() const { return fGUIState; };

  /**
   * Invoke all registered callbacks and listeners */
  void invokeAll();

  friend class GUI::GUIState;

protected:
  explicit GUIParamCxMgr(GUIState *iGUIState) : fGUIState{iGUIState}
  {
    DCHECK_F(fGUIState != nullptr);
  }

  template<typename TParam>
  inline TParam __registerListener(TParam iParam, Parameters::IChangeListener *iChangeListener)
  {
    if(iParam.exists() && iChangeListener)
      fParamCxs.emplace_back(iParam.connect(iChangeListener));
    return iParam;
  }

  template<typename TParam>
  TParam __registerCallback(TParam iParam, Parameters::ChangeCallback iCallback, bool iInvokeCallback);

  template<typename TParam>
  TParam __registerCallback1(TParam iParam, Parameters::ChangeCallback1<TParam> iCallback, bool iInvokeCallback);

private:
  // the gui state
  GUIState *fGUIState;

  // Maintains the connections for the listeners and callbacks... will be automatically discarded in the destructor
  std::vector<std::unique_ptr<FObjectCx>> fParamCxs{};
};

}

// Include template implementation code
#include "GUIParamCxMgr.hpp"
