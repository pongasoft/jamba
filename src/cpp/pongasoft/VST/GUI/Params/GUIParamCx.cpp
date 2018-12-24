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
#include "GUIParamCx.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCx::GUIParamCx
//------------------------------------------------------------------------
GUIParamCx::GUIParamCx(ParamID iParamID, FObject *iParameter, Parameters::IChangeListener *iChangeListener) :
  FObjectCx(iParameter),
  fParamID{iParamID},
  fChangeListener{iChangeListener}
{
  DCHECK_F(fChangeListener != nullptr);
}

//------------------------------------------------------------------------
// GUIParamCx::close
//------------------------------------------------------------------------
void GUIParamCx::close()
{
  if(fChangeListener)
    fChangeListener = nullptr;

  FObjectCx::close();
}

//------------------------------------------------------------------------
// GUIParamCx::update
//------------------------------------------------------------------------
void GUIParamCx::update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage)
{
  if(iMessage == IDependent::kChanged)
  {
    if(fChangeListener)
      fChangeListener->onParameterChange(fParamID);
  }
}

}
}
}
}