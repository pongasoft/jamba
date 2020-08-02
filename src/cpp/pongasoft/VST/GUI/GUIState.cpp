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
#include <sstream>
#include "GUIState.h"
#include <pongasoft/VST/GUI/Params/GUIParamCxMgr.h>

namespace pongasoft::VST::GUI {

//------------------------------------------------------------------------
// GUIState::GUIState
//------------------------------------------------------------------------
GUIState::GUIState(Parameters const &iPluginParameters) :
  fPluginParameters{iPluginParameters}
{
}

//------------------------------------------------------------------------
// GUIState::findParam
//------------------------------------------------------------------------
std::shared_ptr<IGUIParameter> GUIState::findParam(ParamID iParamID) const
{
  auto rawPtr = getRawVstParameter(iParamID);

  if(rawPtr)
    return rawPtr;

  return std::dynamic_pointer_cast<IGUIParameter>(getJmbParameter(iParamID));
}

//------------------------------------------------------------------------
// GUIState::addJmbParam
//------------------------------------------------------------------------
void GUIState::addJmbParam(std::shared_ptr<IGUIJmbParameter> iParameter)
{
  DCHECK_F(iParameter != nullptr);

  ParamID paramID = iParameter->getJmbParamID();

  DCHECK_F(fPluginParameters.getJmbParamDef(paramID) != nullptr, "jmb parameter [%d] not registered", paramID);
  DCHECK_F(fJmbParams.find(paramID) == fJmbParams.cend(), "duplicate paramID [%d]", paramID);

  auto rawPtr = iParameter.get();
  auto paramDef = iParameter->getParamDef();
  if(paramDef->fShared && paramDef->isSerializable())
  {
    switch(paramDef->fOwner)
    {
      case IJmbParamDef::Owner::kRT:
        fMessageHandler.registerHandler(paramID, rawPtr);
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

  fJmbParams[paramID] = std::move(iParameter);
  fAllRegistrationOrder.emplace_back(paramID);
}

//------------------------------------------------------------------------
// GUIState::setParamNormalized
//------------------------------------------------------------------------
tresult GUIState::setParamNormalized(NormalizedState const *iNormalizedState)
{
  tresult res = kResultOk;

  for(int i = 0; i < iNormalizedState->getCount(); i++)
  {
    auto paramID = iNormalizedState->fSaveOrder->fOrder[i];
    res |= fVstParameters->setParamNormalized(paramID, iNormalizedState->fValues[i]);
  }

  return res;
}

//------------------------------------------------------------------------
// GUIState::readRTState
//------------------------------------------------------------------------
tresult GUIState::readRTState(IBStreamer &iStreamer)
{
  auto normalizedState = fPluginParameters.newRTState();

  tresult res = fPluginParameters.readRTState(iStreamer, normalizedState.get());

  if(res == kResultOk)
  {
    return setParamNormalized(normalizedState.get());
  }

  return res;
}

//------------------------------------------------------------------------
// GUIState::readGUIState
//------------------------------------------------------------------------
tresult GUIState::readGUIState(IBStreamer &iStreamer)
{
  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();

  // nothing to read ?
  if(saveOrder.getCount() == 0)
    return kResultOk;

  if(saveOrder.fVersion >= 0)
  {
    uint16 stateVersion;
    if(!iStreamer.readInt16u(stateVersion))
      stateVersion = 0;

    if(stateVersion != saveOrder.fVersion)
      return readDeprecatedGUIState(stateVersion, iStreamer, saveOrder);
  }

  return readGUIState(saveOrder, iStreamer);
}

//------------------------------------------------------------------------
// GUIState::readDeprecatedGUIState
//------------------------------------------------------------------------
tresult GUIState::readDeprecatedGUIState(uint16 iDeprecatedVersion,
                                         IBStreamer &iStreamer,
                                         NormalizedState::SaveOrder const &iLatestSaveOrder)
{
  auto deprecatedSaveOrder = fPluginParameters.getGUIDeprecatedSaveStateOrder(iDeprecatedVersion);
  if(deprecatedSaveOrder)
  {
    auto res = readGUIState(*deprecatedSaveOrder, iStreamer);

    if(res == kResultOk)
      res = handleGUIStateUpgrade(iDeprecatedVersion, iLatestSaveOrder.fVersion);
    return res;
  }
  else
  {
    DLOG_F(WARNING, "unexpected GUI state version %d", iDeprecatedVersion);
    return readGUIState(iLatestSaveOrder, iStreamer);
  }
}

//------------------------------------------------------------------------
// GUIState::readGUIState
//------------------------------------------------------------------------
tresult GUIState::readGUIState(NormalizedState::SaveOrder const &iSaveOrder, IBStreamer &iStreamer)
{
  // nothing to read ?
  if(iSaveOrder.getCount() == 0)
    return kResultOk;

  tresult res = kResultOk;

  for(int i = 0; i < iSaveOrder.getCount(); i++)
  {
    auto paramID = iSaveOrder.fOrder[i];
    auto iter = fJmbParams.find(paramID);
    if(iter == fJmbParams.cend())
    {
      // not jmb => vst
      if(fPluginParameters.getRawVstParamDef(paramID))
      {
        ParamValue value = fPluginParameters.readNormalizedValue(paramID, iStreamer);
        fVstParameters->setParamNormalized(paramID, value);
      }
      else
      {
        DLOG_F(ERROR, "Param [%d] expected in GUI save state order version [%d] not registered",
               paramID,
               iSaveOrder.fVersion);
      }
    }
    else
    {
      res |= iter->second->readFromStream(iStreamer);
    }
  }

  return res;
}

//------------------------------------------------------------------------
// GUIState::writeGUIState
//------------------------------------------------------------------------
tresult GUIState::writeGUIState(IBStreamer &oStreamer) const
{
  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();

  // nothing saved when there is nothing to save
  if(saveOrder.getCount() == 0)
    return kResultOk;

  if(saveOrder.fVersion >= 0)
    oStreamer.writeInt16u(static_cast<uint16>(saveOrder.fVersion));

  tresult res = kResultOk;

  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    auto iter = fJmbParams.find(paramID);
    if(iter == fJmbParams.cend())
    {
      DCHECK_F(existsVst(paramID)); // sanity check
      
      ParamValue value = fVstParameters->getParamNormalized(paramID);
      oStreamer.writeDouble(value);
    }
    else
    {
      res |= iter->second->writeToStream(oStreamer);
    }
  }

  return res;
}

//------------------------------------------------------------------------
// GUIState::init
//------------------------------------------------------------------------
tresult GUIState::init(VstParametersSPtr iVstParameters, IMessageProducer *iMessageProducer, IDialogHandler *iDialogHandler)
{
  fVstParameters = std::move(iVstParameters);
  fMessageProducer = iMessageProducer;
  fDialogHandler = iDialogHandler;

  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();
  if(saveOrder.getCount() > 0 && saveOrder.fVersion == 0)
  {
    DLOG_F(WARNING, "GUI Save State version is using the default entry order. Use Parameters::setGUISaveStateOrder to set explicitly.");
  }

#ifndef NDEBUG
  bool regOk = true;
  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    if(!fPluginParameters.getRawVstParamDef(paramID))
    {
      // no vst parameter registered with this id
      if(!getJmbParameter(paramID))
      {
        regOk = false;
        DLOG_F(ERROR, "Expected parameter [%d] used in GUISaveStateOrder not registered. If it is a Vst parameter, make sure you have added it to Parameters. If it is a Jmb parameter, make sure you have added it to the GUIState.", paramID);
      }
    }
  }
  DCHECK_F(regOk, "GUI state sanity check failed: some Jmb parameters are part of the save order but have not been added to GUIState.");
#endif

  return kResultOk;
}

//------------------------------------------------------------------------
// GUIState::createParamCxMgr
//------------------------------------------------------------------------
std::unique_ptr<GUIParamCxMgr> GUIState::createParamCxMgr()
{
  return std::unique_ptr<GUIParamCxMgr>(new GUIParamCxMgr(this));
}

//------------------------------------------------------------------------
// GUIState::getJmbParameter
//------------------------------------------------------------------------
std::shared_ptr<IGUIJmbParameter> GUIState::getJmbParameter(ParamID iParamID) const
{
  // 1. we locate it in the map
  auto iter = fJmbParams.find(iParamID);
  if(iter != fJmbParams.cend())
    return iter->second;

  // 2. not found => create it from its definition
  auto paramDef = fPluginParameters.getJmbParamDef(iParamID);
  if(paramDef)
  {
    auto param = paramDef->newGUIParam();
    // implementation note: method is defined const and this is an implementation detail, it does not change
    // the GUIState per se
    const_cast<GUIState *>(this)->addJmbParam(param);
    return param;
  }

  // 3. not such Jmb param
  return nullptr;
}

//------------------------------------------------------------------------
// GUIState::allocateMessage
//------------------------------------------------------------------------
IPtr<IMessage> GUIState::allocateMessage()
{
  if(fMessageProducer)
    return fMessageProducer->allocateMessage();
  else
    return IPtr<IMessage>();
}

//------------------------------------------------------------------------
// GUIState::sendMessage
//------------------------------------------------------------------------
tresult GUIState::sendMessage(IPtr<IMessage> iMessage)
{
  if(fMessageProducer)
    return fMessageProducer->sendMessage(iMessage);
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// GUIState::showDialog
//------------------------------------------------------------------------
bool GUIState::showDialog(std::string iTemplateName)
{
  if(fDialogHandler)
    return fDialogHandler->showDialog(iTemplateName);

  return false;
}

//------------------------------------------------------------------------
// GUIState::dismissDialog
//------------------------------------------------------------------------
bool GUIState::dismissDialog()
{
  if(fDialogHandler)
    return fDialogHandler->dismissDialog();

  return false;
}

}