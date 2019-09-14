/*
 * Copyright (c) 2018-2019 pongasoft
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
#include "MomentaryButtonView.h"

#include <vstgui4/vstgui/lib/cdrawcontext.h>

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// MomentaryButtonView::draw
//------------------------------------------------------------------------
void MomentaryButtonView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  bool on = isOn();
  if(fInverse)
    on = !on;

  if(on)
    drawOn(iContext);
  else
    drawOff(iContext);

  setDirty(false);
}

//------------------------------------------------------------------------
// MomentaryButtonView::drawOn
//------------------------------------------------------------------------
void MomentaryButtonView::drawOn(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord frameHeight;
    int frameIndex;

    if(fImageHasDisabledState)
    {
      frameHeight = fImage->getHeight() / 3;
      frameIndex = getMouseEnabled() ? 2 : 0;
    }
    else
    {
      frameHeight = fImage->getHeight() / 2;
      frameIndex = 1;
    }

    fImage->draw(iContext, getViewSize(), CPoint{0, frameIndex * frameHeight});
  }
  else
  {
    // no image => simply fill the surface with appropriate color (background and "on" color)
    // so that the button is fully functioning right away
    iContext->setFillColor(getMouseEnabled() ? getOnColor() : getDisabledColor());
    iContext->drawRect(getViewSize(), kDrawFilled);
  }
}

//------------------------------------------------------------------------
// MomentaryButtonView::drawOff
//------------------------------------------------------------------------
void MomentaryButtonView::drawOff(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord frameHeight;
    int frameIndex;

    if(fImageHasDisabledState)
    {
      frameHeight = fImage->getHeight() / 3;
      frameIndex = getMouseEnabled() ? 1 : 0;
    }
    else
    {
      frameHeight = fImage->getHeight() / 2;
      frameIndex = 0;
    }

    fImage->draw(iContext, getViewSize(), CPoint{0, frameIndex * frameHeight});
  }
  else
  {
    if(!getMouseEnabled())
    {
      iContext->setFillColor(getDisabledColor());
      iContext->drawRect(getViewSize(), kDrawFilled);
    }
    // else already done: when there is no image the background color is drawn
  }
}

//------------------------------------------------------------------------
// MomentaryButtonView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult MomentaryButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(getComputedOnStep());
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// MomentaryButtonView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult MomentaryButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(getComputedOffStep());
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// MomentaryButtonView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult MomentaryButtonView::onMouseCancel()
{
  setControlValue(getComputedOffStep());
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// MomentaryButtonView::onKeyDown
//------------------------------------------------------------------------
int32_t MomentaryButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(getComputedOnStep());
    setDirty(true);
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// MomentaryButtonView::onKeyUp
//------------------------------------------------------------------------
int32_t MomentaryButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(getComputedOffStep());
    setDirty(true);
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// MomentaryButtonView::sizeToFit
//------------------------------------------------------------------------
bool MomentaryButtonView::sizeToFit()
{
  DLOG_F(INFO, "MomentaryButtonView::sizeToFit");
  return CustomView::sizeToFit(getImage(), 2);
}

//------------------------------------------------------------------------
// MomentaryButtonView::getComputedOnStep
//------------------------------------------------------------------------
int32 MomentaryButtonView::getComputedOnStep() const
{
  auto onStep = getOnStep();

  if(onStep > -1)
    return onStep;

  onStep = fControlParameter.getStepCount();

  if(onStep > 0)
    return onStep;
  else
    return 1;
}

}