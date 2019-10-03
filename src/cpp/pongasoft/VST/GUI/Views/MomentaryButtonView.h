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

#include <vstgui4/vstgui/lib/controls/ccontrol.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include "CustomControlView.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

/**
 * A momentary button is a button that lets you set the value of a parameter to its "on" value when
 * pressed, otherwise its "off" value.
 *
 * This view is designed to handle parameters which are backed by a `bool` representation but it works for any parameter
 * (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter.
 *
 * - `on-step` maps to the "on" value unless it is set to its default (-1) value in which case it maps to "stepCount"
 *   (should be `-1` or [0, stepCount])
 * - `off-step` maps to the "off" value unless it is set to its default (-1) value in which case it maps to `0`
 *   (should be `-1` or [0, stepCount])
 * - `inverse` inverses the meaning of "on" and "off" in regards to drawing the view/image
 *
 * Note that this view defines the "on" state as being the opposite of the "off" state and so the "off" state is the one
 * being checked against the off value (otherwise we could end up in a situation where both `isOn` and `isOff` are
 * `true`...). The consequence is that the underlying parameter might have a value that is neither the "off" value
 * nor the "on" value, but for the sake of this view it will be treated as "on" (similarly to C/C++ where `0` is
 * `false` and any non-zero is `true`).
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class MomentaryButtonView : public CustomDiscreteControlView
{
public:
  explicit MomentaryButtonView(const CRect &iSize) : CustomDiscreteControlView(iSize)
  {
    // off color is grey
    fBackColor = CColor{200,200,200};
  }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  // called to display the "on" state
  virtual void drawOn(CDrawContext *iContext);

  // called to display the "off" state
  virtual void drawOff(CDrawContext *iContext);

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

  // get/setOnColor (the off color is the back color...)
  CColor const &getOnColor() const { return fOnColor; }
  void setOnColor(CColor const &iColor) { fOnColor = iColor; }

  // get/setDisabledColor color for when disabled (mouse enabled set to false)
  CColor const &getDisabledColor() const { return fDisabledColor; }
  void setDisabledColor(CColor const &iColor) { fDisabledColor = iColor; }

  /**
   * If 'button-image-has-disabled-state' is selected then the image should contain the following
   * 3 frames (each is of size image height / 3):
   *   - at y = 0, the button in its disabled state
   *   - at y = 1 * image height / 3, the button in its off state
   *   - at y = 2 * image height / 3, the button in its on state
   *
   * If 'button-image-has-disabled-state' is NOT selected then the image should contain the following
   * 2 frames (each is of size image height / 2):
   *   - at y = 0, the button in its off state
   *   - at y = image height / 2, the button in its on state
   */
  BitmapPtr getImage() const { return fImage; }
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  /**
   * If the image contains a disabled state (3 frames) or not (2 frames) */
  bool getImageHasDisabledState() const { return fImageHasDisabledState; }
  void setImageHasDisabledState(bool iValue) { fImageHasDisabledState = iValue; }

  // get/setInverse (toggles which image is on and which is off)
  bool getInverse() const { return fInverse; }
  void setInverse(bool iInverse) { fInverse = iInverse; }

protected:
  int32 fOffStep{-1};
  int32 fOnStep{-1};

  CColor fOnColor{kRedCColor};
  CColor fDisabledColor{kBlackCColor};
  BitmapSPtr fImage{nullptr};
  bool fInverse{false};

  // whether the image has disabled state (3 frames) or not (2 frames)
  bool fImageHasDisabledState{false};

public:
  class Creator : public CustomViewCreator<MomentaryButtonView, CustomDiscreteControlView>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<int32>("on-step", &MomentaryButtonView::getOnStep, &MomentaryButtonView::setOnStep);
      registerIntegerAttribute<int32>("off-step", &MomentaryButtonView::getOffStep, &MomentaryButtonView::setOffStep);
      registerColorAttribute("on-color", &MomentaryButtonView::getOnColor, &MomentaryButtonView::setOnColor);
      registerColorAttribute("disabled-color", &MomentaryButtonView::getDisabledColor, &MomentaryButtonView::setDisabledColor);
      registerBitmapAttribute("button-image", &MomentaryButtonView::getImage, &MomentaryButtonView::setImage);
      registerBooleanAttribute("button-image-has-disabled-state", &MomentaryButtonView::getImageHasDisabledState, &MomentaryButtonView::setImageHasDisabledState);
      registerBooleanAttribute("inverse", &MomentaryButtonView::getInverse, &MomentaryButtonView::setInverse);
    }
  };
};

}