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

#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Encapsulates classes that want to be aware of parameters and be notified when they change
 */
class GUIParamCxAware : public Parameters::IChangeListener
{
public:
  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerRawVstParam(ParamID iParamID, Parameters::ChangeCallback iChangeCallback);

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
  bool __internal__registerRawVstControl(int32_t iParamID, ParamValue &oControlValue, GUIRawVstParam &oGUIRawVstParam);

  // shortcut for BooleanParameter
  GUIVstParam<bool> registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIVstParam<Percent> registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a vst parameter simply given its id
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID, bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
    else
      return GUIVstParam<T>{};
  }

  /**
   * Register a vst parameter simply given its id
   * @return empty param if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID, Parameters::ChangeCallback iChangeCallback)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam<T>(iParamID, std::move(iChangeCallback));
    else
      return GUIVstParam<T>{};
  }

  /**
   * Convenient method to register a parameter which is backed by a value when the parameter does not exist.
   *
   * IMPORTANT: this api should be considered internal as this is not the right concept... It will be removed!
   *
   * @param iParamID -1 when there is no parameter to use
   * @param oControlValue the reference to the value
   * @param oGUIVstParam the reference to the backed parameter
   * @return true if a new parameter was registered, false otherwise
   */
  template<typename T>
  bool __internal__registerVstControl(int32_t iParamID, T &oControlValue, GUIVstParam<T> &oGUIVstParam);

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam(iParamDef, iSubscribeToChanges ? this : nullptr);
    else
      return GUIVstParam<T>{};
  }

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return empty param  if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(VstParam<T> const &iParamDef, Parameters::ChangeCallback iChangeCallback)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam(iParamDef, std::move(iChangeCallback));
    else
      return GUIVstParam<T>{};
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
  GUIJmbParam<T> registerJmbParam(GUIJmbParam<T> &iParamDef)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerJmbParam(iParamDef, this);
    else
      return GUIJmbParam<T>{};
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
  GUIJmbParam<T> registerJmbParam(GUIJmbParam<T> &iParamDef, Parameters::ChangeCallback iChangeCallback)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerJmbParam(iParamDef, std::move(iChangeCallback));
    else
      return GUIJmbParam<T>{};
  }

  /**
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID,
                                  bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerJmbParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
    else
      return GUIJmbParam<T>{};
  }

  /**
 * Registers the ser param only given its id and return the wrapper to the param.
 *
 * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
 */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID, Parameters::ChangeCallback iChangeCallback)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerJmbParam<T>(iParamID, std::move(iChangeCallback));
    else
      return GUIJmbParam<T>{};
  }

  /**
   * Called during initialization
   */
  virtual void initState(GUIState *iGUIState)
  {
    fParamCxMgr = iGUIState->createParamCxMgr();
  }

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
   * @return true if the param was present, false otherwise
   */
  bool unregisterParam(ParamID iParamID);

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return an empty param so that you can write param = unregisterParam(param)
   */
  GUIRawVstParam unregisterParam(GUIRawVstParam const &iParam);

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return an empty param so that you can write param = unregisterParam(param)
   */
  template<typename T>
  GUIVstParam<T> unregisterParam(GUIVstParam<T> const &iParam)
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
  GUIJmbParam<T> unregisterParam(GUIJmbParam<T> const &iParam)
  {
    if(iParam.exists())
      fParamCxMgr->unregisterParam(iParam.getParamID());
    return GUIJmbParam<T>{};
  }

  /**
   * Callback when a parameter changes. Empty default implementation
   */
  void onParameterChange(ParamID iParamID) override {}

protected:
  // Access to parameters
  std::unique_ptr<GUIParamCxMgr> fParamCxMgr;
};

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