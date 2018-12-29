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

#include <vstgui4/vstgui/lib/cdrawcontext.h>
#include "TextButtonView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// TextButtonView::registerParameters
//------------------------------------------------------------------------
void TextButtonView::registerParameters()
{
  if(!fParamCxMgr || getTitleTag() < 0)
    return;

  if(fTitle.exists())
    unregisterParam(fTitle.getParamID());

  auto paramID = static_cast<ParamID>(getTitleTag());

  fTitle =
    registerJmbCallback<UTF8String>(paramID, [this] {
      setTitle(fTitle);
    }, true); // make sure to invoke the callback when the param exists to set the title right away
}

//------------------------------------------------------------------------
// TextButtonView::setTitleTag
//------------------------------------------------------------------------
void TextButtonView::setTitleTag(int32_t iValue)
{
  fTitleTag = iValue;
  registerParameters();
}

//------------------------------------------------------------------------
// TextButtonView::valueChanged
//------------------------------------------------------------------------
void TextButtonView::valueChanged()
{
  CControl::valueChanged();

  if(getValue() == getMax())
    onClick();
}

//------------------------------------------------------------------------
// TextButtonView::onClick
//------------------------------------------------------------------------
void TextButtonView::onClick()
{
  if(fOnClickListener)
    fOnClickListener();
}

//------------------------------------------------------------------------
// TextButtonView::draw
// Implementation note: code copied from VSTGUI4 and changed to account
// for disabled state
//------------------------------------------------------------------------
void TextButtonView::draw(CDrawContext *context)
{
  bool highlight = value > 0.5;

  CGradient *bgGradient = getMouseEnabled() ?
                          (highlight ? getGradientHighlighted() : getGradient()) : getDisabledGradient();
  CColor const &textColor = getMouseEnabled() ?
                            (highlight ? getTextColorHighlighted() : getTextColor()) : getDisabledTextColor();

  auto lineWidth = getFrameWidth();
  if(lineWidth < 0.)
    lineWidth = context->getHairlineSize();
  context->setDrawMode(kAntiAliasing);
  context->setLineWidth(lineWidth);
  context->setLineStyle(CLineStyle(CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound));
  context->setFrameColor(highlight ? frameColorHighlighted : frameColor);
  CRect r(getViewSize());
  r.inset(lineWidth / 2., lineWidth / 2.);
  if(bgGradient)
  {
    CGraphicsPath *path = getPath(context, lineWidth);
    if(path)
    {
      context->fillLinearGradient(path, *bgGradient, r.getTopLeft(), r.getBottomLeft(), false);
      context->drawGraphicsPath(path, CDrawContext::kPathStroked);
    }
  }
  CRect titleRect = getViewSize();
  titleRect.inset(lineWidth / 2., lineWidth / 2.);

  CBitmap *iconToDraw = nullptr;
  if(!getMouseEnabled() && getDisabledBackground())
    iconToDraw = getDisabledBackground();
  else
    iconToDraw = highlight ? (iconHighlighted ? iconHighlighted : icon) : (icon ? icon : iconHighlighted);
  CDrawMethods::drawIconAndText(context, iconToDraw, iconPosition, getTextAlignment(), getTextMargin(), titleRect,
                                title, getFont(), textColor);
  setDirty(false);
}

//------------------------------------------------------------------------
// TextButtonView::setMouseEnabled
//------------------------------------------------------------------------
void TextButtonView::setMouseEnabled(bool iEnable)
{
  if(iEnable != getMouseEnabled())
  {
    CView::setMouseEnabled(iEnable);
    markDirty();
  }
}


}
}
}
}