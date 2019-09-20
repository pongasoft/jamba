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

#include <vstgui/lib/cview.h>

#include <pongasoft/VST/Parameters.h>
#include "GUIRawVstParameter.h"
#include "GUIJmbParameter.h"
#include "GUIVstParameter.h"
#include "GUIOptionalParam.h"

namespace pongasoft::VST::GUI {

class GUIState;

namespace Params {

class GUIParamCxMgr;

/**
 * Encapsulates classes that want to be aware of parameters and be notified when they change
 */
class ParamAware : public Parameters::IChangeListener
{
public:
  virtual ~ParamAware();

  //------------------------------------------------------------------------
  // Base Param
  //------------------------------------------------------------------------

  /**
   * Registers the "base" param for the most generic use case but as a result is fairly limited and mainly gives access
   * to the string representation of the param.
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   *
   * @return empty param if not found
   */
  IGUIParam registerBaseParam(TagID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a callback for the "base" param for the most generic use case. As a result it is fairly limited and
   * mainly gives access to the string representation of the param.
   *
   * @return empty param if not found
   */
  IGUIParam registerBaseCallback(TagID iParamID,
                                 Parameters::ChangeCallback iChangeCallback,
                                 bool iInvokeCallback);

  /**
   * Registers a callback for the "base" param for the most generic use case. As a result it is fairly limited and
   * mainly gives access to the string representation of the param.
   *
   * @return empty param if not found
   */
  IGUIParam registerBaseCallback(TagID iParamID,
                                 Parameters::ChangeCallback1<IGUIParam> iChangeCallback,
                                 bool iInvokeCallback);

  //------------------------------------------------------------------------
  // Optional Param
  //------------------------------------------------------------------------

  /**
   * Registers an optional parameter which handles Vst, Jmb or no parameter at all. This parameter is particularly
   * useful to write very generic views that can accept "any" kind of parameter (Vst or Jmb) while still being
   * usable if no parameter is assigned at all.
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   * Note that this parameter has a very different signature since it is designed to be more dynamic (changed in the
   * UI editor) and the previous value gets propagated if the
   *
   * @return param (never empty!)
   */
  template<typename T>
  GUIOptionalParam<T> registerOptionalParam(TagID iParamID,
                                            bool iSubscribeToChanges = true);

  /**
   * Registers an optional parameter which handles Vst, Jmb or no parameter at all. This parameter is particularly
   * useful to write very generic views that can accept "any" kind of parameter (Vst or Jmb) while still being
   * usable if no parameter is assigned at all.
   * The callback will be invoked when the parameter changes.
   * Note that this parameter has a very different signature since it is designed to be more dynamic (changed in the
   * UI editor) and the previous value gets propagated if the
   *
   * @return param (never empty!)
   */
  template<typename T>
  GUIOptionalParam<T> registerOptionalCallback(TagID iParamID,
                                               Parameters::ChangeCallback iChangeCallback,
                                               bool iInvokeCallback = false);

  /**
   * Registers an optional parameter which handles Vst, Jmb or no parameter at all. This parameter is particularly
   * useful to write very generic views that can accept "any" kind of parameter (Vst or Jmb) while still being
   * usable if no parameter is assigned at all.
   * The callback will be invoked when the parameter changes.
   * Note that this parameter has a very different signature since it is designed to be more dynamic (changed in the
   * UI editor) and the previous value gets propagated if the
   *
   * @return param (never empty!)
   */
  template<typename T>
  GUIOptionalParam<T> registerOptionalCallback(TagID iParamID,
                                               Parameters::ChangeCallback1<GUIOptionalParam<T>> iChangeCallback,
                                               bool iInvokeCallback = false);

  //------------------------------------------------------------------------
  // Optional Discrete Param
  //------------------------------------------------------------------------

  /**
   * Registers an optional discrete parameter which handles Vst, Jmb or no parameter at all. The underlying parameter
   * will be converted to a discrete parameter if possible.
   * This parameter is particularly useful to write very generic views that can accept "any" kind of parameter
   * (Vst or Jmb) while still being usable if no parameter is assigned at all.
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   *
   * @return param (never empty!)
   */
  GUIOptionalParam<int32> registerOptionalDiscreteParam(TagID iParamID,
                                                        int32 iStepCount,
                                                        bool iSubscribeToChanges = true);

  /**
   * Registers an optional discrete parameter which handles Vst, Jmb or no parameter at all. The underlying parameter
   * will be converted to a discrete parameter if possible.
   * This parameter is particularly useful to write very generic views that can accept "any" kind of parameter
   * (Vst or Jmb) while still being usable if no parameter is assigned at all.
   * The callback will be invoked when the parameter changes.
   *
   * @return param (never empty!)
   */
  GUIOptionalParam<int32> registerOptionalDiscreteCallback(TagID iParamID,
                                                           int32 iStepCount,
                                                           Parameters::ChangeCallback iChangeCallback,
                                                           bool iInvokeCallback = false);

  /**
   * Registers an optional discrete parameter which handles Vst, Jmb or no parameter at all. The underlying parameter
   * will be converted to a discrete parameter if possible.
   * This parameter is particularly useful to write very generic views that can accept "any" kind of parameter
   * (Vst or Jmb) while still being usable if no parameter is assigned at all.
   * The callback will be invoked when the parameter changes.
   *
   * @return param (never empty!)
   */
  GUIOptionalParam<int32> registerOptionalDiscreteCallback(TagID iParamID,
                                                           int32 iStepCount,
                                                           Parameters::ChangeCallback1<GUIOptionalParam<int32>> iChangeCallback,
                                                           bool iInvokeCallback = false);

  //------------------------------------------------------------------------
  // Raw Vst Param / ParamID
  //------------------------------------------------------------------------

  /**
   * Registers a raw parameter (no conversion)
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   *
   * @return empty param if not found
   */
  GUIRawVstParam registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a raw parameter (no conversion)
   * The callback will be invoked when the parameter changes.
   *
   * @return empty param if not found
   */
  GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                        Parameters::ChangeCallback iChangeCallback,
                                        bool iInvokeCallback = false);

