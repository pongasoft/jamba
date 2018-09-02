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
   * Registers a raw parameter (no conversion)
   */
  GUIRawVstParam registerRawVstParam(ParamID iParamID,
                                     Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(VstParam<T> iParamDef,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

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
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUIJmbParam<T> registerJmbParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  // getGUIState
  GUIState *getGUIState() const { return fGUIState; };

  friend class GUI::GUIState;

protected:
  explicit GUIParamCxMgr(GUIState *iGUIState) : fGUIState{iGUIState}
  {
    DCHECK_F(fGUIState != nullptr);
  }

  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawVstParameter> registerRawVstParameter(ParamID iParamID,
                                                              Parameters::IChangeListener *iChangeListener = nullptr);


private:
  // the gui state
  GUIState *fGUIState;

  // Maintains the connections for the listeners... will be automatically discarded in the destructor
  std::map<ParamID, std::unique_ptr<GUIParamCx>> fParamCxs;
};


//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxMgr::registerVstParam(VstParam<T> iParamDef,
                                               Parameters::IChangeListener *iChangeListener)
{
  auto param = registerRawVstParameter(iParamDef->fParamID, iChangeListener);

  if(param)
    return GUIVstParam<T>{std::make_unique<GUIVstParameter<T>>(std::move(param), iParamDef)};
  else
    return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxMgr::registerVstParam(ParamID iParamID,
                                               Parameters::IChangeListener *iChangeListener)
{
  auto param = registerRawVstParameter(iParamID, iChangeListener);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return GUIVstParam<T>{};
  }

  auto rawParamDef = fGUIState->getRawVstParamDef(iParamID);

  auto paramDef = std::dynamic_pointer_cast<VstParamDef<T>>(rawParamDef);

  if(paramDef)
  {
    if(iChangeListener)
      fParamCxs[iParamID] = std::move(param->connect(iChangeListener));
    else
      unregisterParam(iParamID);

    return GUIVstParam<T>{std::make_unique<GUIVstParameter<T>>(std::move(param), paramDef)};
  }

  DLOG_F(WARNING, "vst param [%d] is not of the requested type", iParamID);
  unregisterParam(iParamID);
  return GUIVstParam<T>{};
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerJmbParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxMgr::registerJmbParam(ParamID iParamID,
                                               Parameters::IChangeListener *iChangeListener)
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
    if(iChangeListener)
      fParamCxs[iParamID] = std::move(res->connect(iChangeListener));
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

}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
