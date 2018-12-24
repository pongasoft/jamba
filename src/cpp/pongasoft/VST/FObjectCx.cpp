#include <memory>

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

#include "FObjectCx.h"

namespace pongasoft {
namespace VST {

//------------------------------------------------------------------------
// FObjectCx::FObjectCx
//------------------------------------------------------------------------
FObjectCx::FObjectCx(FObject *iTarget) : fTarget{iTarget}
{
  DCHECK_F(fTarget != nullptr);

  fTarget->addRef();
  fTarget->addDependent(this);
  fIsConnected = true;
}

//------------------------------------------------------------------------
// FObjectCx::close
//------------------------------------------------------------------------
void FObjectCx::close()
{
  if(fIsConnected)
  {
    fTarget->removeDependent(this);
    fTarget->release();
    fIsConnected = false;
  }
}

//------------------------------------------------------------------------
// FObjectCxCallback::FObjectCxCallback
//------------------------------------------------------------------------
FObjectCxCallback::FObjectCxCallback(FObject *iTarget, FObjectCxCallback::ChangeCallback iChangeCallback)
  : FObjectCx(iTarget), fChangeCallback(std::move(iChangeCallback))
{
}

//------------------------------------------------------------------------
// FObjectCxCallback::close
//------------------------------------------------------------------------
void FObjectCxCallback::close()
{
  if(fChangeCallback)
    fChangeCallback = ChangeCallback();

  FObjectCx::close();
}

//------------------------------------------------------------------------
// FObjectCxCallback::update
//------------------------------------------------------------------------
void FObjectCxCallback::update(FUnknown * /* iChangedUnknown */, Steinberg::int32 iMessage)
{
  if(iMessage == IDependent::kChanged)
  {
    if(fChangeCallback)
      fChangeCallback();
  }
}

}
}