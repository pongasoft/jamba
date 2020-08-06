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
#include "DrawContext.h"
#include <vstgui4/vstgui/lib/controls/ccontrol.h>
#include <vstgui4/vstgui/lib/controls/cparamdisplay.h>
#include <vstgui4/vstgui/lib/platform/iplatformfont.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

// TODO SDK.370 need to define enum to replace the one that is gone => temporarily using CParamDisplay::Style

//------------------------------------------------------------------------
// RelativeDrawContext::drawString
//------------------------------------------------------------------------
void RelativeDrawContext::drawString(UTF8String const &fText, RelativeRect const &fSize, StringDrawContext &sdc)
{
  if(!(sdc.fStyle & CParamDisplay::Style::kNoTextStyle))
  {
    fDrawContext->saveGlobalState();
    CRect textRect = toAbsoluteRect(fSize);
    textRect.inset(sdc.fTextInset.x, sdc.fTextInset.y);

    CRect oldClip;
    fDrawContext->getClipRect(oldClip);
    CRect newClip(textRect);
    newClip.bound(oldClip);
    fDrawContext->setClipRect(newClip);

    if(sdc.fAntialias)
      fDrawContext->setDrawMode(kAntiAliasing);
    fDrawContext->setFont(sdc.fFont);

    // draw darker text (as shadow)
    if(sdc.fStyle & CParamDisplay::Style::kShadowText)
    {
      CRect newSize(textRect);
      newSize.offset(sdc.fShadowTextOffset);
      fDrawContext->setFontColor(sdc.fShadowColor);
      fDrawContext->drawString(fText.getPlatformString(), newSize, sdc.fHorizTxtAlign, sdc.fAntialias);
    }
    fDrawContext->setFontColor(sdc.fFontColor);
    fDrawContext->drawString(fText.getPlatformString(), textRect, sdc.fHorizTxtAlign, sdc.fAntialias);
    fDrawContext->restoreGlobalState();
  }

}

#if EDITOR_MODE

//------------------------------------------------------------------------
// RelativeDrawContext::debugText
//------------------------------------------------------------------------
void RelativeDrawContext::debugText(RelativeCoord x, RelativeCoord y, char const *iText)
{
  StringDrawContext sdc{};
  sdc.fStyle |= CParamDisplay::Style::kShadowText;
  sdc.fHorizTxtAlign = kLeftText;
  sdc.fFont = fDebugStringFont;
  sdc.fFontColor = fDebugStringColor;
  sdc.fShadowColor = kBlackCColor;

  CCoord height = 20;

  if(sdc.fFont)
  {
    auto platformFont = sdc.fFont->getPlatformFont();
    if(platformFont && platformFont->getAscent() > 0 && platformFont->getDescent() > 0)
    {
      height = platformFont->getAscent() + platformFont->getDescent();
    }
  }

  if(x == -1)
  {
    sdc.fHorizTxtAlign = kRightText;
    x = 0;
  }

  RelativeRect rect;
  if(y == -1)
    rect = RelativeRect{x, getHeight() - height, getWidth(), getHeight()};
  else
    rect = RelativeRect{x, y, getWidth(), y + height};


//  fillRect(rect, fDebugBgColor);

  drawString(iText, rect, sdc);
}

#endif

}
}
}