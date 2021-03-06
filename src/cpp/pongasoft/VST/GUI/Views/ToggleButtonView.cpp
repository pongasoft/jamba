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
#include "ToggleButtonView.h"

#include <vstgui4/vstgui/lib/cdrawcontext.h>

namespace pongasoft::VST::GUI::Views {

//------------------------------------------------------------------------
// ToggleButtonView::setFrames
//------------------------------------------------------------------------
void ToggleButtonView::setFrames(int iFrames)
{
  if(iFrames != 2 && iFrames != 4)
  {
    DLOG_F(WARNING, "frames should be either 2 or 4");
  }
  else
    fFrames = iFrames;
}

//------------------------------------------------------------------------
// ToggleButtonView::draw
//------------------------------------------------------------------------
void ToggleButtonView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  bool on = isOn();
  if(fInverse)
    on = !on;

  if(fImage)
  {
    int frameIndex;
    CCoord frameHeight = fImage->getHeight() / getFrames();
    if(getFrames() == 4)
    {
      frameIndex = on ? 2 : 0;
      if(fPressed)
        frameIndex++;
    }
    else
    {
      frameIndex = on ? 1 : 0;
    }

    fImage->draw(iContext, getViewSize(), CPoint{0, frameIndex * frameHeight});
  }
  else
  {
    // no image => simply fill the surface with appropriate color (background and "on" color)
    // so that the button is fully functioning right away
    if(on)
    {
      iContext->setFillColor(getOnColor());
      iContext->drawRect(getViewSize(), kDrawFilled);
    }

    if(getFrames() == 4)
    {
      if(fPressed)
      {
        iContext->setFillColor(CColor{0,0,0,120});
        iContext->drawRect(getViewSize(), kDrawFilled);
      }
    }
  }

  setDirty(false);
}

//------------------------------------------------------------------------
// ToggleButtonView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult ToggleButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fPressed = true;
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ToggleButtonView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult ToggleButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fPressed = false;
  toggleControlValue();
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ToggleButtonView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult ToggleButtonView::onMouseCancel()
{
  fPressed = false;
  setDirty(true);
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ToggleButtonView::onKeyDown
//------------------------------------------------------------------------
int32_t ToggleButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    fPressed = true;
    setDirty(true);
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// ToggleButtonView::onKeyUp
//------------------------------------------------------------------------
int32_t ToggleButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    fPressed = false;
    toggleControlValue();
    setDirty(true);
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// ToggleButtonView::sizeToFit
//------------------------------------------------------------------------
bool ToggleButtonView::sizeToFit()
{
  return CustomView::sizeToFit(getImage(), fFrames);
}

//------------------------------------------------------------------------
// ToggleButtonView::getComputedOnStep
//------------------------------------------------------------------------
int32 ToggleButtonView::getComputedOnStep() const
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

//------------------------------------------------------------------------
// ToggleButtonView::toggleControlValue
//------------------------------------------------------------------------
int32 ToggleButtonView::toggleControlValue()
{
  auto newControlValue = isOff() ? getComputedOnStep() : getComputedOffStep();
  setControlValue(newControlValue);
  return newControlValue;
}

}
