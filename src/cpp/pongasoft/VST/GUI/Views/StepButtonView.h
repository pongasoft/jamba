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
#pragma once

#include <vstgui4/vstgui/lib/controls/ccontrol.h>
#include <vstgui4/vstgui/lib/cdrawcontext.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include "CustomControlView.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

/**
 * A step button lets you step through the values of a parameter by repeatedly clicking on the button.
 *
 * This view works for any parameter (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter.
 *
 * In addition to the attributes exposed by `CustomDiscreteControlView`, this class exposes the following attributes:
 *
 * Attribute              | Description
 * ---------              | -----------
 * `step-increment`       | @copydoc getStepIncrement()
 * `shift-step-increment` | @copydoc getShiftStepIncrement()
 * `wrap`                 | @copydoc getWrap()
 * `held-color`           | @copydoc getHeldColor()
 * `released-color`       | @copydoc getReleasedColor()
 * `button-image`         | @copydoc getImage()
 * `arrow-direction`      | @copydoc getArrowDirection()
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class StepButtonView : public CustomDiscreteControlView
{
public:
  /**
   * Represents the direction of the arrow (if not using an image) */
  enum class EArrowDirection
  {
    kAuto,
    kUp,
    kRight,
    kDown,
    kLeft
  };
public:
  explicit StepButtonView(const CRect &iSize) : CustomDiscreteControlView(iSize)
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

  //! Color to use when the button is held and no image is provided
  inline CColor const &getHeldColor() const { return fHeldColor; }
  void setHeldColor(CColor const &iColor) { fHeldColor = iColor; markDirty(); }

  //! Color to use when the button is not held and no image is provided
  inline CColor const &getReleasedColor() const { return fReleasedColor; }
  void setReleasedColor(CColor const &iColor) { fReleasedColor = iColor; markDirty(); }

  /**
   * The image for the button.
   *
   * The image should contain 2 frames (each is of size image height / 2) with the following convention:
   *
   *   y | frame
   *   - | -----
   *   0 | the button in its released state
   *   image height / 2 | the button in its held state
   *
   *   Example: ![2 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/arrow_up.png)
   *
   */
  BitmapPtr getImage() const { return fImage; }

   //! Attribute `button-image`.
  void setImage(BitmapPtr iImage) { fImage = iImage; markDirty(); }

  /**
   * Value by which this button will increment (positive) or decrement (negative) the parameter.
   */
  int32 getStepIncrement() const { return fStepIncrement; }

  /**
   * @see getStepIncrement
   */
  void setStepIncrement(int32 iStepIncrement) { fStepIncrement = iStepIncrement; fButtonPolygon = nullptr; }

  /**
   * Value by which this button will increment (positive) or decrement (negative) the parameter when the shift key
   * modified is being held. This allows to have bigger steps (or smaller steps) when shift is used.
   */
  int32 getShiftStepIncrement() const { return fShiftStepIncrement; }

  /**
   * @see getShiftStepIncrement
   */
  void setShiftStepIncrement(int32 iStepIncrement) { fShiftStepIncrement = iStepIncrement; }

  /**
   * Defines what happens when the value reaches its end of range after being incremented (resp. decremented).
   * When set to `true` it will wrap around, otherwise it will remain at its max (resp. min) */
  inline bool getWrap() const { return fWrap; }
  void setWrap(bool iFlag) { fWrap = iFlag; markDirty(); }

  /**
   * Used only when no bitmap provided in order to draw an arrow pointing in the direction defined by this attribute.
   * A value of `auto` will trigger the rendering of an arrow up if `step-increment` is positive, and down if negative.
   */
  EArrowDirection getArrowDirection() const { return fArrowDirection; }

  //! Attribute `arrow-direction`.
  void setArrowDirection(EArrowDirection iArrowDirection) { fArrowDirection = iArrowDirection; fButtonPolygon = nullptr; markDirty(); }

protected:
  // registerParameters
  void registerParameters() override;

  /**
   * Adds the increment to the current value and wrap if necessary.
   *
   * @return the next value (which may be the current value if no wrap)
   */
  virtual int32 computeNextValue(int32 iIncrement) const;

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

  int32 fStepIncrement{1};
  int32 fShiftStepIncrement{1};
  bool fWrap{false};
  CColor fHeldColor{kRedCColor};
  CColor fReleasedColor{kGreyCColor};
  BitmapSPtr fImage{nullptr};

  EArrowDirection fArrowDirection{EArrowDirection::kAuto};
  std::unique_ptr<CDrawContext::PointList> fButtonPolygon{};

public:
  class Creator : public CustomViewCreator<StepButtonView, CustomDiscreteControlView>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<int32>("step-increment", &StepButtonView::getStepIncrement, &StepButtonView::setStepIncrement);
      registerIntegerAttribute<int32>("shift-step-increment", &StepButtonView::getShiftStepIncrement, &StepButtonView::setShiftStepIncrement);
      registerBooleanAttribute("wrap", &StepButtonView::getWrap, &StepButtonView::setWrap);
      registerColorAttribute("held-color", &StepButtonView::getHeldColor, &StepButtonView::setHeldColor);
      registerColorAttribute("released-color", &StepButtonView::getReleasedColor, &StepButtonView::setReleasedColor);
      registerBitmapAttribute("button-image", &StepButtonView::getImage, &StepButtonView::setImage);
      registerListAttribute<EArrowDirection>("arrow-direction",
                                             &StepButtonView::getArrowDirection, &StepButtonView::setArrowDirection,
                                             {
                                               {"auto", EArrowDirection::kAuto},
                                               {"up", EArrowDirection::kUp},
                                               {"right", EArrowDirection::kRight},
                                               {"down", EArrowDirection::kDown},
                                               {"left", EArrowDirection::kLeft}
                                             }
      );
    }
  };
};

}