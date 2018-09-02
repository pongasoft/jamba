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
// GUIParamCxMgr::registerRawVstParameter
//------------------------------------------------------------------------
std::unique_ptr<GUIRawVstParameter> GUIParamCxMgr::registerRawVstParameter(ParamID iParamID,
                                                                           Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getRawVstParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  if(iChangeListener)
  {
    fParamCxs[iParamID] = std::move(param->connect(iChangeListener));
  }
  else
  {
    unregisterParam(iParamID);
  }

  return param;
}



//------------------------------------------------------------------------
// GUIParamCxMgr::registerRawVstParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxMgr::registerRawVstParam(ParamID iParamID,
                                                  Parameters::IChangeListener *iChangeListener)
{
  auto param = registerRawVstParameter(iParamID, iChangeListener);

  if(!param)
  {
    return GUIRawVstParam{};
  }

  return GUIRawVstParam{std::move(param)};
}

//------------------------------------------------------------------------
// GUIParamCxMgr::unregisterParam
//------------------------------------------------------------------------
bool GUIParamCxMgr::unregisterParam(ParamID iParamID)
{
  return fParamCxs.erase(iParamID) == 1;
}

}
}
}
}