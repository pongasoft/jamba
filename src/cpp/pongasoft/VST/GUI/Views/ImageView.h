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

#include "CustomView.h"

namespace pongasoft::VST::GUI::Views {

/**
 * This view simply renders an image (in addition to the background color). This view can be used for adding a static
 * image like a logo, decal, sticker, etc...
 *
 * In addition to the attributes exposed by `CustomView`, this class exposes the following attributes:
 *
 * Attribute      | Description
 * ---------      | -----------
 * `image`        | @copydoc getImage()
 *
 * \note Although this view can be used during development to iron out the look and feel of the device, it is strongly
 *       recommended that once the design is locked down, a single background image be created with all the individual
 *       images composited in order to reduce the amount of memory and runtime computation required to render them.
 */
class ImageView : public CustomView
{
public:
  explicit ImageView(const CRect &iSize) : CustomView(iSize) { CView::setMouseEnabled(false); }

  /**
   * The image to use. Refers to a bitmap from the Bitmaps section of the editor (`bitmaps` section in the xml file). */
  BitmapPtr getImage() const { return fImage; }

  //! attribute `image`
  void setImage(BitmapPtr iImage) { fImage = iImage; }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

protected:
  BitmapSPtr fImage{nullptr};

public:
  class Creator : public CustomViewCreator<ImageView, CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerBitmapAttribute("image", &ImageView::getImage, &ImageView::setImage);
    }
  };

};

}
