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

#include <pongasoft/VST/Parameters.h>
#include "GUIRawVstParameter.h"
#include "GUIJmbParameter.h"
#include "GUIVstParameter.h"
#include "GUIOptionalParam.h"

namespace pongasoft {
namespace VST {
namespace GUI {

class GUIState;

namespace Params {

class GUIParamCxMgr;

/**
 * Encapsulates classes that want to be aware of parameters and be notified when they change
 */
class GUIParamCxAware : public Parameters::IChangeListener
{
public:
  virtual ~GUIParamCxAware();

  bool registerRawOptionalParam(TagID iParamID,
                                GUIRawOptionalParam &oParam,
                                bool iSubscribeToChanges = true);

  template<typename T>
  bool registerOptionalParam(TagID iParamID,
                             GUIOptionalParam<T> &oParam,
                             bool iSubscribeToChanges = true);


  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a raw parameter (no conversion). This version will properly unregister a previously registered
   * param.
   *
   * @param iParamID `UNDEFINED_PARAM_ID` when there is no parameter to use
   * @param oGUIRawVstParam the reference to the backed parameter
   * @return true if a new parameter was registered, false otherwise
   */
  bool registerParam(TagID iParamID, GUIRawVstParam &oGUIRawVstParam);

  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerRawVstCallback(ParamID iParamID,
                                        Parameters::ChangeCallback iChangeCallback,
                                        bool iInvokeCallback = false);

  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerParam(RawVstParam const &iParamDef, bool iSubscribeToChanges = true) {
    return registerRawVstParam(iParamDef->fParamID, iSubscribeToChanges);
  }

  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerCallback(RawVstParam const &iParamDef,
                                  Parameters::ChangeCallback iChangeCallback,
                                  bool iInvokeCallback = false) {
    return registerRawVstCallback(iParamDef->fParamID, std::move(iChangeCallback), iInvokeCallback);
  }

  /**
   * Convenient method to register a parameter which is backed by a value when the parameter does not exist.
   *
   * IMPORTANT: this api should be considered internal as this is not the right concept... It will be removed!
   *
   * @param iParamID -1 when there is no parameter to use
   * @param oControlValue the reference to the value
   * @param oGUIRawVstParam the reference to the backed parameter
   * @return true if a new parameter was registered, false otherwise
   */
  bool __internal__registerRawVstControl(TagID iParamID, ParamValue &oControlValue, GUIRawVstParam &oGUIRawVstParam);

  // shortcut for BooleanParameter
  GUIVstParam<bool> registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIVstParam<Percent> registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a Vst parameter simply given its id
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a callback for a Vst parameter simply given its id
   * @return empty param if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstCallback(ParamID iParamID,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback = false);

  /**
   * Convenient call to register a Vst param simply by using its description. Takes care of the type due to method API
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges = true);

  /**
   * Convenient call to register a callback for the Vst param simply by using its description. Takes care of the
   * type due to method API
   *
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerCallback(VstParam<T> const &iParamDef,
                                  Parameters::ChangeCallback iChangeCallback,
                                  bool iInvokeCallback = false);

  /**
   * Convenient call to register a callback for the Vst param simply by using its description. Takes care of
   * the type due to method API. Since the callback (Parameters::ChangeCallback1) is providing the param, it is not
   * returned by this api.
   *
   * Example:
   *
   *     registerCallback<int>(fParams->fMyParam, [] (GUIVstParam<int> &iParam) { iParam.getValue()...; });
   *
   * Note how the compiler requires the type (which seems that it could be inferred).
   *
   * @return `false` if not found or not proper type
   */
  template<typename T>
  bool registerCallback(VstParam<T> const &iParamDef,
                        Parameters::ChangeCallback1<GUIVstParam<T>> iChangeCallback,
                        bool iInvokeCallback = false);

  /**
   * This method registers this class to be notified of the GUIJmbParam changes. Note that GUIJmbParam is already
   * a wrapper directly accessible from the view and as a result there is no need to call this method unless you
   * want to subscribe to the changes, hence there isn't a second optional parameter
   *
   * @return a copy of `iParamDef` for convenience and symmetry of the APIs or empty wrapper if GUIParamCxAware::initState has not
   *         been called
   */
  template<typename T>
  GUIJmbParam<T> registerParam(GUIJmbParam<T> &iParamDef);

  /**
   * This method registers the callback to be invoked on GUIJmbParam changes.
   *
   * @return a copy of `iParamDef` for convenience and symmetry of the APIs or empty wrapper if GUIParamCxAware::initState has not
   *         been called
   */
  template<typename T>
  GUIJmbParam<T> registerCallback(GUIJmbParam<T> &iParamDef,
                                  Parameters::ChangeCallback iChangeCallback,
                                  bool iInvokeCallback = false);

  /**
   * This method registers the callback to be invoked on GUIJmbParam changes. Since the callback
   * (Parameters::ChangeCallback1) is providing the param, it is not returned by this api.
   *
   * Example:
   *
   *     registerCallback<Range>(fState->fMyParam, [] (GUIJmbParam<Range> &iParam) { iParam.getValue()...; });
   *
   * @return `false` if GUIParamCxAware::initState has not been called
   */
  template<typename T>
  bool registerCallback(GUIJmbParam<T> &iParamDef,
                        Parameters::ChangeCallback1<GUIJmbParam<T>> iChangeCallback,
                        bool iInvokeCallback = false);

  /**
   * Registers the Jmb param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use GUIJmbParam::exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a callback for this Jmb param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use GUIJmbParam::exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbCallback(ParamID iParamID,
                                     Parameters::ChangeCallback iChangeCallback,
                                     bool iInvokeCallback = false);

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
class ViewGUIParamCxAware : public GUIParamCxAware
{
public:
  // required because we implement other registerCallback methods
  using GUIParamCxAware::registerCallback;

  // defining the change listener callback
  using ChangeListener = std::function<void(TView *, ParamID)>;

  // Constructor
  explicit ViewGUIParamCxAware(TView *iView) : fView(iView), fListener{} {}

  /**
   * Since this class is intended to be used without inheriting from GUIParamCxAware, you can register
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

    return GUIParamCxAware::registerCallback<T>(iParamDef, std::move(callback), iInvokeCallback);
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

    return GUIParamCxAware::registerCallback<T>(iParam, std::move(callback), iInvokeCallback);
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
}
}