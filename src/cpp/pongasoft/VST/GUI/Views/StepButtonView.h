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
#pragma once

#include <vstgui4/vstgui/lib/controls/ccontrol.h>
#include <vstgui4/vstgui/lib/cdrawcontext.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include "CustomControlView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

/**
 * A step button lets you step through the values of a parameter by repeatedly clicking on the button. It works
 * for discrete values (`stepCount > 0`) in which case the increments should be an integer, or continuous values
 * (`stepCount == 0`) in which case the increment should be in the range `]-1.0, 1.0[`
 *
 * - The `step-increment` attribute defines whether this button will increment (positive) or decrement (negative)
 *   the value and by which amount.
 * - The `shift-step-increment` attribute allows for a different value when the shift modifier is being held (for example
 *   having bigger steps or smaller steps).
 * - The `wrap` attribute defines what happens when the value reaches its end of range after being incremented (resp.
 *   decremented). When set to `true` it will wrap around, otherwise it will remain at its max (resp. min).
 */
class StepButtonView : public RawCustomControlView
{
public:
  explicit StepButtonView(const CRect &iSize) : RawCustomControlView(iSize)
  {
  }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  // called to display the "held" button state
  virtual void drawHeldButton(CDrawContext *iContext);

  // called to display the "released" button state
  virtual void drawReleasedButton(CDrawContext *iContext);

  // input events (mouse/keyboard)
  CMouseEventResult onMouseDown(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseUp(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseCancel() override;
  int32_t onKeyDown(VstKeyCode &keyCode) override;
  int32_t onKeyUp(VstKeyCode &keyCode) override;

  // true if held
  bool isHeld() const { return fHeld; }

  // get/setHeldColor
  inline CColor const &getHeldColor() const { return fHeldColor; }
  void setHeldColor(CColor const &iColor) { fHeldColor = iColor; markDirty(); }

  // get/setReleasedColor
  inline CColor const &getReleasedColor() const { return fReleasedColor; }
  void setReleasedColor(CColor const &iColor) { fReleasedColor = iColor; markDirty(); }

  /**
   * returns the image for the button which should have 2 frames
   * The images should contain the following 2 frames (each is of size image height / 2):
   *   - at y = 0, the button in its released state
   *   - at y = image height / 2, the button in its held state
   */
  BitmapPtr getImage() const { return fImage; }

  /**
   * @see getImage
   */
  void setImage(BitmapPtr iImage) { fImage = iImage; markDirty(); }

  /**
   * @return the increment value applied every time the step button is released, negative value will be decreasing
   * and positive value will be increasing
   */
  double getStepIncrement() const { return fStepIncrement; }

  /**
   * @see getStepIncrement
   */
  void setStepIncrement(double iStepIncrement) { fStepIncrement = iStepIncrement; fButtonPolygon = nullptr; }

  /**
   * @return the increment value applied every time the step button is released while shift is being selected.
   * Negative value will be decreasing and positive value will be increasing
   */
  double getShiftStepIncrement() const { return fShiftStepIncrement; }

  /**
   * @see getShiftStepIncrement
   */
  void setShiftStepIncrement(double iStepIncrement) { fShiftStepIncrement = iStepIncrement; }

  /// get/setWrap => whether the discrete value wraps around when it reaches max (or min)
  inline bool getWrap() const { return fWrap; }
  void setWrap(bool iFlag) { fWrap = iFlag; markDirty(); }

protected:
  // registerParameters
  void registerParameters() override;

  /**
   * Adds the increment to the current value and wrap if necessary. Note that if the parameter represents a discrete
   * value (step count > 0), then the increment will be applied to the discrete value!
   *
   * @return the next value (which may be the current value if no wrap)
   */
  virtual ParamValue computeNextValue(double iIncrement) const;

  /**
   * @return the polygon used to draw the button (by default a triangle pointing up if `fStepIncrement` is positive).
   */
  virtual CDrawContext::PointList *getButtonPolygon();

  // setViewSize
  void setViewSize(const CRect &rect, bool invalid) override;

protected:
  /**
   * State of the button (`true` for held, `false` for released)
   */
  bool fHeld{false};

  double fStepIncrement{1.0};
  double fShiftStepIncrement{0.0};
  bool fWrap{false};
  CColor fHeldColor{kRedCColor};
  CColor fReleasedColor{kGreyCColor};
  BitmapSPtr fImage{nullptr};
  std::unique_ptr<CDrawContext::PointList> fButtonPolygon{};

public:
  class Creator : public CustomViewCreator<StepButtonView, RawCustomControlView>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerDoubleAttribute("step-increment", &StepButtonView::getStepIncrement, &StepButtonView::setStepIncrement);
      registerDoubleAttribute("shift-step-increment", &StepButtonView::getShiftStepIncrement, &StepButtonView::setShiftStepIncrement);
      registerBooleanAttribute("wrap", &StepButtonView::getWrap, &StepButtonView::setWrap);
      registerColorAttribute("held-color", &StepButtonView::getHeldColor, &StepButtonView::setHeldColor);
      registerColorAttribute("released-color", &StepButtonView::getReleasedColor, &StepButtonView::setReleasedColor);
      registerBitmapAttribute("button-image", &StepButtonView::getImage, &StepButtonView::setImage);
    }
  };
};

}
}
}
}