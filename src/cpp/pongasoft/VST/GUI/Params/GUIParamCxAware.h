/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

  // shortcut for BooleanParameter
  GUIVstParam<bool> registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIVstParam<Percent> registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
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
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
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
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return true if the param was present, false otherwise
   */
  bool unregisterParam(ParamID iParamID) { return fParamCxMgr->unregisterParam(iParamID); }

  /**
   * Callback when a parameter changes. Empty default implementation
   */
  void onParameterChange(ParamID iParamID) override {}

protected:
  // Access to parameters
  std::unique_ptr<GUIParamCxMgr> fParamCxMgr;
};


}
}
}
}