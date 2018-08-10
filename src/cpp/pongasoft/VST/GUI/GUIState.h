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

#include <pongasoft/VST/Parameters.h>
#include "pongasoft/VST/MessageHandler.h"
#include <pongasoft/VST/GUI/Params/VstParameters.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include <pongasoft/VST/GUI/Params/GUISerParameter.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

namespace Params {
class GUIParamCxMgr;
}

class GUIState
{
public:
  explicit GUIState(Parameters const &iPluginParameters) :
    fPluginParameters{iPluginParameters}
  {};

  /**
   * Called by the GUIController. */
  virtual tresult init(VstParametersSPtr iVstParameters);

  // getPluginParameters
  Parameters const &getPluginParameters() const { return fPluginParameters; }

  /**
   * This method is called for each parameter managed by the GUIState that is not a regular VST parameter
   */
  template<typename T>
  GUISerParam<T> add(SerParam<T> iParamDef);

  /**
   * @return true if there is a vst param with the provided ID
   */
  inline bool existsVst(ParamID iParamID) const { return fVstParameters->exists(iParamID); }

  /**
   * @return true if there is a ser param with the provided ID
   */
  inline bool existsSer(ParamID iParamID) const { return fSerParams.find(iParamID) != fSerParams.cend(); }

  /**
   * @return the raw parameter given its id
   */
  std::unique_ptr<GUIRawVstParameter> getRawVstParameter(ParamID iParamID) const
  {
    if(existsVst(iParamID))
      return std::make_unique<GUIRawVstParameter>(iParamID, fVstParameters);
    else
      return nullptr;
  }

  // getRawVstParamDef
  std::shared_ptr<RawVstParamDef> getRawVstParamDef(ParamID iParamID) const
  {
    return fPluginParameters.getRawVstParamDef(iParamID);
  }

  /**
   * @return the ser parameter given its id (nullptr if not found)
   */
  IGUISerParameter *getSerParameter(ParamID iParamID) const;

  /**
   * This method is called from the GUI controller setComponentState method and reads the state coming from RT
   * and initializes the vst host parameters accordingly
   */
  virtual tresult readRTState(IBStreamer &iStreamer);

  /**
   * This method is called from the GUI controller setState method and reads the state previously saved by the
   * GUI only (parameters that are ui only) and initializes the vst host parameters accordingly
   */
  virtual tresult readGUIState(IBStreamer &iStreamer);

  /**
   * This method is called from the GUI controller getState method and writes the state specific to the
   * GUI only (parameters that are ui only), reading the values from the vst host parameters
   */
  virtual tresult writeGUIState(IBStreamer &oStreamer) const;

  /**
   * The CustomView class automatically calls this method to get a handle of a ParamCxMgr used to register for interest
   * and obtain GUIParam instances. See CustomView::registerXXX methods.
   */
  std::unique_ptr<GUIParamCxMgr> createParamCxMgr();

  /**
   * Handle an incoming message => will forward to SerParam marked enabledForMessaging
   */
  tresult handleMessage(Message const &iMessage) { return fMessageHandler.handleMessage(iMessage); }

protected:
  // the parameters
  Parameters const &fPluginParameters;

  // setParamNormalized
  tresult setParamNormalized(NormalizedState const *iNormalizedState);

protected:
  VstParametersSPtr fVstParameters{};

  // handles messages for
  MessageHandler fMessageHandler;

  // contains all the (serializable) registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::unique_ptr<IGUISerParameter>> fSerParams{};

  // add serializable parameter to the structures
  void addSerParam(std::unique_ptr<IGUISerParameter> iParameter);
};

/**
 * Simple templated extension to expose the plugin parameters as its real type
 *
 * @tparam TPluginParameters the type of the plugin parameters (subclass of Parameters)
 */
template<typename TPluginParameters>
class GUIPluginState : public GUIState
{
public:
  using PluginParameters = TPluginParameters;

public:
  explicit GUIPluginState(PluginParameters const &iPluginParameters) :
    GUIState{iPluginParameters},
    fParams{iPluginParameters}
  { }

public:
  PluginParameters const &fParams;
};

//------------------------------------------------------------------------
// GUIState::add
//------------------------------------------------------------------------
template<typename T>
GUISerParam<T> GUIState::add(SerParam<T> iParamDef)
{
  auto rawPtr = new GUISerParameter<T>(iParamDef);
  std::unique_ptr<IGUISerParameter> guiParam{rawPtr};
  addSerParam(std::move(guiParam));
  if(iParamDef->fShared)
  {
    if(iParamDef->fOwner == ISerParamDef::Owner::kRT)
    {
      if(iParamDef->fSerializer)
        fMessageHandler.registerHandler(iParamDef->fParamID, rawPtr);
    }
    else
    {
      DLOG_F(WARNING, "NOT implemented: Currently GUI owned parameter cannot be shared");
    }
  }
  return rawPtr;
}

}
}
}

