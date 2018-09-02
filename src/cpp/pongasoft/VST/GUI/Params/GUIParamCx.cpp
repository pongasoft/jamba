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
  fParamID{iParamID},
  fParameter{iParameter},
  fChangeListener{iChangeListener}
{
  DCHECK_F(fParameter != nullptr);
  DCHECK_F(fChangeListener != nullptr);

  fParameter->addRef();
  fParameter->addDependent(this);
  fIsConnected = true;
}

//------------------------------------------------------------------------
// GUIParamCx::close
//------------------------------------------------------------------------
void GUIParamCx::close()
{
  if(fIsConnected)
  {
    fParameter->removeDependent(this);
    fParameter->release();
    fIsConnected = false;
  }
}

//------------------------------------------------------------------------
// GUIParamCx::update
//------------------------------------------------------------------------
void GUIParamCx::update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage)
{
  if(iMessage == IDependent::kChanged)
  {
    fChangeListener->onParameterChange(fParamID);
  }
}

}
}
}
}