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
#include "GUIParamCxAware.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCxAware::registerRawVstParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges)
{
  if(!fParamCxMgr)
    return GUIRawVstParam{};

  return fParamCxMgr->registerRawVstParam(iParamID, iSubscribeToChanges ? this : nullptr);
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerVstBooleanParam
//------------------------------------------------------------------------
GUIVstParam<bool> GUIParamCxAware::registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<bool>(iParamID, iSubscribeToChanges);
}

//------------------------------------------------------------------------
// GUIParamCxAware::registerVstPercentParam
//------------------------------------------------------------------------
GUIVstParam<Percent> GUIParamCxAware::registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges)
{
  return registerVstParam<Percent>(iParamID, iSubscribeToChanges);
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
bool GUIParamCxAware::unregisterParam(ParamID iParamID)
{
  if(fParamCxMgr)
    return fParamCxMgr->unregisterParam(iParamID);
  return false;
}

//------------------------------------------------------------------------
// GUIParamCxAware::unregisterParam
//------------------------------------------------------------------------
GUIRawVstParam GUIParamCxAware::unregisterParam(GUIRawVstParam const &iParam)
{
  if(iParam.exists())
    fParamCxMgr->unregisterParam(iParam.getParamID());
  return GUIRawVstParam{};
}

}
}
}
}