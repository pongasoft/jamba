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
 * Extends the CTextButton view to:
 * - have a (potentially) dynamic title tied to a parameter (fTitle via fTitleTag)
 * - handle click conveniently (either inherit from this class and implement onClick, or provide an onClick listener)
 * - optionally handle image (3 states : disabled / off / on)
 */
class TextButtonView : public CustomViewAdapter<CTextButton>
{
public:
  using OnClickListener = std::function<void()>;

  // Constructor
  explicit TextButtonView(const CRect &iSize) : CustomViewAdapter(iSize) {}

  // get/set titleTag to (optionally) tie the title of this text button to a (Jmb) parameter
  virtual void setTitleTag(TagID iValue);
  TagID getTitleTag() const { return fTitleTag; };

  // get/set disabled text color (color to use when button is disabled)
  CColor const &getDisabledTextColor() const { return fDisabledTextColor; }
  virtual void setDisabledTextColor(CColor const &iColor) { fDisabledTextColor = iColor; };

  // get/set disabled gradient (gradient to use when button is disabled)
  GradientPtr getDisabledGradient() const { return fDisabledGradient; }
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
   * If you want to replace the entire drawing with an image you can provide a 'button-image'.
   *
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

  // get/set precision-override
  int32 getPrecisionOverride() const { return fPrecisionOverride; }
  void setPrecisionOverride(int32 iPrecisionOverride) { fPrecisionOverride = iPrecisionOverride; markDirty(); }

protected:
  /**
   * Called when the button should be drawn entirely (background + text + icon)
   */
  virtual void drawButtonText(CDrawContext *context);

  /**
   * Called when the button is simply rendered as an image (\see TextButtonView::getImage)
   */
  virtual void drawButtonImage(CDrawContext *context);

protected:
  // (optionally) tie the title of this text button to a (Jmb) parameter
  TagID fTitleTag{UNDEFINED_TAG_ID};

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

