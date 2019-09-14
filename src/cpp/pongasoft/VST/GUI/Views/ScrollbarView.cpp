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

#include "ScrollbarView.h"
#include <pongasoft/VST/GUI/GUIUtils.h>

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
  iContext->drawRect(CRectUtils::offsetLeft(fLeftHandleRect, fLeftHandleRect.getWidth() / 2.0), kDrawFilled);
}

//------------------------------------------------------------------------
// ScrollbarView::drawRightHandle
//------------------------------------------------------------------------
void ScrollbarView::drawRightHandle(CDrawContext *iContext)
{
  iContext->setFillColor(fZoomHandlesColor);
  iContext->drawEllipse(fRightHandleRect, kDrawFilled);
  iContext->drawRect(CRectUtils::offsetRight(fRightHandleRect, -fRightHandleRect.getWidth() / 2.0), kDrawFilled);
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
bool ScrollbarView::ZoomBox::stretch(CCoord iDeltaX, DragType iDragType)
{
  // nothing to do if no delta
  if(iDeltaX == 0)
    return false;

  CCoord newLeft;
  CCoord newRight;

  switch(iDragType)
  {
    case DragType::kZoomLeft:
      // we move the left and right in opposite direction
      newLeft = getLeft() + iDeltaX;
      newRight = getRight() - iDeltaX;
      break;

    case DragType::kStretchLeft:
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
      break;
    }

    case DragType::kStretchRight:
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
      break;
    }

    case DragType::kZoomRight:
      // we move the left and right in opposite direction
      newRight = getRight() + iDeltaX;
      newLeft = getLeft() - iDeltaX;
      break;

    default:
      return false;
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

  fZoomBox = computeZoomBox();

  auto h = fZoomBox.getHeight();

  RelativeView rv = fZoomBox.getRelativeView();

  if(showHandles())
  {
    fLeftHandleRect = {fZoomBox.getLeft(), 0, fZoomBox.getLeft() + fZoomBox.fZoomHandlesSize, h};
    fRightHandleRect = {fZoomBox.getRight() - fZoomBox.fZoomHandlesSize, 0, fZoomBox.getRight() , h};
    fScrollbarRect = {fLeftHandleRect.right + fScrollbarGutterSpacing, 0, fRightHandleRect.left - fScrollbarGutterSpacing, h};

    fLeftHandleRect = rv.toAbsoluteRect(fLeftHandleRect);
    fRightHandleRect = rv.toAbsoluteRect(fRightHandleRect);
    fScrollbarRect = rv.toAbsoluteRect(fScrollbarRect);
  }
  else
  {
    fLeftHandleRect = {0, 0, 0, 0};
    fRightHandleRect = {0, 0, 0, 0};
    fScrollbarRect = rv.toAbsoluteRect({fZoomBox.getLeft(), 0, fZoomBox.getRight(), h});
  }

  fNeedsRecomputing = false;
}

//------------------------------------------------------------------------
// ScrollbarView::getScrollbarWidth
//------------------------------------------------------------------------
CCoord ScrollbarView::getScrollbarWidth() const
{
  if(showHandles())
  {
    auto scrollbarLeft = fZoomBox.getLeft() + fZoomBox.fZoomHandlesSize + fScrollbarGutterSpacing;
    auto scrollbarRight = fZoomBox.getRight() - fZoomBox.fZoomHandlesSize - fScrollbarGutterSpacing;
    return scrollbarRight - scrollbarLeft;
  }
  else
  {
    return fZoomBox.getWidth();
  }
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
  return fOffsetPercentParam.getValue();
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
    fOffsetPercentParam.update(iOffsetPercent);
  }
}

//------------------------------------------------------------------------
// ScrollbarView::getZoomPercent
//------------------------------------------------------------------------
double ScrollbarView::getZoomPercent() const
{
  return fZoomPercentParam.getValue();
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
    fZoomPercentParam.update(iZoomPercent);
  }
}

//------------------------------------------------------------------------
// ScrollbarView::registerParameters
//------------------------------------------------------------------------
void ScrollbarView::registerParameters()
{
  if(!fParamCxMgr)
    return; // not set yet

  registerOptionalParam(fOffsetPercentTag, fOffsetPercentParam);
  registerOptionalParam(fZoomPercentTag, fZoomPercentParam);
}

//------------------------------------------------------------------------
// ScrollbarView::setOffsetPercentTag
//------------------------------------------------------------------------
void ScrollbarView::setOffsetPercentTag(TagID offsetPercentTag)
{
  fOffsetPercentTag = offsetPercentTag;
}


