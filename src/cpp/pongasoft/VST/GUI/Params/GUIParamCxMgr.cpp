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
#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::registerRawOptionalParam
//------------------------------------------------------------------------
bool GUIParamCxMgr::registerRawOptionalParam(TagID iParamID,
                                             GUIRawOptionalParam &oParam,
                                             Parameters::IChangeListener *iChangeListener)
{
  auto previousTagID = oParam.getTagID();

  if(previousTagID != iParamID)
    unregisterParam(previousTagID);

  bool paramChanged = false;

  // if VST parameter...
  if(existsVst(iParamID))
  {
    paramChanged = oParam.assign(fGUIState->getRawVstParameter(iParamID));
  }

  // if Jmb parameter...
  if(!paramChanged && existsJmb(iParamID))
  {
    auto param = fGUIState->getJmbParameter(iParamID);

    auto res = dynamic_cast<GUIJmbParameter<ParamValue> *>(param);
    if(res)
    {
      paramChanged = oParam.assign(res);
    }
    else
    {
      DLOG_F(WARNING, "jmb param [%d] is not of ParamValue type", iParamID);
    }
  }

  // no vst or jmb parameter match => using default
  if(!paramChanged)
  {
    paramChanged = oParam.clearAssignment(iParamID, true);
#ifndef NDEBUG
    if(iParamID != UNDEFINED_PARAM_ID)
      DLOG_F(WARNING, "could not find any parameter (vst or jmb) with id [%d]... reverting to default", iParamID);
#endif
  }

  if(iChangeListener)
  {
    fParamCxs[iParamID] = oParam.connect(iChangeListener);
  }
  else
  {
    unregisterParam(iParamID);
  }

  return paramChanged;
}

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


}
}
}
}