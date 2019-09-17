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

#include "ParamDisplayView.h"

namespace pongasoft::VST::GUI::Views {

//------------------------------------------------------------------------
// ParamDisplayView::draw
//------------------------------------------------------------------------
void ParamDisplayView::draw(CDrawContext *iContext)
{
  if(style & kNoDrawStyle)
    return;

  auto string = fParam.exists() ? fParam.toUTF8String(fPrecisionOverride) : "";

  drawBack(iContext);
  drawPlatformText(iContext, UTF8String(string).getPlatformString());
  setDirty(false);
}

//------------------------------------------------------------------------
// ParamDisplayView::registerParameters
//------------------------------------------------------------------------
void ParamDisplayView::registerParameters()
{
  fParam = registerBaseParam(getTag());
}


}