//------------------------------------------------------------------------
// ScrollbarView::setZoomPercentTag
//------------------------------------------------------------------------
void ScrollbarView::setZoomPercentTag(TagID zoomPercentTag)
{
  fZoomPercentTag = zoomPercentTag;
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

  auto box = computeZoomBox();

  // first we handle zoom via handles
  if(showHandles())
  {
    if(fLeftHandleRect.pointInside(where) || fRightHandleRect.pointInside(where))
    {
      fZoomPercentEditor = fZoomPercentParam.edit();

      // when the size changes, it can also affect the offset
      fOffsetPercentEditor = fOffsetPercentParam.edit();

      if(fLeftHandleRect.pointInside(where))
        fDragType = box.isMaxRight() ? DragType::kStretchLeft : DragType::kZoomLeft;
      else
        fDragType = box.isMinLeft() ? DragType::kStretchRight : DragType::kZoomRight;
      fDragGestureX = x;
      return kMouseEventHandled;
    }
  }

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
    // clicking on the left of the box => moves the box by its width on the left (like a "page")
    if(buttons.getModifierState() == CButton::kShift)
      box.move(-1);
    else
      box.move(-getScrollbarWidth());
    setOffsetPercent(box.computeOffsetPercent());
    return kMouseEventHandled;
  }

  if(x > box.getRight())
  {
    // clicking on the right of the box => moves the box by its width on the right (like a "page")
    if(buttons.getModifierState() == CButton::kShift)
      box.move(1);
    else
      box.move(getScrollbarWidth());
    setOffsetPercent(box.computeOffsetPercent());
    return kMouseEventHandled;
  }

  // when enabled, double clicking on the scrollbar when not full will make it zoom in to minimum (make it full)
  if(getEnableZoomDoubleClick() && buttons.isDoubleClick())
  {
    box.minZoom();
    setOffsetPercent(box.computeOffsetPercent());
    setZoomPercent(box.computeZoomPercent());
    return kMouseEventHandled;
  }

  // the scrollbar itself was clicked => beginning of drag gesture...
  fZoomPercentEditor = fZoomPercentParam.edit();

  // when the size changes, it can also affect the offset
  fOffsetPercentEditor = fOffsetPercentParam.edit();

  fDragGestureX = x;
  fDragType = DragType::kScroll;

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseMoved
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseMoved(CPoint &where, const CButtonState &buttons)
{
  if(fDragType == DragType::kNone)
    return kMouseEventNotHandled;

  if(fDragType == DragType::kScroll)
  {
    RelativeView rv(fMargin.apply(getViewSize()));
    RelativeCoord x = rv.fromAbsolutePoint(where).x;

    auto deltaX = x - fDragGestureX;

    if(buttons.getModifierState() == CButton::kShift)
    {
      deltaX *= Utils::Range<Percent>{1.0, 0.0}.mapValue(getZoomPercent(), getShiftDragFactor());
    }

    auto box = computeZoomBox();
    box.move(deltaX);
    fDragGestureX = x;
    setOffsetPercent(box.computeOffsetPercent());
  }
  else
  {
    RelativeView rv(fMargin.apply(getViewSize()));
    RelativeCoord x = rv.fromAbsolutePoint(where).x;

    auto deltaX = x - fDragGestureX;

    if(buttons.getModifierState() == CButton::kShift)
    {
      deltaX *= Utils::Range<Percent>{1.0, 0.0}.mapValue(getZoomPercent(), getShiftDragFactor());
    }

    auto box = computeZoomBox();
    if(box.stretch(deltaX, fDragType))
    {
      setOffsetPercent(box.computeOffsetPercent());
      setZoomPercent(box.computeZoomPercent());
    }
    fDragGestureX = x;
  }

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseUp
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(fDragType == DragType::kNone)
    return kMouseEventNotHandled;

  fDragType = DragType::kNone;

  if(fOffsetPercentEditor)
  {
    fOffsetPercentEditor->commit();
    fOffsetPercentEditor = nullptr;
  }

  if(fZoomPercentEditor)
  {
    fZoomPercentEditor->commit();
    fZoomPercentEditor = nullptr;
  }

  return kMouseEventHandled;
}

//------------------------------------------------------------------------
// ScrollbarView::onMouseCancel
//------------------------------------------------------------------------
CMouseEventResult ScrollbarView::onMouseCancel()
{
  if(fDragType == DragType::kNone)
    return kMouseEventNotHandled;

  fDragType = DragType::kNone;

  if(fOffsetPercentEditor)
  {
    fOffsetPercentEditor->commit();
    fOffsetPercentEditor = nullptr;
  }

  if(fZoomPercentEditor)
  {
    fZoomPercentEditor->commit();
    fZoomPercentEditor = nullptr;
  }

  return kMouseEventHandled;
}



}
}
}
}