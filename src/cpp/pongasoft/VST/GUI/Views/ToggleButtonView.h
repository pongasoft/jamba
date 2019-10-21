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
#pragma once

#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include "CustomControlView.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * A toggle button is a button that lets you toggle the value of a parameter between an "on" value and an "off" value.
 *
 * This view is designed to handle parameters which are backed by a `bool` representation but it works for any parameter
 * (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter.
 *
 * @note This view defines the "on" state as being the opposite of the "off" state and so the "off" state is the one
 *       being checked against the off value (otherwise we could end up in a situation where both `isOn` and `isOff` are
 *       `true`...). The consequence is that the underlying parameter might have a value that is neither the "off" value
 *       nor the "on" value, but for the sake of this view it will be treated as "on" (similarly to C/C++ where `0` is
 *       `false` and any non-zero is `true`).
 *
 * In addition to the attributes exposed by `CustomDiscreteControlView`, this class exposes the following attributes:
 *
 * Attribute | Description | More
 * --------- | ----------- | ----
 * `on-step` |  maps to the "on" value unless it is set to its default (-1) value in which case it maps to `getStepCount()` (should be `-1` or `[0, stepCount]`) | `getOnStep()`
 * `off-step` | maps to the "off" value unless it is set to its default (-1) value in which case it maps to `0` (should be `-1` or `[0, stepCount]`) | `getOffStep()`
 * `on-color` | when no image is provided, `back-color` is used for the "off" state and `on-color` for the "on" state (draws a rectangle with this color) | `getOnColor()`
 * `frames` | the number of frames the image contains (see `getImage()` for details) | `getFrames()`
 * `button-image` | the image to use to draw the button (see `getImage()` for details on the content of the image) | `getImage()`
 * `inverse` | inverses the meaning of "on" and "off" in regards to drawing the view/image | `getInverse()`
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class ToggleButtonView : public CustomDiscreteControlView
{
public:
  explicit ToggleButtonView(const CRect &iSize) : CustomDiscreteControlView(iSize)
  {
    // off color is grey
    fBackColor = CColor{200, 200, 200};
  }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  // input events (mouse/keyboard)
  CMouseEventResult onMouseDown(CPoint &where, const CButtonState &buttons) override;

  CMouseEventResult onMouseUp(CPoint &where, const CButtonState &buttons) override;

  CMouseEventResult onMouseCancel() override;

  int32_t onKeyDown(VstKeyCode &keyCode) override;

  int32_t onKeyUp(VstKeyCode &keyCode) override;

  // sizeToFit
  bool sizeToFit() override;

  /**
   * Attribute `off-step`
   */
  int32 getOffStep() const { return fOffStep; }
  void setOffStep(int32 iStep) { fOffStep = iStep; markDirty(); }
  int32 getComputedOffStep() const { return std::max(Utils::ZERO_INT32, getOffStep()); }

  /**
   * Attribute `on-step`
   */
  int32 getOnStep() const { return fOnStep; }
  void setOnStep(int32 iStep) { fOnStep = iStep; markDirty(); }
  int32 getComputedOnStep() const;

  // is on or off
  bool isOff() const { return getControlValue() == getComputedOffStep(); }
  bool isOn() const { return !isOff(); }

  /**
   * Toggles between on and off control value.
   *
   * @return the new control value
   */
  int32 toggleControlValue();

  // get/set frames (should be either 2 or 4) 4 includes the pressed state
  int getFrames() const { return fFrames; }

  void setFrames(int iFrames);

  // get/setOnColor (the off color is the back color...)
  CColor const &getOnColor() const { return fOnColor; }

  void setOnColor(CColor const &iColor) { fOnColor = iColor; }

  // get/setInverse (toggles which image is on and which is off)
  bool getInverse() const { return fInverse; }

  void setInverse(bool iInverse) { fInverse = iInverse; }

  /**
   * Attribute `button-image`.
   *
   * The content of the image depends on the attribute `frames` (`getFrames()`) with the following convention:
   *
   * - for 2 frames each is of size image height / 2:
   *   y | frame
   *   - | -----
   *   0 | the button in its "off" state
   *   image height / 2 | the button in its "on" state
   *
   *   Example: ![2 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/bankC.png)
   *
   * - for 4 frames each is of size image height / 4:
   *   y | frame
   *   - | -----
   *   0 | the button in its "off" state
   *   image height * 1/4 | the button in its "off" state depressed
   *   image height * 2/4 | the button in its "on" state
   *   image height * 3/4 | the button in its "on" state depressed
   *
   *   Example: ![4 frames example](https://raw.githubusercontent.com/pongasoft/vst-vac-6v/v1.2.0/resource/Button_Live_4frames.png)
   */
  BitmapPtr getImage() const { return fImage; }

  //! Attribute `button-image`.
  void setImage(BitmapPtr iImage) { fImage = iImage; }

protected:
  int32 fOffStep{-1};
  int32 fOnStep{-1};

  int fFrames{4};
  CColor fOnColor{kRedCColor};
  BitmapSPtr fImage{nullptr};
  bool fInverse{false};

  bool fPressed{false};

public:
  class Creator : public CustomViewCreator<ToggleButtonView, CustomDiscreteControlView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<int32>("on-step", &ToggleButtonView::getOnStep, &ToggleButtonView::setOnStep);
      registerIntegerAttribute<int32>("off-step", &ToggleButtonView::getOffStep, &ToggleButtonView::setOffStep);
      registerIntAttribute("frames", &ToggleButtonView::getFrames, &ToggleButtonView::setFrames);
      registerColorAttribute("on-color", &ToggleButtonView::getOnColor, &ToggleButtonView::setOnColor);
      registerBitmapAttribute("button-image", &ToggleButtonView::getImage, &ToggleButtonView::setImage);
      registerBooleanAttribute("inverse", &ToggleButtonView::getInverse, &ToggleButtonView::setInverse);
    }
  };
};

}