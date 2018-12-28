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

#include <pongasoft/VST/Parameters.h>
#include "pongasoft/VST/MessageHandler.h"
#include <pongasoft/VST/GUI/Params/VstParameters.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include <pongasoft/VST/GUI/Params/GUIJmbParameter.h>
#include <pongasoft/VST/MessageProducer.h>
#include "ViewCxMgr.h"

namespace pongasoft {
namespace VST {
namespace Debug { class ParamDisplay; }
namespace GUI {

using namespace Params;

namespace Params {
class GUIParamCxMgr;
}

class GUIState : public IMessageProducer
{
public:
  // Constructor
  explicit GUIState(Parameters const &iPluginParameters);

  /**
   * Called by the GUIController. */
  virtual tresult init(VstParametersSPtr iVstParameters, IMessageProducer *iMessageProducer);

  // getPluginParameters
  Parameters const &getPluginParameters() const { return fPluginParameters; }

  /**
   * This method is called for each parameter managed by the GUIState that is not a regular VST parameter
   */
  template<typename T>
  GUIJmbParam<T> add(JmbParam<T> iParamDef);

  /**
   * @return true if there is a vst param with the provided ID
   */
  inline bool existsVst(ParamID iParamID) const { return fVstParameters && fVstParameters->exists(iParamID); }

  /**
   * @return true if there is a ser param with the provided ID
   */
  inline bool existsJmb(ParamID iParamID) const { return fJmbParams.find(iParamID) != fJmbParams.cend(); }

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

  // getGUIVstParameter
  template<typename T>
  std::unique_ptr<GUIVstParameter<T>> getGUIVstParameter(ParamID iParamID) const;

  // getGUIVstParameter
  template<typename T>
  inline std::unique_ptr<GUIVstParameter<T>> getGUIVstParameter(VstParam<T> iParamDef) const {
    return iParamDef ? getGUIVstParameter<T>(iParamDef->fParamID) : nullptr;
  }

  /**
   * Connects the paramID to the listener. The connection object returned automatically closes the connection
   * when it gets destroyed
   *
   * @return nullptr if the parameter does not exist
   */
  std::unique_ptr<FObjectCx> connect(ParamID iParamID, Parameters::IChangeListener *iChangeListener) const
  {
    return fVstParameters ? fVstParameters->connect(iParamID, iChangeListener) : nullptr;
  }

  /**
   * Connects the paramID to the callback. The connection object returned automatically closes the connection
   * when it gets destroyed
   *
   * @return nullptr if the parameter does not exist
   */
  std::unique_ptr<FObjectCx> connect(ParamID iParamID, Parameters::ChangeCallback iChangeCallback) const
  {
    return fVstParameters ? fVstParameters->connect(iParamID, std::move(iChangeCallback)) : nullptr;
  }

  /**
   * Allow for registering an arbitrary callback on an arbitrary view without having to inherit from the view.
   * The registration will automatically be discarded when the view is deleted.
   *
   * Example usage:
   *
   * TextButtonView *button = ....;
   * fState->registerConnectionFor(button)->registerCallback<int>(fParams->fMyParam,
   *   [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
   *   iButton->setMouseEnabled(iParam > 3);
   * });
   *
   * @param TView should be a subclass of VSTGUI::CView
   * @return a pointer to an object for registering callbacks, listener and params.
   *         Note: You should not keep this pointer around as it be will automatically be deleted when the view
   *         goes away.
   */
  template<typename TView>
  inline ViewGUIParamCxAware<TView> *registerConnectionFor(TView *iView) {
    return fViewCxMgr.registerConnectionFor(iView, this);
  }

  /**
   * @return the Jmb parameter given its id (nullptr if not found)
   */
  IGUIJmbParameter *getJmbParameter(ParamID iParamID) const;

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
   * Handle an incoming message => will forward to JmbParam marked shared by rtOwner
   */
  tresult handleMessage(Message const &iMessage) { return fMessageHandler.handleMessage(iMessage); }

  /**
   * Broadcast a message without requiring the need to instantiate a GUIJmbParam
   */
  template<typename T>
  tresult broadcast(JmbParam<T> const &iParamDef, T const &iMessage);

  // getAllRegistrationOrder
  std::vector<ParamID> const &getAllRegistrationOrder() const { return fAllRegistrationOrder; }

  // gives access for debug
  friend class Debug::ParamDisplay;

protected:
  // the parameters
  Parameters const &fPluginParameters;

  // raw vst parameters
  VstParametersSPtr fVstParameters{};

  // view connection mgr
  ViewCxMgr fViewCxMgr{};

  // message producer (to send messages)
  IMessageProducer *fMessageProducer{};

  // handles messages (receive messages)
  MessageHandler fMessageHandler{};

  // contains all the (serializable) registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::unique_ptr<IGUIJmbParameter>> fJmbParams{};

  // order in which the parameters were registered
  std::vector<ParamID> fAllRegistrationOrder{};

protected:
  // setParamNormalized
  tresult setParamNormalized(NormalizedState const *iNormalizedState);

  // add serializable parameter to the structures
  void addJmbParam(std::unique_ptr<IGUIJmbParameter> iParameter);

  // allocateMessage
  IPtr<IMessage> allocateMessage() override;

  // sendMessage
  tresult sendMessage(IPtr<IMessage> iMessage) override;
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
GUIJmbParam<T> GUIState::add(JmbParam<T> iParamDef)
{
  auto rawPtr = new GUIJmbParameter<T>(iParamDef);
  std::unique_ptr<IGUIJmbParameter> guiParam{rawPtr};
  addJmbParam(std::move(guiParam));
  if(iParamDef->fShared && iParamDef->fSerializer)
  {
    switch(iParamDef->fOwner)
    {
      case IJmbParamDef::Owner::kRT:
        fMessageHandler.registerHandler(iParamDef->fParamID, rawPtr);
        break;

      case IJmbParamDef::Owner::kGUI:
        rawPtr->setMessageProducer(this);
        break;

      default:
        // not reached
        DLOG_F(ERROR, "not reached");
        break;
    }
  }
  return rawPtr;
}

//------------------------------------------------------------------------
// GUIState::broadcast
//------------------------------------------------------------------------
template<typename T>
tresult GUIState::broadcast(JmbParam<T> const &iParamDef, const T &iMessage)
{
  if(!iParamDef->fShared)
  {
    DLOG_F(WARNING, "broadcast ignored: parameter [%d] is not marked shared", iParamDef->fParamID);
    return kResultFalse;
  }

  tresult res = kResultOk;

  auto message = allocateMessage();

  if(message)
  {
    Message m{message.get()};

    // sets the message ID
    m.setMessageID(iParamDef->fParamID);

    // serialize the content
    if(iParamDef->writeToMessage(iMessage, m) == kResultOk)
      res |= sendMessage(message);
    else
      res = kResultFalse;
  }
  else
    res = kResultFalse;

  return res;
}

//------------------------------------------------------------------------
// GUIState::getGUIVstParameter
//------------------------------------------------------------------------
template<typename T>
std::unique_ptr<GUIVstParameter<T>> GUIState::getGUIVstParameter(ParamID iParamID) const
{
  auto param = getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  auto rawParamDef = getRawVstParamDef(iParamID);

  auto paramDef = std::dynamic_pointer_cast<VstParamDef<T>>(rawParamDef);

  if(paramDef)
  {
    return std::make_unique<GUIVstParameter<T>>(std::move(param), paramDef);
  }
  else
  {
    DLOG_F(WARNING, "vst param [%d] is not of the requested type", iParamID);
    return nullptr;
  }
}


}
}
}