  /**
   * Registers a raw parameter (no conversion)
   * The callback will be invoked when the parameter changes.
   *
   * @return empty param if not found
   */
  GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                        Parameters::ChangeCallback1<GUIRawVstParam> iChangeCallback,
                                        bool iInvokeCallback = false);


  //------------------------------------------------------------------------
  // Raw Vst Param / param definition (RawVstParam)
  //------------------------------------------------------------------------

  /**
   * Registers a raw parameter (no conversion). Convenient call using the param def
   * (accessible in param-aware views via `fParams->fMyParam`).
   */
  inline GUIRawVstParam registerParam(RawVstParam const &iParamDef, bool iSubscribeToChanges = true) {
    return registerRawVstParam(iParamDef->fParamID, iSubscribeToChanges);
  }

  /**
   * Registers a raw parameter (no conversion). Convenient call using the param def
   * (accessible in param-aware views via `fParams->fMyParam`).
   */
  inline GUIRawVstParam registerCallback(RawVstParam const &iParamDef,
                                         Parameters::ChangeCallback iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerRawVstCallback(iParamDef->fParamID, std::move(iChangeCallback), iInvokeCallback);
  }

  /**
   * Registers a raw parameter (no conversion). Convenient call using the param def
   * (accessible in param-aware views via `fParams->fMyParam`).
   */
  inline GUIRawVstParam registerCallback(RawVstParam const &iParamDef,
                                         Parameters::ChangeCallback1<GUIRawVstParam> iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerRawVstCallback(iParamDef->fParamID, std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Shortcut Vst Param bool/Percent
  //------------------------------------------------------------------------

  // shortcut for BooleanParameter (will be removed => not very useful)
  [[deprecated("deprecated since 4.0.0 => use registerVstParam<bool> directly")]]
  GUIVstParam<bool> registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter (will be removed => not very useful)
  [[deprecated("deprecated since 4.0.0 => use registerVstParam<Percent> directly")]]
  GUIVstParam<Percent> registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  //------------------------------------------------------------------------
  // Vst Param (Typed) / ParamID
  //------------------------------------------------------------------------

  /**
   * Register a Vst parameter simply given its id
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a callback for a Vst parameter simply given its id
   * The callback will be invoked when the parameter changes.
   *
   * @return empty param if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback = false);

  /**
   * Register a callback for a Vst parameter simply given its id
   * The callback will be invoked when the parameter changes.
   *
   * @return empty param if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                     Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                     bool iInvokeCallback = false);

  //------------------------------------------------------------------------
  // Vst Param (Typed) / param definition (VstParam<T>)
  //------------------------------------------------------------------------

  /**
   * Convenient call to register a Vst param simply by using its description (accessible in param-aware views
   * via `fParams->fMyParam`). Takes care of the type due to method API.
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges = true) {
    return registerVstParam<T>(iParamDef.getParamID(), iSubscribeToChanges);
  }

  /**
   * Convenient call to register a callback for the Vst param simply by using its description (accessible in
   * param-aware views via `fParams->fMyParam`). Takes care of the type due to method API
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerCallback(VstParam<T> const &iParamDef,
                                         Parameters::ChangeCallback iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerVstCallback<T>(iParamDef.getParamID(), std::move(iChangeCallback), iInvokeCallback);
  }

  /**
   * Convenient call to register a callback for the Vst param simply by using its description (accessible in
   * param-aware views via `fParams->fMyParam`). Takes care of the type due to method API.
   *
   * Example:
   *
   * ```
   * registerCallback<int>(fParams->fMyParam, [] (GUIVstParam<int> &iParam) { iParam.getValue()...; });
   * ```
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIVstParam<T> registerCallback(VstParam<T> const &iParamDef,
                                         Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerVstCallback<T>(iParamDef.getParamID(), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Jmb Param (Typed) / ParamID
  //------------------------------------------------------------------------

  /**
   * Registers the Jmb param only given its id and return the wrapper to the param.
   * `onParameterChanges` will be called on changes (if `iSubscribeToChanges` is set to `true`).
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use GUIJmbParam::exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a callback for this Jmb param only given its id and return the wrapper to the param.
   * The callback will be invoked when the parameter changes.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use GUIJmbParam::exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback = false);


  /**
   * Registers a callback for this Jmb param only given its id and return the wrapper to the param.
   * The callback will be invoked when the parameter changes.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use GUIJmbParam::exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                     Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                                     bool iInvokeCallback = false);

  //------------------------------------------------------------------------
  // Jmb Param (Typed) / Param definition (JmbParam<T>)
  //------------------------------------------------------------------------

  /**
   * Convenient call to register a Jmb param simply by using its description (accessible in param-aware views
   * via `fParams->fMyParam`). Takes care of the type due to method API.
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIJmbParam<T> registerParam(JmbParam<T> const &iParamDef, bool iSubscribeToChanges = true) {
    return registerJmbParam<T>(iParamDef.getParamID(), iSubscribeToChanges);
  }

  /**
   * Convenient call to register a callback for the Jmb param simply by using its description (accessible in
   * param-aware views via `fParams->fMyParam`). Takes care of the type due to method API
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIJmbParam<T> registerCallback(JmbParam<T> const &iParamDef,
                                         Parameters::ChangeCallback iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerJmbCallback<T>(iParamDef.getParamID(), std::move(iChangeCallback), iInvokeCallback);

  }

  /**
   * Convenient call to register a callback for the Jmb param simply by using its description (accessible in
   * param-aware views via `fParams->fMyParam`). Takes care of the type due to method API.
   *
   * Example:
   *
   * ```
   * registerCallback<int>(fParams->fMyParam, [] (GUIJmbParam<int> &iParam) { iParam.getValue()...; });
   * ```
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIJmbParam<T> registerCallback(JmbParam<T> const &iParamDef,
                                         Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerJmbCallback<T>(iParamDef.getParamID(), std::move(iChangeCallback), iInvokeCallback);
  }

  //------------------------------------------------------------------------
  // Jmb Param (Typed) / Param (GUIJmbParam<T>)
  //------------------------------------------------------------------------

  /**
   * Convenient call to register a Jmb param simply by using the param from the state (accessible in state-aware views
   * via `fState->fMyParam`). Takes care of the type due to method API.
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIJmbParam<T> registerParam(GUIJmbParam<T> &iParam, bool iSubscribeToChanges = true) {
    return registerJmbParam<T>(iParam.getParamID(), true);
  }

  /**
   * Convenient call to register a callback for the Jmb param simply by using the param from the state
   * (accessible in state-aware views via `fState->fMyParam`). Takes care of the type due to method API.
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline GUIJmbParam<T> registerCallback(GUIJmbParam<T> &iParam,
                                         Parameters::ChangeCallback iChangeCallback,
                                         bool iInvokeCallback = false) {
    return registerJmbCallback<T>(iParam.getParamID(), std::move(iChangeCallback), iInvokeCallback);
  }

  /**
   * Convenient call to register a callback for the Jmb param simply by using the param from the state
   * (accessible in state-aware views via `fState->fMyParam`). Takes care of the type due to method API.
   *
   * Example:
   *
   * ```
   * registerCallback<int>(fParams->fMyParam, [] (GUIJmbParam<int> &iParam) { iParam.getValue()...; });
   * ```
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  inline bool registerCallback(GUIJmbParam<T> &iParam,
                               Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                               bool iInvokeCallback = false) {
    return registerJmbCallback<T>(iParam.getParamID(), std::move(iChangeCallback), iInvokeCallback);
  }

  /**
   * Called during initialization
   */
  virtual void initState(GUIState *iGUIState);

  /**
   * Subclasses should override this method to register each parameter
   */
  virtual void registerParameters()
  {
    // subclasses implements this method
  }

  /**
   * Unregisters all parameters */
  void unregisterAll();

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return `true` if the param was present, `false` otherwise
   */
  bool unregisterParam(ParamID iParamID);

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return an empty param so that you can write `param = unregisterParam(param)`
   */
  GUIRawVstParam unregisterParam(GUIRawVstParam const &iParam);

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return an empty param so that you can write `param = unregisterParam(param)`
   */
  template<typename T>
  GUIVstParam<T> unregisterParam(GUIVstParam<T> const &iParam);

  /**
 * Removes the registration of the provided param (closing the connection/stopping to listen)
 *
 * @return an empty param so that you can write `param = unregisterParam(param)`
 */
  template<typename T>
  GUIJmbParam<T> unregisterParam(GUIJmbParam<T> const &iParam);

  /**
   * Callback when a parameter changes. Empty default implementation
   */
  void onParameterChange(ParamID iParamID) override {}

  /**
   * Invoke all registered callbacks and param listener */
  void invokeAll();

protected:
  // Access to parameters
  std::unique_ptr<GUIParamCxMgr> fParamCxMgr{};
};

/**
 * This subclass allows for registering callbacks to any kind of view without having to inherit from it
 * (see GUIState::registerConnectionFor)
 *
 * @tparam TView should be a subclass of VSTGUI::CView
 */
template<typename TView>
class ParamAwareView : public ParamAware
{
  // ensures that TView is a subclass of CView
  static_assert(std::is_convertible<TView *, VSTGUI::CView*>::value, "TView must be a subclass of CView");

public:
  // required because we implement other registerCallback methods
  using ParamAware::registerCallback;

  // defining the change listener callback
  using ChangeListener = std::function<void(TView *, ParamID)>;

  // Constructor
  explicit ParamAwareView(TView *iView) : fView(iView), fListener{} {}

  /**
   * Since this class is intended to be used without inheriting from ParamAware, you can register
   * a listener so that the onParameterChange method will be invoking this listener.
   */
  void registerListener(ChangeListener iListener)
  {
    fListener = std::move(iListener);
  }

  /**
   * Register a callback for the Vst parameter. This version is specific to this class and allow for
   * the view to be passed back (since the view is not inheriting from this class).
   *
   * Example:
   *
   *     registerCallback<bool>(fParams->fMyParam, [] (CTextLabel *iView, GUIVstParam<bool> &iParam) { iParam.getValue()...; });
   *
   * @return `false` if not found or not proper type
   */
  template<typename T>
  inline bool registerCallback(VstParam<T> const &iParamDef,
                               Parameters::ChangeCallback2<TView, GUIVstParam<T>> iChangeCallback,
                               bool iInvokeCallback = false)
  {
    auto callback = [view = this->fView, cb2 = std::move(iChangeCallback)] (GUIVstParam<T> &iParam) {
      cb2(view, iParam);
    };

    return ParamAware::registerCallback<T>(iParamDef, std::move(callback), iInvokeCallback);
  }

  /**
   * Register a callback for the Jmb parameter. This version is specific to this class and allow for
   * the view to be passed back (since the view is not inheriting from this class).
   *
   * Example:
   *
   *     registerCallback<Range>(fState->fMyParam, [] (CTextLabel *iView, GUIJmbParam<Range> &iParam) { iParam.getValue()...; });
   *
   * @return `false` if not found or not proper type
   */
  template<typename T>
  inline bool registerCallback(GUIJmbParam<T> &iParam,
                               Parameters::ChangeCallback2<TView, GUIJmbParam<T>> iChangeCallback,
                               bool iInvokeCallback = false)
  {
    auto callback = [view = this->fView, cb2 = std::move(iChangeCallback)] (GUIJmbParam<T> &iParam) {
      cb2(view, iParam);
    };

    return ParamAware::registerCallback<T>(iParam, std::move(callback), iInvokeCallback);
  }

  // onParameterChange => delegate to listener
  void onParameterChange(ParamID iParamID) override
  {
    if(fListener)
      fListener(fView, iParamID);
  }

protected:
  TView *fView;
  ChangeListener fListener;
};

}
}
