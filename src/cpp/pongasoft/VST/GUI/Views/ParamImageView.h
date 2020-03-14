/*
 * Copyright (c) 2020 pongasoft
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

#include "CustomControlView.h"

namespace pongasoft::VST::GUI::Views {

/**
 * This view renders a param value as an image (the `ParamDisplayView` view renders a param value as a string). The
 * image provided must be a filmstrip with each frame representing each value the param can have.
 *
 * As a result, this view works for any parameter (both Vst and Jmb) that is (or can be interpreted as) a
 * discrete parameter.
 *
 * In addition to the attributes exposed by `CustomDiscreteControlView`, this class exposes the following attributes:
 *
 * Attribute      | Description
 * ---------      | -----------
 * `image`        | @copydoc getImage()
 * `inverse`      | @copydoc getInverse()
 *
 * \note This view can be used to render status LEDs for example and all other kinds of graphical representation
 *       of a parameter value
 *
 * @see CustomDiscreteControlView for details on discrete parameters and the usage of `step-count`
 */
class ParamImageView : public CustomDiscreteControlView
{
public:
  explicit ParamImageView(const CRect &iSize) : CustomDiscreteControlView(iSize) {}

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  /**
   * The image to use. The image must be formatted as a filmstrip (similar to the various button views). This means
   * that there should be one frame for each value the parameter can have.
   *
   * For example for an on/off status LED, the parameter has 1 step (so 2 values) and as a result the image should
   * have 2 frames:
   *
   *   y                | frame
   *   -                | -----
   *   0                | the frame for off
   *   image height / 2 | the frame for on
   *
   *   Example: ![2 frames example](https://raw.githubusercontent.com/pongasoft/re-ab-switch/v1.2.0/GUI2D/Reason_GUI_front_root_cv_ledA_lamp.png)
   */
  BitmapPtr getImage() const { return fImage; }

  //! attribute `image`
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  //! Inverses the frames (last frame is first frame, etc...)
  bool getInverse() const { return fInverse; }

  //! Attribute `inverse`
  void setInverse(bool iInverse) { fInverse = iInverse; }

  /**
   * Returns the number of frames the image has (which is defined by the [number of steps + 1] the
   * underlying parameter has)
   */
  int32 getFrames() const;

  // registerParameters
  void registerParameters() override;

  //! Use the image and number of frames to size the view appropriately
  bool sizeToFit() override;

protected:
  BitmapSPtr fImage{nullptr};
  bool fInverse{false};

public:
  class Creator : public CustomViewCreator<ParamImageView, CustomDiscreteControlView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerBitmapAttribute("image", &ParamImageView::getImage, &ParamImageView::setImage);
      registerBooleanAttribute("inverse", &ParamImageView::getInverse, &ParamImageView::setInverse);
    }
  };
};

}
