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

#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include "CustomControlView.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * A discrete button behaves like a toggle button except that it is `on` only if the control value (tied to a parameter)
 * is equal to the `step` value (`DiscreteButtonView::getStep()` / `step` property in the xml).
 * Selecting the button will set the underlying parameter to `step`. This button can be used to implement a radio group
 * button behavior (only one selected at a time).
 *
 * This view works for any parameter (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter.
 *
 * In addition to the attributes exposed by `CustomDiscreteControlView`, this class exposes the following attributes:
 *
 * Attribute | Description | More
 * --------- | ----------- | ----
 * `frames` | the number of frames the image contains (see `getImage()` for details) | `getFrames()`
 * `on-color` | when no image is provided, `back-color` is used for the "off" state and `on-color` for the "on" state (draws a rectangle with this color) | `getOnColor()`
 * `button-image` | the image to use to draw the button (see `getImage()` for details on the content of the image) | `getImage()`
 * `inverse` | inverses the meaning of "on" and "off" in regards to drawing the view/image | `getInverse()`
 * `step` | the value used to check whether the button is "on" or "off" as well as the value to set the parameter to when the button is selected | `getStep()`
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class DiscreteButtonView : public CustomDiscreteControlView
{
public:
  explicit DiscreteButtonView(const CRect &iSize) : CustomDiscreteControlView(iSize)
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

  // is on or off
  bool isOn() const;

  bool isOff() const { return !isOn(); }

  void setOn();

  //! Attribute `frames`. Should be either 2 or 4 (4 includes the pressed state).
  int getFrames() const { return fFrames; }

  void setFrames(int iFrames);

  // get/setOnColor (the off color is the back color...)
  CColor const &getOnColor() const { return fOnColor; }

  void setOnColor(CColor const &iColor) { fOnColor = iColor; }

  // get/setInverse (toggles which image is on and which is off)
  bool getInverse() const { return fInverse; }

  void setInverse(bool iInverse) { fInverse = iInverse; }

  int32 getStep() const { return fStep; }
  void setStep(int32 step);

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
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  void registerParameters() override;

protected:
  int fFrames{4};
  CColor fOnColor{kRedCColor};
  BitmapSPtr fImage{nullptr};
  bool fInverse{false};

  bool fPressed{false};

  int32 fStep{0};

public:
  class Creator : public CustomViewCreator<DiscreteButtonView, CustomDiscreteControlView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntAttribute("frames", &DiscreteButtonView::getFrames, &DiscreteButtonView::setFrames);
      registerColorAttribute("on-color", &DiscreteButtonView::getOnColor, &DiscreteButtonView::setOnColor);
      registerBitmapAttribute("button-image", &DiscreteButtonView::getImage, &DiscreteButtonView::setImage);
      registerBooleanAttribute("inverse", &DiscreteButtonView::getInverse, &DiscreteButtonView::setInverse);
      registerIntegerAttribute<int32>("step", &DiscreteButtonView::getStep, &DiscreteButtonView::setStep);
    }
  };
};

}