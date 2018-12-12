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

#include "ScrollbarView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

//------------------------------------------------------------------------
// ScrollbarView::draw
//------------------------------------------------------------------------
void ScrollbarView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  recompute();

  iContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

  if(showHandles())
  {
    drawLeftHandle(iContext);
    drawRightHandle(iContext);
  }

  drawScrollbar(iContext);
}

//------------------------------------------------------------------------
// ScrollbarView::drawLeftHandle
//------------------------------------------------------------------------
void ScrollbarView::drawLeftHandle(CDrawContext *iContext)
{
  iContext->setFillColor(fZoomHandlesColor);
  iContext->drawEllipse(fLeftHandleRect, kDrawFilled);
  iContext->drawRect(crect::offsetLeft(fLeftHandleRect, fLeftHandleRect.getWidth() / 2.0), kDrawFilled);
}

//------------------------------------------------------------------------
// ScrollbarView::drawRightHandle
//------------------------------------------------------------------------
void ScrollbarView::drawRightHandle(CDrawContext *iContext)
{
  iContext->setFillColor(fZoomHandlesColor);
  iContext->drawEllipse(fRightHandleRect, kDrawFilled);
  iContext->drawRect(crect::offsetRight(fRightHandleRect, -fRightHandleRect.getWidth() / 2.0), kDrawFilled);
}

//------------------------------------------------------------------------
// ScrollbarView::drawScrollbar
//------------------------------------------------------------------------
void ScrollbarView::drawScrollbar(CDrawContext *iContext)
{
  iContext->setFillColor(fScrollbarColor);
  iContext->drawRect(fScrollbarRect, kDrawFilled);
}

//------------------------------------------------------------------------
// ScrollbarView::computeZoomBox
//------------------------------------------------------------------------
ScrollbarView::ZoomBox ScrollbarView::computeZoomBox() const
{
  ZoomBox box{};

  box.fViewSize = fMargin.apply(getViewSize());

  auto h = box.fViewSize.getHeight();
  auto w = box.fViewSize.getWidth();

  box.fZoomHandlesSize = fZoomHandlesSize == -1 ? h : fZoomHandlesSize;

  auto minWidth = fScrollbarMinSize;

  if(minWidth == -1)
  {
    minWidth = h;

    if(showHandles())
      minWidth += 3.0 * box.fZoomHandlesSize;
  }

  if(minWidth > w)
    minWidth = w;

  box.fMinWidth = minWidth;
  box.fMaxWidth = w;
  box.fHalfWidth = box.computeWidth(getZoomPercent()) / 2.0;

  box.fMinCenter = 0 + box.fHalfWidth;
  box.fMaxCenter = w - box.fHalfWidth;
  box.fCenter = box.computeCenter(getOffsetPercent());

  return box;
}

//------------------------------------------------------------------------
// ScrollbarView::ZoomBox::stretch
//------------------------------------------------------------------------
bool ScrollbarView::ZoomBox::stretch(CCoord iDeltaX, bool iLeft)
{
  // nothing to do if no delta
  if(iDeltaX == 0)
    return false;

  CCoord newLeft;
  CCoord newRight;

  // which handle is being moved?
  if(iLeft)
  {
    // we try to move the left side of the box without touching the ride side (thus stretching the box)
    newLeft = getLeft() + iDeltaX;
    newRight = getRight();

    const auto minLeft = getMinLeft();
    if(newLeft < minLeft)
    {
      // we are at the limit, need to stretch the other side
      newRight += minLeft - newLeft;
      newLeft = minLeft;
    }
  }
  else
  {
    // we try to move the right side of the box without touching the left side (thus stretching the box)
    newRight = getRight() + iDeltaX;
    newLeft = getLeft();

    const auto maxRight = getMaxRight();
    if(newRight > maxRight)
    {
      // we are at the limit, need to stretch the other side
      newLeft -= newRight - maxRight;
      newRight = maxRight;
    }
  }

  // we recompute half width fist.
  auto newHalfWidth = Utils::clamp(newRight - newLeft, fMinWidth, fMaxWidth) / 2.0;
  if(newHalfWidth == fHalfWidth)
    return false;

  fHalfWidth = Utils::clamp(newRight - newLeft, fMinWidth, fMaxWidth) / 2.0;

  // then we can establish the new center
  fMinCenter = fHalfWidth;
  fMaxCenter = fMaxWidth - fHalfWidth;
  fCenter = Utils::clamp((newLeft + newRight) / 2.0, fMinCenter, fMaxCenter);

  return true;
}

