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

    if(getStepIncrement() >= 0)
    {
      fButtonPolygon->emplace_back(viewSize.left, viewSize.bottom);
      fButtonPolygon->emplace_back(viewSize.right, viewSize.bottom);
      fButtonPolygon->emplace_back((viewSize.left + viewSize.right) / 2.0, viewSize.top);
    }
    else
    {
      fButtonPolygon->emplace_back(viewSize.right, viewSize.top);
      fButtonPolygon->emplace_back(viewSize.left, viewSize.top);
      fButtonPolygon->emplace_back((viewSize.left + viewSize.right) / 2.0, viewSize.bottom);
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
  RawCustomControlView::registerParameters();

  // this section is only used during dev to display a warning...
#ifndef NDEBUG
  const auto stepIncrement = getStepIncrement();

  if(fControlParameter.exists())
  {
    const auto stepCount = fControlParameter.getStepCount();

    if(stepCount == 0)
    {
      // continuous value => step increment should be in the range ]-1.0, 1.0[ otherwise it does not make
      // much sense
      if(stepIncrement <= -1.0 || stepIncrement >= 1.0)
        DLOG_F(WARNING, "parameter [%d] represents a continuous value (stepCount == 0) "
                        "so step-increment (%f) should be in the ]-1.0, 1.0[ range",
               fControlParameter.getParamID(),
               stepIncrement);
    }
    else
    {
      auto s = std::abs(stepIncrement);

      // discrete value => step increment should be an integer
      if(std::floor(s) != s)
        DLOG_F(WARNING, "parameter [%d] represents a discrete value (stepCount == %d) "
                        "so step-increment (%f) should be an integer",
               fControlParameter.getParamID(),
               stepCount,
               stepIncrement);
    }
  }

  if(stepIncrement == 0.0)
  {
    DLOG_F(WARNING, "step-increment is set to 0.0 so this control will have no effect");
  }
#endif

}

//------------------------------------------------------------------------
// StepButtonView::computeNextValue
//------------------------------------------------------------------------
ParamValue StepButtonView::computeNextValue(double iIncrement) const
{
  // no increment, noop
  if(iIncrement == 0.0)
    return getControlValue();

  auto stepCount = fControlParameter.exists() ? fControlParameter.getStepCount() : 0;

  if(stepCount > 0)
  {
    // Handles discrete value case => convert normalized value to discrete value before applying increment
    auto discreteValue = convertNormalizedValueToDiscreteValue(stepCount, getControlValue());
    discreteValue += static_cast<decltype(discreteValue)>(iIncrement);

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

    // converts back to normalized value
    return convertDiscreteValueToNormalizedValue(stepCount, discreteValue);
  }
  else
  {
    // Handles continuous case => use normalized value directly
    auto value = getControlValue();
    value += iIncrement;

    // handles wrapping
    if(getWrap())
    {
      if(iIncrement > 0)
      {
        while(value > 1.0)
          value -= 1.0;
      }
      else
      {
        while(value < 0.0)
          value += 1.0;
      }
    }
    else
      // no wrapping => simply clamp the value to the range
      value = Utils::clamp(value, 0.0, 1.0);

    return value;
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