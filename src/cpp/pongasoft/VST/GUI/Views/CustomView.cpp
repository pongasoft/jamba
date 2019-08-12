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
#include <vstgui4/vstgui/uidescription/iviewcreator.h>
#include <vstgui4/vstgui/lib/cdrawcontext.h>
#include "CustomView.h"
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

///////////////////////////////////////////
// CustomView::CustomView
///////////////////////////////////////////
CustomView::CustomView(const CRect &iSize)
  : CView(iSize),
    GUIParamCxAware(),
    fTag{UNDEFINED_TAG_ID},
    fEditorMode{false},
    fBackColor{0, 0, 0}
{
}

//------------------------------------------------------------------------
// CustomView::draw
//------------------------------------------------------------------------
void CustomView::draw(CDrawContext *iContext)
{
  drawBackColor(iContext);

  setDirty(false);
}

//------------------------------------------------------------------------
// CustomView::drawBackColor
//------------------------------------------------------------------------
void CustomView::drawBackColor(CDrawContext *iContext)
{
  if(getBackColor().alpha != 0)
  {
    iContext->setFillColor(getBackColor());
    iContext->drawRect(getViewSize(), kDrawFilled);
  }
}

///////////////////////////////////////////
// CustomView::setBackColor
///////////////////////////////////////////
void CustomView::setBackColor(CColor const &iColor)
{
  // to force the redraw
  if(fBackColor != iColor)
  {
    fBackColor = iColor;
    drawStyleChanged();
  }
}

///////////////////////////////////////////
// CustomView::drawStyleChanged
///////////////////////////////////////////
void CustomView::drawStyleChanged()
{
  markDirty();
}

///////////////////////////////////////////
// CustomView::onParameterChange
///////////////////////////////////////////
void CustomView::onParameterChange(ParamID /* iParamID */)
{
  markDirty();
}

///////////////////////////////////////////
// CustomView::setEditorMode
///////////////////////////////////////////
void CustomView::setEditorMode(bool iEditorMode)
{
#if EDITOR_MODE
  if(fEditorMode != iEditorMode)
  {
    fEditorMode = iEditorMode;
    onEditorModeChanged();
  }
#endif

  // when not in editor mode, this does nothing...
}

///////////////////////////////////////////
// CustomView::getEditorMode
///////////////////////////////////////////
bool CustomView::getEditorMode() const
{
#if EDITOR_MODE
  return fEditorMode;
#else
  return false;
#endif
}

}
}
}
}