//------------------------------------------------------------------------
// ScrollbarView::ZoomBox::maxZoom
//------------------------------------------------------------------------
void ScrollbarView::ZoomBox::maxZoom(RelativeCoord const &iNewCenter)
{
  fHalfWidth = fMinWidth / 2.0;

  // then we can establish the new center
  fMinCenter = fHalfWidth;
  fMaxCenter = fMaxWidth - fHalfWidth;
  fCenter = Utils::clamp(iNewCenter, fMinCenter, fMaxCenter);
}

//------------------------------------------------------------------------
// ScrollbarView::ZoomBox::minZoom
//------------------------------------------------------------------------
void ScrollbarView::ZoomBox::minZoom()
{
  fHalfWidth = fMaxWidth / 2.0;

  // then we can establish the new center
  fMinCenter = fHalfWidth;
  fMaxCenter = fHalfWidth;
  fCenter = fHalfWidth;
}

//------------------------------------------------------------------------
// ScrollbarView::recompute
//------------------------------------------------------------------------
void ScrollbarView::recompute()
{
  if(!fNeedsRecomputing)
    return;

  ZoomBox box = computeZoomBox();

  auto h = box.getHeight();

  RelativeView rv = box.getRelativeView();

  if(showHandles())
  {
    fLeftHandleRect = {box.getLeft(), 0, box.getLeft() + box.fZoomHandlesSize, h};
    fRightHandleRect = {box.getRight() - box.fZoomHandlesSize, 0, box.getRight() , h};
    fScrollbarRect = {fLeftHandleRect.right + fScrollbarGutterSpacing, 0, fRightHandleRect.left - fScrollbarGutterSpacing, h};

    fLeftHandleRect = rv.toAbsoluteRect(fLeftHandleRect);
    fRightHandleRect = rv.toAbsoluteRect(fRightHandleRect);
    fScrollbarRect = rv.toAbsoluteRect(fScrollbarRect);
  }
  else
  {
    fLeftHandleRect = {0, 0, 0, 0};
    fRightHandleRect = {0, 0, 0, 0};
    fScrollbarRect = rv.toAbsoluteRect({box.getLeft(), 0, box.getRight(), h});
  }

  fNeedsRecomputing = false;
}

//------------------------------------------------------------------------
// ScrollbarView::setViewSize
//------------------------------------------------------------------------
void ScrollbarView::setViewSize(const CRect &rect, bool invalid)
{
  needsRecomputing();
  CView::setViewSize(rect, invalid);
}

//------------------------------------------------------------------------
// ScrollbarView::getOffsetPercent
//------------------------------------------------------------------------
double ScrollbarView::getOffsetPercent() const
{
  if(fOffsetPercentParam.exists())
    return fOffsetPercentParam.getValue();
  else
    return fOffsetPercentValue;
}

//------------------------------------------------------------------------
// ScrollbarView::setOffsetPercent
//------------------------------------------------------------------------
void ScrollbarView::setOffsetPercent(double iOffsetPercent)
{
  if(fOffsetPercentEditor)
    fOffsetPercentEditor->setValue(iOffsetPercent);
  else
  {
    if(fOffsetPercentParam.exists())
      fOffsetPercentParam.setValue(iOffsetPercent);
    else
    {
      if(fOffsetPercentValue != iOffsetPercent)
      {
        fOffsetPercentValue = iOffsetPercent;
        needsRecomputing();
      }
    }
  }
}

//------------------------------------------------------------------------
// ScrollbarView::getZoomPercent
//------------------------------------------------------------------------
double ScrollbarView::getZoomPercent() const
{
  if(fZoomPercentParam.exists())
    return fZoomPercentParam.getValue();
  else
    return fZoomPercentValue;
}

