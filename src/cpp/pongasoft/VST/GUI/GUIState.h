/*
 * Copyright (c) 2018-2020 pongasoft
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
#include <pongasoft/VST/GUI/Params/IGUIParameter.hpp>
#include <pongasoft/VST/GUI/Params/GUIJmbParameter.h>
#include <pongasoft/VST/MessageProducer.h>
#include "ParamAwareViews.h"

namespace pongasoft::VST {

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
   * @return true if there is a jmb param with the provided ID
   */
  inline bool existsJmb(ParamID iParamID) const { return fJmbParams.find(iParamID) != fJmbParams.cend()
                                                         ||
                                                         fPluginParameters.getJmbParamDef(iParamID) != nullptr; }

  /**
   * Generic call which returns a param with the given id or `nullptr` if there isn't one. It can be either
   * a Vst or Jmb param.
   */
  std::shared_ptr<IGUIParameter> findParam(ParamID iParamID) const;

  /**
   * @return the raw parameter given its id
   */
  std::shared_ptr<GUIRawVstParameter> getRawVstParameter(ParamID iParamID) const
  {
    if(existsVst(iParamID))
      return std::make_shared<GUIRawVstParameter>(iParamID,
                                                  fVstParameters,
                                                  fPluginParameters.getRawVstParamDef(iParamID));
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
  std::shared_ptr<GUIVstParameter<T>> getGUIVstParameter(ParamID iParamID) const;

  // getGUIVstParameter
  template<typename T>
  inline std::shared_ptr<GUIVstParameter<T>> getGUIVstParameter(VstParam<T> iParamDef) const {
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
    auto ptr = findParam(iParamID);
    if(ptr)
      return ptr->connect(iChangeListener);
    else
      return nullptr;
  }

  /**
   * Connects the paramID to the callback. The connection object returned automatically closes the connection
   * when it gets destroyed
   *
   * @return nullptr if the parameter does not exist
   */
  std::unique_ptr<FObjectCx> connect(ParamID iParamID, Parameters::ChangeCallback iChangeCallback) const
  {
    auto ptr = findParam(iParamID);
    if(ptr)
      return ptr->connect(iChangeCallback);
    else
      return nullptr;
  }

  /**
   * Allow for registering an arbitrary callback on an arbitrary view without having to inherit from the view.
   * The registration will automatically be discarded when the view is deleted.
   *
   * Example usage:
   *
   *     TextButtonView *button = ....;
   *     fState->makeParamAware(button)->registerCallback<int>(fParams->fMyParam,
   *       [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
   *       iButton->setMouseEnabled(iParam > 3);
   *     });
   *
   * @tparam TView should be a subclass of VSTGUI::CView
   * @return a pointer to an object for registering callbacks, listener and params.
   *         Note: You should not keep this pointer around as it be will automatically be deleted when the view
   *         goes away.
   */
  template<typename TView>
  inline ParamAwareView<TView> *makeParamAware(TView *iView) {
    return fParamAwareViews.makeParamAware(iView, this);
  }

  /**
   * @deprecated Use makeParamAware instead */
  template<typename TView>
  [[deprecated("Since 4.0.0 - Use makeParamAware instead")]]
  inline ParamAwareView<TView> *registerConnectionFor(TView *iView) {
    return makeParamAware<TView>(iView, this);
  }

  /**
   * @return the Jmb parameter given its id (nullptr if not found)
   */
  std::shared_ptr<IGUIJmbParameter> getJmbParameter(ParamID iParamID) const;

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

  /**
   * When Jamba detects that a previously saved GUI state matches a deprecated version (as registered with
   * `setGUIDeprecatedSaveStateOrder`), it will call this method to let the plugin handle the upgrade if necessary.
   *
   * \note When this method is called, the parameters from the deprecated stream have been read and properly
   *       set so this method is only used for handling conversion cases.
   *
   * @param iDeprecatedVersion the deprecated version
   * @param iVersion the current version
   * @return `kResultTrue` if handled, `kResultFalse` if unhandled
   */
  virtual tresult handleGUIStateUpgrade(int16 iDeprecatedVersion, int16 iVersion) const { return kResultTrue; }

  // gives access for debug
  friend class Debug::ParamDisplay;

protected:
  // the parameters
  Parameters const &fPluginParameters;

  // raw vst parameters
  VstParametersSPtr fVstParameters{};

  // param aware views
  ParamAwareViews fParamAwareViews{};

  // message producer (to send messages)
  IMessageProducer *fMessageProducer{};

  // handles messages (receive messages)
  MessageHandler fMessageHandler{};

  // contains all the (serializable) registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<IGUIJmbParameter>> fJmbParams{};

  // order in which the parameters were registered
  std::vector<ParamID> fAllRegistrationOrder{};

protected:
  // setParamNormalized
  tresult setParamNormalized(NormalizedState const *iNormalizedState);

  // add serializable parameter to the structures
  void addJmbParam(std::shared_ptr<IGUIJmbParameter> iParameter);

  // allocateMessage
  IPtr<IMessage> allocateMessage() override;

  // sendMessage
  tresult sendMessage(IPtr<IMessage> iMessage) override;

  virtual tresult readDeprecatedGUIState(uint16 iDeprecatedVersion,
                                         IBStreamer &iStreamer,
                                         NormalizedState::SaveOrder const &iLatestSaveOrder);

  //! Reads the gui state from the stream using the provided order
  virtual tresult readGUIState(NormalizedState::SaveOrder const &iSaveOrder, IBStreamer &iStreamer);
};

/**
 * Simple templated extension to expose the plugin parameters as its real type
 *
 * @tparam TPluginParameters the type of the plugin parameters (subclass of Parameters)
 */
template<typename TPluginParameters>
class GUIPluginState : public GUIState
{
  // ensures that TPluginParameters is a subclass of Parameters
  static_assert(std::is_convertible<TPluginParameters*, Parameters*>::value, "TPluginParameters must be a subclass of Parameters");

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
  auto guiParam = iParamDef->newGUIParam();
  addJmbParam(guiParam);
  return GUIJmbParam<T>(std::dynamic_pointer_cast<GUIJmbParameter<T>>(guiParam));
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
std::shared_ptr<GUIVstParameter<T>> GUIState::getGUIVstParameter(ParamID iParamID) const
{
  std::shared_ptr<GUIRawVstParameter> param = getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  auto res = param->asVstParameter<T>();

  if(res)
    return res;
  else
  {
    DLOG_F(WARNING, "vst param [%d] is not of the requested type", iParamID);
    return nullptr;
  }
}


}
}

//------------------------------------------------------------------------
// Implementation note: because of the circular dependency between
// JmbParamDef and IGUIJmbParameter, this templated method is defined
// here in GUIState.h since GUIState.cpp is the primary user of this
// method.
//------------------------------------------------------------------------
namespace pongasoft::VST {
//------------------------------------------------------------------------
// JmbParamDef<T>::newGUIParam
//------------------------------------------------------------------------
template<typename T>
std::shared_ptr<GUI::Params::IGUIJmbParameter> JmbParamDef<T>::newGUIParam()
{
  auto ptr = std::dynamic_pointer_cast<JmbParamDef<T>>(IParamDef::shared_from_this());
  return VstUtils::make_sfo<GUI::Params::GUIJmbParameter<T>>(ptr);
}
}

