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
#include "GUIParamCxMgr.h"

namespace pongasoft::VST::GUI::Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::invokeAll
//------------------------------------------------------------------------
void GUIParamCxMgr::invokeAll()
{
  for(auto &it: fParamCxs)
  {
    it.second->onTargetChange();
  }
}

//------------------------------------------------------------------------
// GUIParamCxMgr::unregisterParam
//------------------------------------------------------------------------
bool GUIParamCxMgr::unregisterParam(TagID iParamID)
{
  if(iParamID >= 0)
    return fParamCxs.erase(static_cast<ParamID>(iParamID)) == 1;
  return false;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::unregisterAll
//------------------------------------------------------------------------
void GUIParamCxMgr::unregisterAll()
{
  fParamCxs.clear();
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerOptionalDiscreteParam
//------------------------------------------------------------------------
GUIOptionalParam<int32> GUIParamCxMgr::registerOptionalDiscreteParam(TagID iParamID,
                                                                     int32 iStepCount,
                                                                     Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->findParam(iParamID);
  std::shared_ptr<ITGUIParameter<int32>> optionalParam = nullptr;

  if(param)
  {
    auto discreteParam = param->asDiscreteParameter(iStepCount);

    if(discreteParam)
    {
      optionalParam = std::move(discreteParam);
    }
    else
    {
      DLOG_F(WARNING, "param [%d] is not a discrete parameter or cannot be interpreted as one", iParamID);
    }
  }

  // no vst or jmb parameter match => using default
  if(!optionalParam)
  {
    auto pint32 = VstUtils::make_sfo<GUIValParameter<int32>>(iParamID, 0);

    if(iStepCount > 0)
      optionalParam = std::move(pint32->asDiscreteParameter(iStepCount));
    else
      optionalParam = std::move(pint32);

#ifndef NDEBUG
    if(iParamID != UNDEFINED_PARAM_ID)
      DLOG_F(WARNING, "could not find any parameter (vst or jmb) with id [%d]... reverting to default", iParamID);
#endif
  }

  return __registerListener(GUIOptionalParam<int32>(std::move(optionalParam)), iChangeListener);
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerBaseParam
//------------------------------------------------------------------------
IGUIParam GUIParamCxMgr::registerBaseParam(TagID iParamID, Parameters::IChangeListener *iChangeListener)
{
  auto param = iParamID >= 0 ? fGUIState->findParam(static_cast<ParamID>(iParamID)) : nullptr;

  return __registerListener(IGUIParam(param), iChangeListener);
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerRawVstParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxMgr::registerRawVstParam(ParamID iParamID, Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
  }

  return __registerListener(GUIRawVstParam(std::move(param)), iChangeListener);
}

}