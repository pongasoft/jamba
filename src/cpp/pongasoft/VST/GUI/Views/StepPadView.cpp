/*
 * Copyright (c) 2019-2020 pongasoft
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
#include "StepPadView.h"
#include <pongasoft/Utils/Constants.h>


namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// StepPadView::draw
//------------------------------------------------------------------------
void StepPadView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  bool held = isHeld();
  if(fInverse)
    held = !held;

  if(held)
    drawHeldPad(iContext);
  else
    drawReleasedPad(iContext);

  setDirty(false);
}

//------------------------------------------------------------------------
// StepPadView::drawHeldPad
//------------------------------------------------------------------------
void StepPadView::drawHeldPad(CDrawContext *iContext)
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
    // no image => simply fill the surface with appropriate color (background and "held" color)
    // so that the button is fully functioning right away
    iContext->setFillColor(getMouseEnabled() ? getHeldColor() : getDisabledColor());
    iContext->drawRect(getViewSize(), kDrawFilled);
  }
}

//------------------------------------------------------------------------
// StepPadView::drawReleasedPad
//------------------------------------------------------------------------
void StepPadView::drawReleasedPad(CDrawContext *iContext)
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
// StepPadView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult StepPadView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fHeld = true;
  fEditor = fControlParameter.edit();
  fMousePosition = where;
  markDirty();

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// StepPadView::onMouseMoved
//------------------------------------------------------------------------
CMouseEventResult StepPadView::onMouseMoved(CPoint &where, const CButtonState &buttons)
{
  auto res = setNextValue(where, buttons);

  if(res == kMouseEventHandled)
    markDirty();

  return res;
}

//------------------------------------------------------------------------
// StepPadView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult StepPadView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  auto res = setNextValue(where, buttons);

  if(fEditor)
    fEditor->commit();
  fEditor = nullptr;

  fHeld = false;

  markDirty();

  return res;
}

//------------------------------------------------------------------------
// StepPadView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult StepPadView::onMouseCancel()
{
  if(fEditor)
    fEditor->rollback();
  fEditor = nullptr;

  fHeld = false;

  markDirty();

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// StepPadView::registerParameters
//------------------------------------------------------------------------
void StepPadView::registerParameters()
{
  CustomDiscreteControlView::registerParameters();

  // this section is only used during dev to display a warning...
#ifndef NDEBUG
  if(fControlParameter.getStepCount() == 0)
    DLOG_F(WARNING, "step count is 0 so this control will have no effect");

  if(fDragFactor <= 0)
    DLOG_F(WARNING, "Drag factor [%f] is <= 0 resulting in no effect", fDragFactor);

  if(fShiftDragFactor <= 0)
    DLOG_F(WARNING, "Shift Drag factor [%f] is <= 0 resulting in no effect", fShiftDragFactor);
#endif
}

//------------------------------------------------------------------------
// StepPadView::sizeToFit
//------------------------------------------------------------------------
bool StepPadView::sizeToFit()
{
  DLOG_F(INFO, "StepPadView::sizeToFit");
  return CustomView::sizeToFit(getImage(), fImageHasDisabledState ? 3 : 2);
}

//------------------------------------------------------------------------
// StepPadView::setNextValue
//------------------------------------------------------------------------
CMouseEventResult StepPadView::setNextValue(CPoint &iWhere, const CButtonState &iButtons)
{
  if(fControlParameter.getStepCount() == 0)
    return kMouseEventNotHandled;

  if(!(iButtons & kLButton))
    return kMouseEventNotHandled;

  if(fEditor)
  {
    // is the shift key being held?
    bool isShiftKeyHeld = iButtons.getModifierState() == CButton::kShift;

    // which increment to use?
    auto increment = isShiftKeyHeld ? getShiftStepIncrement() : getStepIncrement();

    // no increment => nothing to change
    if(increment == 0)
      return kMouseEventHandled;

    // determine appropriate drag factor (number of pixels to go through the range)
    auto dragFactor = isShiftKeyHeld ? fShiftDragFactor : fDragFactor;

    // sanity check...
    if(dragFactor <= 0)
      return kMouseEventHandled;

    // how much did we move (in pixels)
    auto delta = (fDirection == EPositiveDirection::kUp  || fDirection == EPositiveDirection::kDown) ?
                 fMousePosition.y - iWhere.y : // vertical   / y axis
                 fMousePosition.x - iWhere.x;  // horizontal / x axis

    // reverses the direction if necessary
    if(fDirection == EPositiveDirection::kDown || fDirection == EPositiveDirection::kRight)
      delta = -delta;

    // the size of the range of values is determined by (stepCount / increment) (for example if there are
    // 50 steps and an increment of 5 then there are 10 values in the range)
    // the dragFactor represents how many pixels are required to go through the range => each value in the range
    // represents dragFactor / (size of range) = dragFactor * (increment / stepCount) = (dragFactor * increment) / stepCount
    // we can then multiply delta by the inverse to get the multiple of increments
    delta *= fControlParameter.getStepCount() / (increment * dragFactor);

    // the delta is used as a multiple of increment (takes care of increase/decrease with sign of delta)
    increment *= static_cast<int32>(delta);

    if(increment != 0)
    {
      auto nextValue = computeNextDiscreteValue(getControlValue(), fControlParameter.getStepCount(), increment, getWrap());
      fEditor->setValue(nextValue);
      fMousePosition = iWhere;
    }

    return kMouseEventHandled;
  }
  else
    return kMouseEventNotHandled;
}


}