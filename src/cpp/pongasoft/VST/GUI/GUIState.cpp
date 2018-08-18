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
#include <sstream>
#include "GUIState.h"
#include <pongasoft/VST/GUI/Params/GUIParamCxMgr.h>

namespace pongasoft {
namespace VST {
namespace GUI {

//------------------------------------------------------------------------
// GUIState::addJmbParam
//------------------------------------------------------------------------
void GUIState::addJmbParam(std::unique_ptr<IGUIJmbParameter> iParameter)
{
  DCHECK_F(iParameter != nullptr);

  ParamID paramID = iParameter->getParamID();

  DCHECK_F(fPluginParameters.getJmbParamDef(paramID) != nullptr, "jmb parameter [%d] not registered", paramID);
  DCHECK_F(fJmbParams.find(paramID) == fJmbParams.cend(), "duplicate paramID [%d]", paramID);

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
  auto normalizedState = fPluginParameters.readRTState(iStreamer);

  if(normalizedState)
  {
    return setParamNormalized(normalizedState.get());
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIState::readGUIState
//------------------------------------------------------------------------
tresult GUIState::readGUIState(IBStreamer &iStreamer)
{
  auto const &saveOrder = fPluginParameters.getGUISaveStateOrder();

  if(saveOrder.fVersion >= 0)
  {
    uint16 stateVersion;
    if(!iStreamer.readInt16u(stateVersion))
      stateVersion = 0;

    // TODO handle multiple versions
    if(stateVersion != saveOrder.fVersion)
    {
      DLOG_F(WARNING, "unexpected GUI state version %d", stateVersion);
    }
  }

  tresult res = kResultOk;

  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    auto iter = fJmbParams.find(paramID);
    if(iter == fJmbParams.cend())
    {
      // not found => regular parameter
      ParamValue value = fPluginParameters.readNormalizedValue(paramID, iStreamer);
      fVstParameters->setParamNormalized(paramID, value);
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

  if(saveOrder.fVersion >= 0)
    oStreamer.writeInt16u(static_cast<uint16>(saveOrder.fVersion));

  tresult res = kResultOk;

  for(int i = 0; i < saveOrder.getCount(); i++)
  {
    auto paramID = saveOrder.fOrder[i];
    auto iter = fJmbParams.find(paramID);
    if(iter == fJmbParams.cend())
    {
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
tresult GUIState::init(VstParametersSPtr iVstParameters, IMessageProducer *iMessageProducer)
{
  fVstParameters = std::move(iVstParameters);
  fMessageProducer = iMessageProducer;
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
IGUIJmbParameter *GUIState::getJmbParameter(ParamID iParamID) const
{
  auto iter = fJmbParams.find(iParamID);
  if(iter == fJmbParams.cend())
    return nullptr;
  return iter->second.get();
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

}
}
}