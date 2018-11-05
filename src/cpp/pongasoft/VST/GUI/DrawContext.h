/*
 * Copyright (c) 2018 pongasoft
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

#include <vstgui4/vstgui/lib/cdrawcontext.h>
#include <vstgui4/vstgui/lib/cview.h>

#include "Types.h"

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

struct StringDrawContext
{
  CHoriTxtAlign fHorizTxtAlign{kCenterText};
  int32_t fStyle{0};
  FontPtr fFont{nullptr};
  CColor fFontColor{kWhiteCColor};
  CColor fShadowColor{kBlackCColor};
  CPoint fTextInset{0, 0};
  CPoint fShadowTextOffset{1., 1.};
  bool fAntialias{true};
};

using RelativeCoord = CCoord;
using AbsoluteCoord = CCoord;
using RelativePoint = CPoint;
using AbsolutePoint = CPoint;
using RelativeRect = CRect;
using AbsoluteRect = CRect;

class RelativeView
{
public:
  explicit RelativeView(CView const *iView) : fView{iView}
  {
    auto viewSize = fView->getViewSize();
    fOriginX = viewSize.left;
    fOriginY = viewSize.top;
  }

  inline RelativeRect getViewSize() const
  {
    return RelativeRect(0, 0, fView->getWidth(), fView->getHeight());
  }

  inline AbsoluteCoord toAbsoluteX(RelativeCoord x) const
  {
    return x + fOriginX;
  }

  inline AbsoluteCoord toAbsoluteY(RelativeCoord y) const
  {
    return y + fOriginY;
  }

  inline RelativeCoord fromAbsoluteX(AbsoluteCoord x) const
  {
    return x - fOriginX;
  }

  inline RelativeCoord fromAbsoluteY(AbsoluteCoord y) const
  {
    return y - fOriginY;
  }

  inline AbsolutePoint toAbsolutePoint(RelativePoint const &iPoint) const
  {
    return AbsolutePoint{toAbsoluteX(iPoint.x), toAbsoluteY(iPoint.y)};
  }

  inline RelativePoint fromAbsolutePoint(AbsolutePoint const &iPoint) const
  {
    return RelativePoint{fromAbsoluteX(iPoint.x), fromAbsoluteY(iPoint.y)};
  }

  inline AbsolutePoint toAbsolutePoint(RelativeCoord x, RelativeCoord y) const
  {
    return CPoint{x + fOriginX, y + fOriginY};
  }

  inline AbsoluteRect toAbsoluteRect(RelativeRect const &iRect) const
  {
    return CRect(toAbsolutePoint(iRect.getTopLeft()), iRect.getSize());
  }

protected:
  CView const *fView;
  CCoord fOriginX;
  CCoord fOriginY;
};

/**
 * Encapsulates the draw context provided by VSTGUI to reason in relative coordinates (0,0) is top,left
 */
class RelativeDrawContext : public RelativeView
{
public:
  RelativeDrawContext(CView *iView, CDrawContext *iDrawContext) : RelativeView{iView}, fDrawContext{iDrawContext}
  {
  }

  void drawLine(RelativeCoord x1, RelativeCoord y1, RelativeCoord x2, RelativeCoord y2, CColor const &color)
  {
    fDrawContext->setFrameColor(color);
    fDrawContext->drawLine(toAbsolutePoint(x1, y1), toAbsolutePoint(x2, y2));
  }

  void drawRect(RelativeCoord x1, RelativeCoord y1, RelativeCoord x2, RelativeCoord y2, CColor const &iStrokeColor)
  {
    drawRect(RelativeRect{x1, y1, x2, y2}, iStrokeColor);
  }

  void drawRect(RelativeRect const &iRect, CColor const &iStrokeColor)
  {
    fDrawContext->setFrameColor(iStrokeColor);
    fDrawContext->drawRect(toAbsoluteRect(iRect), kDrawStroked);
  }

  void fillRect(RelativeCoord x1, RelativeCoord y1, RelativeCoord x2, RelativeCoord y2, CColor const &iColor)
  {
    fillRect(RelativeRect{x1, y1, x2, y2}, iColor);
  }

  void fillRect(RelativeRect const &iRect, CColor const &iColor)
  {
    fDrawContext->setFillColor(iColor);
    fDrawContext->drawRect(toAbsoluteRect(iRect), kDrawFilled);
  }

  void fillAndStrokeRect(RelativeRect const &iRect, CColor const &iFillColor, CColor const &iStrokeColor)
  {
    fDrawContext->setFillColor(iFillColor);
    fDrawContext->setFrameColor(iStrokeColor);
    fDrawContext->drawRect(toAbsoluteRect(iRect), kDrawFilledAndStroked);
  }

  void drawString(UTF8String const &iText, RelativeCoord x, RelativeCoord y, RelativeCoord iHeight, StringDrawContext &iSdc)
  {
    CRect size{x, y, fDrawContext->getStringWidth(iText.getPlatformString()), iHeight};
    drawString(iText, size, iSdc);
  }

  void drawString(UTF8String const &iText, RelativeRect const &fSize, StringDrawContext &iSdc);

  void drawString(UTF8String const &iText, StringDrawContext &iSdc)
  {
    drawString(iText, getViewSize(), iSdc);
  }

protected:
  CDrawContext *fDrawContext;
};

}
}
}