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

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// TextButtonView::registerParameters
//------------------------------------------------------------------------
void TextButtonView::registerParameters()
{
  registerBaseCallback(getTitleTag(),
                       [this](IGUIParam &iParam) {
                         setTitle(iParam.toUTF8String(fPrecisionOverride));
                       }, true);
}

//------------------------------------------------------------------------
// TextButtonView::setTitleTag
//------------------------------------------------------------------------
void TextButtonView::setTitleTag(ParamID iValue)
{
  fTitleTag = iValue;
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
//------------------------------------------------------------------------
void TextButtonView::draw(CDrawContext *context)
{
  if(fImage)
    drawButtonImage(context);
  else
    drawButtonText(context);
}

//------------------------------------------------------------------------
// TextButtonView::drawButtonText
// Implementation note: code copied from VSTGUI4 and changed to account
// for disabled state
//------------------------------------------------------------------------
void TextButtonView::drawButtonText(CDrawContext *context)
{
  bool highlight = BooleanParamConverter::toBoolean(value);

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
}

//------------------------------------------------------------------------
// TextButtonView::drawButtonImage
//------------------------------------------------------------------------
void TextButtonView::drawButtonImage(CDrawContext *iContext)
{
  CCoord frameHeight;
  int frameIndex;

  bool onState = BooleanParamConverter::toBoolean(value);

  if(fImageHasDisabledState)
  {
    frameHeight = fImage->getHeight() / 3;
    frameIndex = getMouseEnabled() ? (onState ? 2 : 1) : 0;
  }
  else
  {
    frameHeight = fImage->getHeight() / 2;
    frameIndex = onState ? 1 : 0;
  }

  fImage->draw(iContext, getViewSize(), CPoint{0, frameIndex * frameHeight});
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

//------------------------------------------------------------------------
// TextButtonView::click
//------------------------------------------------------------------------
void TextButtonView::click()
{
  setValue(getMax());
}

//------------------------------------------------------------------------
// TextButtonView::unClick
//------------------------------------------------------------------------
void TextButtonView::unClick()
{
  setValue(getMin());
}

}