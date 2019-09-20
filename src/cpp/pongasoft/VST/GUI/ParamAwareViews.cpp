/*
 * Copyright (c) 2019 pongasoft
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
#include <vstgui4/vstgui/lib/cview.h>
#include "ParamAwareViews.h"

namespace pongasoft::VST::GUI {


//------------------------------------------------------------------------
// ParamAwareViews::closeAll
//------------------------------------------------------------------------
void ParamAwareViews::closeAll()
{
  fParamAwareViews.clear();
}

//------------------------------------------------------------------------
// ParamAwareViews::viewWillDelete
//------------------------------------------------------------------------
void ParamAwareViews::viewWillDelete(CView *iView)
{
  auto iter = fParamAwareViews.find(iView);

  if(iter != fParamAwareViews.end())
  {
    iView->unregisterViewListener(this);
    fParamAwareViews.erase(iter);
  }
}

}