//------------------------------------------------------------------------
// ScrollbarView::setZoomPercent
//------------------------------------------------------------------------
void ScrollbarView::setZoomPercent(double iZoomPercent)
{
  if(fZoomPercentEditor)
    fZoomPercentEditor->setValue(iZoomPercent);
  else
  {
    if(fZoomPercentParam.exists())
      fZoomPercentParam.setValue(iZoomPercent);
    else
    {
      if(fZoomPercentValue != iZoomPercent)
      {
        fZoomPercentValue = iZoomPercent;
        needsRecomputing();
      }
    }
  }
}

//------------------------------------------------------------------------
// ScrollbarView::registerParameters
//------------------------------------------------------------------------
void ScrollbarView::registerParameters()
{
  if(!fParamCxMgr)
    return; // not set yet

  if(__internal__registerRawVstControl(fOffsetPercentTag, fOffsetPercentValue, fOffsetPercentParam))
    needsRecomputing();

  if(__internal__registerRawVstControl(fZoomPercentTag, fZoomPercentValue, fZoomPercentParam))
    needsRecomputing();
}

//------------------------------------------------------------------------
// ScrollbarView::setOffsetPercentTag
//------------------------------------------------------------------------
void ScrollbarView::setOffsetPercentTag(int32_t offsetPercentTag)
{
  fOffsetPercentTag = offsetPercentTag;
  registerParameters();
}


//------------------------------------------------------------------------
// ScrollbarView::setZoomPercentTag
//------------------------------------------------------------------------
void ScrollbarView::setZoomPercentTag(int32_t zoomPercentTag)
{
  fZoomPercentTag = zoomPercentTag;
  registerParameters();
}

