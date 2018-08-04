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
#include "GUIRawVstParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIRawVstParameter::Editor::Editor
///////////////////////////////////////////
GUIRawVstParameter::Editor::Editor(ParamID iParamID, VstParametersSPtr iVstParameters) :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)}
{
// DLOG_F(INFO, "GUIRawVstParameter::Editor(%d)", fParamID);
  fVstParameters->beginEdit(fParamID);
  fIsEditing = true;
  fInitialParamValue = fVstParameters->getParamNormalized(fParamID);
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::setValue
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::setValue(ParamValue iValue)
{
  tresult res = kResultFalse;
  if(fIsEditing)
  {
    res = fVstParameters->setParamNormalized(fParamID, iValue);
    if(res == kResultOk)
      fVstParameters->performEdit(fParamID, fVstParameters->getParamNormalized(fParamID));
  }
  return res;
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::commit
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::commit()
{
  if(fIsEditing)
  {
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::rollback
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::rollback()
{
  if(fIsEditing)
  {
    setValue(fInitialParamValue);
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawVstParameter::GUIRawVstParameter
///////////////////////////////////////////
GUIRawVstParameter::GUIRawVstParameter(ParamID iParamID, VstParametersSPtr iVstParameters)  :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)}
{
  // DLOG_F(INFO, "GUIRawVstParameter::GUIRawVstParameter(%d)", fParamID);
  DCHECK_F(fVstParameters->exists(fParamID), "Missing parameter [%d]", iParamID);
}

}
}
}
}