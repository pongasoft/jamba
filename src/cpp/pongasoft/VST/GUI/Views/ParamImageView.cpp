/*
 * Copyright (c) 2020 pongasoft
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

#include "ParamImageView.h"

namespace pongasoft::VST::GUI::Views {

//------------------------------------------------------------------------
// ParamImageView::draw
//------------------------------------------------------------------------
void ParamImageView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  if(fImage)
  {
    auto frames = getFrames();
    CCoord frameHeight = fImage->getHeight() / frames;
    auto frameIndex = getControlValue();
    if(fInverse)
      frameIndex = frames - frameIndex - 1;

    fImage->draw(iContext, getViewSize(), CPoint{0, frameIndex * frameHeight});
  }
}

//------------------------------------------------------------------------
// ParamImageView::registerParameters
//------------------------------------------------------------------------
void ParamImageView::registerParameters()
{
  CustomDiscreteControlView::registerParameters();
#ifndef NDEBUG
  if(fControlParameter.getStepCount() == 0 && getControlTag() != UNDEFINED_PARAM_ID)
    DLOG_F(WARNING, "%d parameter is not discrete (stepCount == 0)", fControlParameter.getParamID());
#endif
}

//------------------------------------------------------------------------
// ParamImageView::getFrames
//------------------------------------------------------------------------
int32 ParamImageView::getFrames() const
{
  auto frames = fControlParameter.getStepCount() + 1;

  if(frames <= 0)
    frames = static_cast<int32>(1);

  return frames;
}

//------------------------------------------------------------------------
// ParamImageView::sizeToFit
//------------------------------------------------------------------------
bool ParamImageView::sizeToFit()
{
  return CustomView::sizeToFit(getImage(), getFrames());
}

}