//------------------------------------------------------------------------
// ScrollbarView::onParameterChange
//------------------------------------------------------------------------
void ScrollbarView::onParameterChange(ParamID iParamID)
{
  if(iParamID == fZoomPercentTag || iParamID == fOffsetPercentTag)
    needsRecomputing();

  CustomView::onParameterChange(iParamID);
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseDown
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  RelativeView rv(fMargin.apply(getViewSize()));
  RelativeCoord x = rv.fromAbsolutePoint(where).x;

  // first we handle zoom via handles
  if(showHandles())
  {
    if(fLeftHandleRect.pointInside(where) || fRightHandleRect.pointInside(where))
    {
      if(fZoomPercentParam.exists())
        fZoomPercentEditor = fZoomPercentParam.edit();
      else
        fZoomPercentValueEditor = fZoomPercentValue;

      // when the size changes, it can also affect the offset
      if(fOffsetPercentParam.exists())
        fOffsetPercentEditor = fOffsetPercentParam.edit();
      else
        fOffsetPercentValueEditor = fOffsetPercentValue;

      fLeftDragHandle = fLeftHandleRect.pointInside(where);
      fDragGestureX = x;
      return kMouseEventHandled;
    }
  }

  auto box = computeZoomBox();

  if(box.isFull())
  {
    // when enabled, double clicking on the scrollbar when full will make it zoom in to maximum
    if(getEnableZoomDoubleClick() && buttons.isDoubleClick())
    {
      box.maxZoom(x);
      setOffsetPercent(box.computeOffsetPercent());
      setZoomPercent(box.computeZoomPercent());
    }
    // when the box is completely full we can't really act on it...
    return kMouseEventHandled;
  }

  if(x < box.getLeft())
  {
    // clicking on the left of the box => moves the box by its width on the left(like a "page")
    box.move(-box.getWidth());
  }
  else
  {
    if(x > box.getRight())
    {
      // clicking on the right of the box => moves the box by its width on the right (like a "page")
      box.move(box.getWidth());
    }
    else
    {
      // when enabled, double clicking on the scrollbar when not full will make it zoom in to minimum (make it full)
      if(getEnableZoomDoubleClick() && buttons.isDoubleClick())
      {
        box.minZoom();
        setOffsetPercent(box.computeOffsetPercent());
        setZoomPercent(box.computeZoomPercent());
        return kMouseEventHandled;
      }

      // the scrollbar itself was clicked => beginning of drag gesture...
      if(fOffsetPercentParam.exists())
        fOffsetPercentEditor = fOffsetPercentParam.edit();
      else
        fOffsetPercentValueEditor = fOffsetPercentValue;
      setOffsetPercent(box.computeOffsetPercent());
      fDragGestureX = x;
      return kMouseEventHandled;
    }
  }

  // if after move, the cursor is in the handle => allow for drag
  if(x >= box.getLeft() && x <= box.getRight())
  {
    // we try to make it the center (which makes it for a more natural movement)
    box.moveTo(x);

    // beginning of drag gesture...
    if(fOffsetPercentParam.exists())
      fOffsetPercentEditor = fOffsetPercentParam.edit();
    else
      fOffsetPercentValueEditor = fOffsetPercentValue;
    setOffsetPercent(box.computeOffsetPercent());
    fDragGestureX = x;
    return kMouseEventHandled;
  }

  // no drag gesture
  setOffsetPercent(box.computeOffsetPercent());

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseMoved
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseMoved(CPoint &where, const CButtonState &buttons)
{
  // are we dragging a handle aka zooming?
  if(fZoomPercentEditor || fZoomPercentValueEditor > -1.0)
  {
    RelativeView rv(fMargin.apply(getViewSize()));
    RelativeCoord x = rv.fromAbsolutePoint(where).x;

    auto deltaX = x - fDragGestureX;

    if(buttons.getModifierState() == CButton::kShift)
      deltaX *= getShiftDragFactor();

    auto box = computeZoomBox();
    if(box.stretch(deltaX, fLeftDragHandle))
    {
      setOffsetPercent(box.computeOffsetPercent());
      setZoomPercent(box.computeZoomPercent());
    }
    fDragGestureX = x;

    return kMouseEventHandled;
  }

  // are we dragging the scrollbar?
  if(fOffsetPercentEditor || fOffsetPercentValueEditor > -1.0)
  {
    RelativeView rv(fMargin.apply(getViewSize()));
    RelativeCoord x = rv.fromAbsolutePoint(where).x;

    auto deltaX = x - fDragGestureX;

    if(buttons.getModifierState() == CButton::kShift)
      deltaX *= getShiftDragFactor();

    auto box = computeZoomBox();
    box.move(deltaX);
    fDragGestureX = x;
    setOffsetPercent(box.computeOffsetPercent());

    return kMouseEventHandled;
  }

  return kMouseEventNotHandled;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  CMouseEventResult res = kMouseEventNotHandled;

  if(fOffsetPercentEditor)
  {
    fOffsetPercentEditor->commit();
    fOffsetPercentEditor = nullptr;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  if(fZoomPercentEditor)
  {
    fZoomPercentEditor->commit();
    fZoomPercentEditor = nullptr;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  if(fOffsetPercentValueEditor > -1.0)
  {
    fOffsetPercentValueEditor = -1.0;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  if(fZoomPercentValueEditor > -1.0)
  {
    fZoomPercentValueEditor = -1.0;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  return res;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseCancel()
{
  CMouseEventResult res = kMouseEventNotHandled;

  if(fOffsetPercentEditor)
  {
    fOffsetPercentEditor->rollback();
    fOffsetPercentEditor = nullptr;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  if(fZoomPercentEditor)
  {
    fZoomPercentEditor->rollback();
    fZoomPercentEditor = nullptr;
    fDragGestureX = -1;
    res = kMouseEventHandled;
  }

  if(fOffsetPercentValueEditor > -1.0)
  {
    fOffsetPercentValue = fOffsetPercentValueEditor;
    fOffsetPercentValueEditor = -1.0;
    fDragGestureX = -1;
    needsRecomputing();
    res = kMouseEventHandled;
  }

  if(fZoomPercentValueEditor > -1.0)
  {
    fZoomPercentValue = fZoomPercentValueEditor;
    fZoomPercentValueEditor = -1.0;
    fDragGestureX = -1;
    needsRecomputing();
    res = kMouseEventHandled;
  }

  return res;
}



}
}
}
}