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
 * A step pad lets you step through the values of a parameter by click dragging vertically or horizontally with
 * the mouse. This control behaves very similarly to a knob with the difference that the visual representation is
 * like a button (held or released).
 *
 * This view works for any parameter (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter.
 *
 * In addition to the attributes exposed by `CustomDiscreteControlView`, this class exposes the following attributes:
 *
 * Attribute                      | Description
 * ---------                      | -----------
 * `step-increment`               | @copydoc getStepIncrement()
 * `shift-step-increment`         | @copydoc getShiftStepIncrement()
 * `drag-factor`                  | @copydoc getDragFactor()
 * `shift-drag-factor`            | @copydoc getShiftDragFactor()
 * `held-color`                   | @copydoc getHeldColor()
 * `disabled-color`               | @copydoc getDisabledColor()
 * `pad-image`                    | @copydoc getImage()
 * `pad-image-has-disabled-state` | @copydoc getImageHasDisabledState()
 * `inverse`                      | @copydoc getInverse()
 * `wrap`                         | @copydoc getWrap()
 * `positive-direction`           | @copydoc getDirection()
 *
 * \note This control can be particularly useful to stack on top of a simple `ParamDisplayView` which offers no
 *       control on its own, thus adding behavior to the view. For example, the released image can be fully
 *       transparent, thus showing the `ParamDisplayView` and the held image semi-transparent so that the other
 *       view is still visible. Typical use case would be to provide a space efficient way to modify a parameter
 *       while showing its value (without requiring a knob or step buttons).
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class StepPadView : public CustomDiscreteControlView
{
public:
  /**
   * Represents the direction the mouse needs to be moved to register a change */
  enum class EPositiveDirection
  {
    kUp,
    kDown,
    kRight,
    kLeft,
  };

public:
  explicit StepPadView(const CRect &iSize) : CustomDiscreteControlView(iSize)
  {
  }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  // called to display the "held" pad state
  virtual void drawHeldPad(CDrawContext *iContext);

  // called to display the "released" pad state
  virtual void drawReleasedPad(CDrawContext *iContext);

  // input events (mouse/keyboard)
  CMouseEventResult onMouseDown(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseMoved(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseUp(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseCancel() override;

  // true if held
  bool isHeld() const { return fHeld; }

  /**
   * When no image is provided, the "released" color is the back color.
   *
   * @return Color to use when the pad is held and no image is provided
   */
  inline CColor const &getHeldColor() const { return fHeldColor; }
  //! attribute `held-color`
  void setHeldColor(CColor const &iColor) { fHeldColor = iColor; }

  /**
   * When no image is provided and the control is disabled, this color is used instead */
  CColor const &getDisabledColor() const { return fDisabledColor; }
  //! attribute `disabled-color`
  void setDisabledColor(CColor const &iColor) { fDisabledColor = iColor; }

  /**
   * The image to use to draw the pad.
   *
   * If `pad-image-has-disabled-state` is `true` then the image should contain the following
   * 3 frames (each is of size image height / 3):
   *   y                    | frame
   *   -                    | -----
   *   0                    | the pad in its disabled state
   *   1 * image height / 3 | the pad in its "released" state
   *   2 * image height / 3 | the pad in its "held" state
   *
   *   Example of a 3 frame image: ![3 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/action_crop.png)
   *
   * If `pad-image-has-disabled-state` is `false` then the image should contain the following
   * 2 frames (each is of size image height / 2):
   *   y                | frame
   *   -                | -----
   *   0                | the pad in its "released" state
   *   image height / 2 | the button in its "held" state
   *
   *   Example of a 2 frame image: ![2 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/bankC.png)
   */
  BitmapPtr getImage() const { return fImage; }

  //! Attribute `pad-image`.
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  /**
   * Flag to determine whether the image contains a disabled state (3 frames) or not (2 frames)
   *
   * @see `getImage()` for details on the impact of this flag on the image */
  bool getImageHasDisabledState() const { return fImageHasDisabledState; }
  //! Attribute `button-image`.
  void setImageHasDisabledState(bool iValue) { fImageHasDisabledState = iValue; }

  /**
   * Value by which this pad will increment (positive) or decrement (negative) the parameter. */
  int32 getStepIncrement() const { return fStepIncrement; }
  //! Attribute `step-increment`.
  void setStepIncrement(int32 iStepIncrement) { fStepIncrement = iStepIncrement; }

  /**
   * Value by which this pad will increment (positive) or decrement (negative) the parameter when the shift key
   * modified is being held. This allows to have bigger steps (or smaller steps) when shift is used.
   */
  int32 getShiftStepIncrement() const { return fShiftStepIncrement; }

  //! Attribute `shift-step-increment`.
  void setShiftStepIncrement(int32 iStepIncrement) { fShiftStepIncrement = iStepIncrement; }

  /**
   * Defines how many pixels the mouse needs to move to go through the range of values. The bigger this number
   * the "slower" the value will change.
   *
   * \note Due to `step-increment`, the size of the range is actually (number of steps / increment). For example
   *       with 50 steps and an increment of 5, this represent 10 values, so if `drag-factor` is set to its
   *       default (100 pixels), the mouse will have to move 10 pixels to result in a value change. */
  double getDragFactor() const { return fDragFactor; }
  //! Attribute `drag-factor`.
  void setDragFactor(double iDragFactor) { fDragFactor = iDragFactor; }

  /**
   * Defines how many pixels the mouse needs to move to go through the range of values when shift is being held.
   * The bigger this number the "slower" the value will change.
   *
   * @see `getDragFactor` for an example of the meaning of this value */
  double getShiftDragFactor() const { return fShiftDragFactor; }
  //! Attribute `shift-drag-factor`.
  void setShiftDragFactor(double iShiftDragFactor) { fShiftDragFactor = iShiftDragFactor; }

  /**
   * Defines what happens when the value reaches its end of range after being incremented (resp. decremented).
   * When set to `true` it will wrap around, otherwise it will remain at its max (resp. min) */
  inline bool getWrap() const { return fWrap; }
  //! Attribute `wrap`.
  void setWrap(bool iFlag) { fWrap = iFlag; }

  /**
   * Inverses the meaning of "held" and "released" in regards to drawing the view/image.
   *
   * \note This attribute does **not** affect the positive direction. Simply change the positive direction
   *       if you want to reverse the positive direction.
   */
  bool getInverse() const { return fInverse; }
  //! Attribute `inverse`.
  void setInverse(bool iInverse) { fInverse = iInverse; }

  /**
   * Defines the direction in which the mouse needs to be dragged in order to increase the value. The opposite
   * direction will decrease.
   *
   * - `up` means that when the mouse is clicked on the pad, then dragged up (vertically), the value of
   *   the underlying parameter will increase and if dragged down (it will decrease).
   * - `down` means that when the mouse is clicked on the pad, then dragged down (vertically), the value of
   *    the underlying parameter will increase and if dragged up (it will decrease).
   * - `right` means that when the mouse is clicked on the pad, then dragged right (horizontally), the value of
   *   the underlying parameter will increase and if dragged left (it will decrease).
   * - `left` means that when the mouse is clicked on the pad, then dragged left (horizontally), the value of
   *   the underlying parameter will increase and if dragged right (it will decrease).
   */
  EPositiveDirection getDirection() const { return fDirection; }
  //! Attribute `positive-direction`.
  void setDirection(EPositiveDirection iDirection) { fDirection = iDirection; }

protected:
  // registerParameters
  void registerParameters() override;

  /**
   * Compute mouse movement and determine the next value for the control */
  virtual CMouseEventResult setNextValue(CPoint &iWhere, const CButtonState &iButtons);

  // sizeToFit
  bool sizeToFit() override;

protected:
  /**
   * State of the pad (`true` for held, `false` for released) */
  bool fHeld{false};

  int32 fStepIncrement{1};
  int32 fShiftStepIncrement{1};
  double fDragFactor{100.0};
  double fShiftDragFactor{100.0};
  bool fWrap{false};

  CColor fHeldColor{kRedCColor};
  CColor fDisabledColor{kBlackCColor};
  BitmapSPtr fImage{nullptr};
  bool fInverse{false};
  EPositiveDirection fDirection{EPositiveDirection::kUp};

  // whether the image has disabled state (3 frames) or not (2 frames)
  bool fImageHasDisabledState{false};

  // the editor for handling parameter change
  std::unique_ptr<GUIOptionalParam<int32>::EditorType> fEditor{};
  CPoint fMousePosition{};

public:
  class Creator : public CustomViewCreator<StepPadView, CustomDiscreteControlView>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<int32>("step-increment", &StepPadView::getStepIncrement, &StepPadView::setStepIncrement);
      registerIntegerAttribute<int32>("shift-step-increment", &StepPadView::getShiftStepIncrement, &StepPadView::setShiftStepIncrement);
      registerDoubleAttribute("drag-factor", &StepPadView::getDragFactor, &StepPadView::setDragFactor);
      registerDoubleAttribute("shift-drag-factor", &StepPadView::getShiftDragFactor, &StepPadView::setShiftDragFactor);
      registerBooleanAttribute("wrap", &StepPadView::getWrap, &StepPadView::setWrap);
      registerColorAttribute("held-color", &StepPadView::getHeldColor, &StepPadView::setHeldColor);
      registerColorAttribute("disabled-color", &StepPadView::getDisabledColor, &StepPadView::setDisabledColor);
      registerBitmapAttribute("pad-image", &StepPadView::getImage, &StepPadView::setImage);
      registerBooleanAttribute("pad-image-has-disabled-state", &StepPadView::getImageHasDisabledState, &StepPadView::setImageHasDisabledState);
      registerBooleanAttribute("inverse", &StepPadView::getInverse, &StepPadView::setInverse);
      registerListAttribute<EPositiveDirection>("positive-direction",
                                                &StepPadView::getDirection, &StepPadView::setDirection,
                                                {
                                                  {"up",   EPositiveDirection::kUp},
                                                  {"down", EPositiveDirection::kDown},
                                                  {"right",   EPositiveDirection::kRight},
                                                  {"left",   EPositiveDirection::kLeft}
                                                }
      );
    }
  };
};

}