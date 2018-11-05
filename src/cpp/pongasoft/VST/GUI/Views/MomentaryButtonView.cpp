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
#include "MomentaryButtonView.h"

#include <vstgui4/vstgui/lib/cdrawcontext.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

///////////////////////////////////////////
// MomentaryButtonView::draw
///////////////////////////////////////////
void MomentaryButtonView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  if(isOn())
    drawOn(iContext);
  else
    drawOff(iContext);

  setDirty(false);
}

///////////////////////////////////////////
// MomentaryButtonView::drawOn
///////////////////////////////////////////
void MomentaryButtonView::drawOn(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord y = fImage->getHeight() / 2;
    fImage->draw(iContext, getViewSize(), CPoint{0, y});
  }
  else
  {
    // no image => simply fill the surface with appropriate color (background and "on" color)
    // so that the button is fully functioning right away
    iContext->setFillColor(getOnColor());
    iContext->drawRect(getViewSize(), kDrawFilled);
  }
}

///////////////////////////////////////////
// MomentaryButtonView::drawOff
///////////////////////////////////////////
void MomentaryButtonView::drawOff(CDrawContext *iContext)
{
  if(fImage)
  {
    CCoord y = 0;
    fImage->draw(iContext, getViewSize(), CPoint{0, y});
  }

  // when there is no image the background color is drawn (already done)
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseDown
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(true);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseUp
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(false);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseCancel
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseCancel()
{
  setControlValue(false);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onKeyDown
///////////////////////////////////////////
int32_t MomentaryButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(true);
    setDirty(true);
    return 1;
  }
  return -1;
}

///////////////////////////////////////////
// MomentaryButtonView::onKeyUp
///////////////////////////////////////////
int32_t MomentaryButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(false);
    setDirty(true);
    return 1;
  }
  return -1;
}

///////////////////////////////////////////
// MomentaryButtonView::sizeToFit
///////////////////////////////////////////
bool MomentaryButtonView::sizeToFit()
{
  DLOG_F(INFO, "MomentaryButtonView::sizeToFit");
  return CustomView::sizeToFit(getImage(), 2);
}

}
}
}
}