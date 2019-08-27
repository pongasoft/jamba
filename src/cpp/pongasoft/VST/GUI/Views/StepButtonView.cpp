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
#include "StepButtonView.h"
#include <pongasoft/Utils/Constants.h>


namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// StepButtonView::draw
//------------------------------------------------------------------------
void StepButtonView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  if(isHeld())
    drawHeldButton(iContext);
  else
    drawReleasedButton(iContext);

  setDirty(false);
}

//------------------------------------------------------------------------
// StepButtonView::drawHeldButton
//------------------------------------------------------------------------
void StepButtonView::drawHeldButton(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord y = fImage->getHeight() / 2;
    fImage->draw(iContext, getViewSize(), CPoint{0, y});
  }
  else
  {
    iContext->setFillColor(getHeldColor());
    iContext->drawPolygon(*getButtonPolygon(), kDrawFilled);
  }
}

//------------------------------------------------------------------------
// StepButtonView::drawReleasedButton
//------------------------------------------------------------------------
void StepButtonView::drawReleasedButton(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord y = 0;
    fImage->draw(iContext, getViewSize(), CPoint{0, y});
  }
  else
  {
    iContext->setFillColor(getReleasedColor());
    iContext->drawPolygon(*getButtonPolygon(), kDrawFilled);
  }
}

//------------------------------------------------------------------------
// StepButtonView::getButtonPolygon
//------------------------------------------------------------------------
CDrawContext::PointList *StepButtonView::getButtonPolygon()
{
  if(!fButtonPolygon)
  {
    auto viewSize = getViewSize();

    fButtonPolygon = std::make_unique<CDrawContext::PointList>();

    // a triangle pointing up or down depending on fStepIncrement sign

    auto direction = getArrowDirection();

    if(direction == EArrowDirection::kAuto)
      direction = getStepIncrement() >= 0 ? EArrowDirection::kUp : EArrowDirection::kDown;

    switch(direction)
    {
      case EArrowDirection::kUp:
        fButtonPolygon->emplace_back(viewSize.left, viewSize.bottom);
        fButtonPolygon->emplace_back(viewSize.right, viewSize.bottom);
        fButtonPolygon->emplace_back((viewSize.left + viewSize.right) / 2.0, viewSize.top);
        break;

      case EArrowDirection::kRight:
        fButtonPolygon->emplace_back(viewSize.left, viewSize.top);
        fButtonPolygon->emplace_back(viewSize.left, viewSize.bottom);
        fButtonPolygon->emplace_back(viewSize.right, (viewSize.top + viewSize.bottom) / 2.0);
        break;

      case EArrowDirection::kDown:
        fButtonPolygon->emplace_back(viewSize.right, viewSize.top);
        fButtonPolygon->emplace_back(viewSize.left, viewSize.top);
        fButtonPolygon->emplace_back((viewSize.left + viewSize.right) / 2.0, viewSize.bottom);
        break;

      case EArrowDirection::kLeft:
        fButtonPolygon->emplace_back(viewSize.right, viewSize.top);
        fButtonPolygon->emplace_back(viewSize.right, viewSize.bottom);
        fButtonPolygon->emplace_back(viewSize.left, (viewSize.top + viewSize.bottom) / 2.0);
        break;

      default:
        DLOG_F(ERROR, "should not be here...");
    }
  }

  return fButtonPolygon.get();
}


//------------------------------------------------------------------------
// StepButtonView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult StepButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  fHeld = true;
  markDirty();

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// StepButtonView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult StepButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(computeNextValue(buttons.getModifierState() == CButton::kShift ? getShiftStepIncrement() : getStepIncrement()));
  fHeld = false;
  markDirty();

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// StepButtonView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult StepButtonView::onMouseCancel()
{
  fHeld = false;
  markDirty();

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// StepButtonView::onKeyDown
//------------------------------------------------------------------------
int32_t StepButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN)
  {
    if(keyCode.modifier == 0 || keyCode.modifier == MODIFIER_SHIFT)
    {
      fHeld = true;
      markDirty();
      return kKeyboardEventHandled;
    }
  }
  return kKeyboardEventNotHandled;
}

//------------------------------------------------------------------------
// StepButtonView::onKeyUp
//------------------------------------------------------------------------
int32_t StepButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN)
  {
    if(keyCode.modifier == 0 || keyCode.modifier == MODIFIER_SHIFT)
    {
      setControlValue(computeNextValue(keyCode.modifier == MODIFIER_SHIFT? getShiftStepIncrement() : getStepIncrement()));
      fHeld = false;
      markDirty();
      return kKeyboardEventHandled;
    }
  }
  return kKeyboardEventNotHandled;
}

//------------------------------------------------------------------------
// StepButtonView::registerParameters
//------------------------------------------------------------------------
void StepButtonView::registerParameters()
{
  CustomDiscreteControlView::registerParameters();

  // this section is only used during dev to display a warning...
#ifndef NDEBUG
  const auto stepIncrement = getStepIncrement();

  const auto stepCount = computeStepCount();

  if(stepCount == 0)
  {
    DLOG_F(WARNING, "computed step count is 0 so this control will have no effect");
  }

//  if(stepCount == 0)
//  {
//    // continuous value => step increment should be in the range ]-1.0, 1.0[ otherwise it does not make
//    // much sense
//    if(stepIncrement <= -1.0 || stepIncrement >= 1.0)
//      DLOG_F(WARNING, "parameter [%d] represents a continuous value (stepCount == 0) "
//                      "so step-increment (%f) should be in the ]-1.0, 1.0[ range",
//             fControlParameter.getTagID(),
//             stepIncrement);
//  }
//  else
//  {
//    auto s = std::abs(stepIncrement);
//
//    // discrete value => step increment should be an integer
//    if(std::floor(s) != s)
//      DLOG_F(WARNING, "parameter [%d] represents a discrete value (stepCount == %d) "
//                      "so step-increment (%f) should be an integer",
//             fControlParameter.getTagID(),
//             stepCount,
//             stepIncrement);
//  }

  if(stepIncrement == 0)
  {
    DLOG_F(WARNING, "step-increment is set to 0 so this control will have no effect");
  }
#endif

}

//------------------------------------------------------------------------
// StepButtonView::computeNextValue
//------------------------------------------------------------------------
int32 StepButtonView::computeNextValue(int32 iIncrement) const
{
  // no increment, noop
  if(iIncrement == 0)
    return getControlValue();

  auto stepCount = computeStepCount();

  if(stepCount > 0)
  {
    auto discreteValue = getControlValue() + iIncrement;

    // handles wrapping
    if(getWrap())
    {
      if(iIncrement > 0)
      {
        while(discreteValue > stepCount)
          discreteValue -= stepCount + 1;
      }
      else
      {
        while(discreteValue < 0)
          discreteValue += stepCount + 1;
      }
    }
    else
    {
      // no wrapping => simply clamp the value to the range
      discreteValue = Utils::clamp(discreteValue, Utils::ZERO_INT32, stepCount);
    }

    return discreteValue;
  }
  else
  {
    return getControlValue();
  }
}

//------------------------------------------------------------------------
// StepButtonView::setViewSize
//------------------------------------------------------------------------
void StepButtonView::setViewSize(const CRect &rect, bool invalid)
{
  if(getViewSize() != rect)
  {
    // invalidate polygon which depends on viewSize
    fButtonPolygon = nullptr;
  }

  CView::setViewSize(rect, invalid);
}

}
}
}
}