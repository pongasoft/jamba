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
#include "DiscreteButtonView.h"

#include <pongasoft/VST/ParamConverters.h>

#include <vstgui4/vstgui/lib/cdrawcontext.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

//------------------------------------------------------------------------
// DiscreteButtonView::setFrames
//------------------------------------------------------------------------
void DiscreteButtonView::setFrames(int iFrames)
{
  if(iFrames != 2 && iFrames != 4)
  {
    DLOG_F(WARNING, "frames should be either 2 or 4");
  }
  else
    fFrames = iFrames;
}

//------------------------------------------------------------------------
// DiscreteButtonView::draw
//------------------------------------------------------------------------
void DiscreteButtonView::draw(CDrawContext *iContext)
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
// DiscreteButtonView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult DiscreteButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fPressed = true;
  markDirty();
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// DiscreteButtonView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult DiscreteButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fPressed = false;
  setOn();
  markDirty();
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// DiscreteButtonView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult DiscreteButtonView::onMouseCancel()
{
  fPressed = false;
  markDirty();
  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// DiscreteButtonView::onKeyDown
//------------------------------------------------------------------------
int32_t DiscreteButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    fPressed = true;
    markDirty();
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// DiscreteButtonView::onKeyUp
//------------------------------------------------------------------------
int32_t DiscreteButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    fPressed = false;
    setOn();
    markDirty();
    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
// DiscreteButtonView::sizeToFit
//------------------------------------------------------------------------
bool DiscreteButtonView::sizeToFit()
{
  return CustomView::sizeToFit(getImage(), fFrames);
}

//------------------------------------------------------------------------
// DiscreteButtonView::setStep
//------------------------------------------------------------------------
void DiscreteButtonView::setStep(int32 step)
{
  fStep = step;
  markDirty();
}

//------------------------------------------------------------------------
// DiscreteButtonView::isOn
//------------------------------------------------------------------------
bool DiscreteButtonView::isOn() const
{
  if(fControlParameter.exists())
  {
    return convertNormalizedValueToDiscreteValue(fControlParameter.getStepCount(), getControlValue()) == fStep;
  }
  else
    return false;
}

//------------------------------------------------------------------------
// DiscreteButtonView::setOn
//------------------------------------------------------------------------
void DiscreteButtonView::setOn()
{
  if(fControlParameter.exists())
  {
    setControlValue(convertDiscreteValueToNormalizedValue(fControlParameter.getStepCount(), fStep));
  }
}

//------------------------------------------------------------------------
// DiscreteButtonView::registerParameters
//------------------------------------------------------------------------
void DiscreteButtonView::registerParameters()
{
  TCustomControlView::registerParameters();
#ifndef NDEBUG
  if(fControlParameter.exists() && fControlParameter.getStepCount() == 0)
    DLOG_F(WARNING, "%d parameter is not discrete (stepCount == 0)", fControlParameter.getParamID());
#endif
}

}
}
}
}
