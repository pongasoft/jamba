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

#include <vstgui4/vstgui/lib/controls/cbuttons.h>
#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * Extension of the `CTextButton` view to add more functionality.
 *
 * In addition to the behavior provided by `CTextButton`, this class adds the following functionality:
 * - have a (potentially) dynamic title tied to a parameter (via `title-tag` / `TextButtonView::fTitleTag`)
 * - handle click conveniently (either inherit from this class and implement `onClick()`, or provide an `onClick` listener)
 * - optionally handle image (3 states : disabled / off / on)
 *
 * In addition to the attributes exposed by `CTextButton`, this class exposes the following attributes:
 *
 * Attribute                         | Description
 * ---------                         | -----------
 * `title-tag`                       | @copydoc getTitleTag()
 * `disabled-text-color`             | @copydoc getDisabledTextColor()
 * `disabled-gradient`               | @copydoc getDisabledGradient()
 * `button-image`                    | @copydoc getImage()
 * `button-image-has-disabled-state` | @copydoc getImageHasDisabledState()
 * `precision-override`              | @copydoc getPrecisionOverride()
 */
class TextButtonView : public CustomViewAdapter<CTextButton>
{
public:
  using OnClickListener = std::function<void()>;

  // Constructor
  explicit TextButtonView(const CRect &iSize) : CustomViewAdapter(iSize) {}

  //! Attribute `title-tag`.
  virtual void setTitleTag(ParamID iValue);

  /**
   * Optional id of the parameter whose string representation (`IGUIParameter::toUTF8String()`) will be used
   * as the title for this button. */
  ParamID getTitleTag() const { return fTitleTag; };

  //! Color used to draw the title when no image is provided and the button is disabled.
  CColor const &getDisabledTextColor() const { return fDisabledTextColor; }

  //! Attribute `disabled-text-color`.
  virtual void setDisabledTextColor(CColor const &iColor) { fDisabledTextColor = iColor; };

  //! Gradient used to draw the button when no image is provided and the button is disabled.
  GradientPtr getDisabledGradient() const { return fDisabledGradient; }

  //! Attribute `disabled-gradient`.
  virtual void setDisabledGradient(GradientPtr iGradient) { fDisabledGradient = iGradient; };

  void setMouseEnabled(bool bEnable) override;

  // draw => overridden to handle disabled state
  void draw(CDrawContext *context) override;

  // registerParameters
  void registerParameters() override;

  // handles value change
  void valueChanged() override;

  // call to programmatically click the button
  virtual void click();

  // call to programmatically unClick the button
  virtual void unClick();

  // callback when the button is clicked
  virtual void onClick();

  // setOnClickListener
  void setOnClickListener(OnClickListener iListener) { fOnClickListener = std::move(iListener); }

  /**
   * The image for the button.
   *
   * If `button-image-has-disabled-state` is `true` then the image should contain the following
   * 3 frames (each is of size image height / 3):
   *   y | frame
   *   - | -----
   *   0 | the button in its disabled state
   *   1 * image height / 3 | the button in its "off" state
   *   2 * image height / 3 | the button in its "on" state
   *
   *   Example: ![3 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/action_crop.png)
   *
   * If `button-image-has-disabled-state` is `false` then the image should contain the following
   * 2 frames (each is of size image height / 2):
   *   y | frame
   *   - | -----
   *   0 | the button in its "off" state
   *   image height / 2 | the button in its "on" state
   *
   *   Example: ![2 frames example](https://raw.githubusercontent.com/pongasoft/vst-sam-spl-64/v1.0.0/resource/bankC.png)
   */
  BitmapPtr getImage() const { return fImage; }
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  /**
   * Used when drawing the image. If `true`, the image contains a disabled state (3 frames) otherwise it doesn't (2 frames).
   *
   * @see getImage() for details on how the frames are laid out in the image */
  bool getImageHasDisabledState() const { return fImageHasDisabledState; }
  void setImageHasDisabledState(bool iValue) { fImageHasDisabledState = iValue; }

  /**
   * Allow to override the precision of the parameter.
   *
   * If set to its default (-1), lets the parameter determine what the precision is (when displaying numbers),
   * but if set `>= 0` it will use the attribute value instead.
   *
   * @see IGUIParameter::toUTF8String()
   */
  int32 getPrecisionOverride() const { return fPrecisionOverride; }
  void setPrecisionOverride(int32 iPrecisionOverride) { fPrecisionOverride = iPrecisionOverride; markDirty(); }

protected:
  /**
   * Called when the button should be drawn entirely (background + text + icon)
   */
  virtual void drawButtonText(CDrawContext *context);

  /**
   * Called when the button is simply rendered as an image.
   *
   * @see getImage()
   */
  virtual void drawButtonImage(CDrawContext *context);

protected:
  // (optionally) tie the title of this text button to a (Jmb) parameter
  ParamID fTitleTag{UNDEFINED_PARAM_ID};

  // Which color and gradient to use when disabled
  CColor fDisabledTextColor;
  GradientSPtr fDisabledGradient;

  // the underlying parameter => use its string representation to display the text of the button
  int32 fPrecisionOverride{-1};

  // the image to use (in place of text)
  BitmapSPtr fImage{nullptr};

  // whether the image has disabled state (3 frames) or not (2 frames)
  bool fImageHasDisabledState{false};

  // the onclick listener
  OnClickListener fOnClickListener{};

public:
  class Creator : public CustomViewCreator<TextButtonView, CustomViewAdapter<CTextButton>>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCTextButton)
    {
      registerTagAttribute("title-tag", &TextButtonView::getTitleTag, &TextButtonView::setTitleTag);
      registerColorAttribute("disabled-text-color", &TextButtonView::getDisabledTextColor, &TextButtonView::setDisabledTextColor);
      registerGradientAttribute("disabled-gradient", &TextButtonView::getDisabledGradient, &TextButtonView::setDisabledGradient);
      registerBitmapAttribute("button-image", &TextButtonView::getImage, &TextButtonView::setImage);
      registerBooleanAttribute("button-image-has-disabled-state", &TextButtonView::getImageHasDisabledState, &TextButtonView::setImageHasDisabledState);
      registerIntegerAttribute<int32>("precision-override", &TextButtonView::getPrecisionOverride, &TextButtonView::setPrecisionOverride);
    }
  };
};

}

