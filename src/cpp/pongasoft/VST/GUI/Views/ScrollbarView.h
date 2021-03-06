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
#pragma once

#include "CustomView.h"
#include <pongasoft/VST/GUI/DrawContext.h>
#include <pongasoft/Utils/Lerp.h>

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

/**
 * Generic scrollbar which handles scrolling and (optional) zooming via handles. The scrollbar is driven by 2
 * parameters which can be tied to Vst or Jmb parameters:
 * - `offsetPercent` which represents the position of the scrollbar as a percent (0 means completely left, 1 means
 *   completely right)
 * - `zoomPercent` which represents the size of the scrollbar as a percent (0 means completely zoomed out (hence the
 *   scrollbar is full), 1 means completely zoomed in (hence the scrollbar is at its minimum size)).
 * At the moment, it handles only horizontal scrollbar.
 *
 * In addition to the attributes exposed by `CustomView`, this class exposes the following attributes:
 *
 * Attribute                  | Description
 * ---------                  | -----------
 * `offset-percent-tag`       | @copydoc getOffsetPercentTag()
 * `zoom-percent-tag`         | @copydoc getZoomPercentTag()
 * `margin`                   | @copydoc getMargin()
 * `scrollbar-color`          | @copydoc getScrollbarColor()
 * `scrollbar-min-size`       | @copydoc getScrollbarMinSize()
 * `scrollbar-gutter-spacing` | @copydoc getScrollbarGutterSpacing()
 * `zoom-handles-color`       | @copydoc getZoomHandlesColor()
 * `zoom-handles-size`        | @copydoc getZoomHandlesSize()
 * `shift-drag-factor`        | @copydoc getShiftDragFactor()
 * `enable-zoom-double-click` | @copydoc getEnableZoomDoubleClick()
 */
class ScrollbarView : public CustomView
{
public:
  // Constructor
  explicit ScrollbarView(const CRect &iSize) : CustomView(iSize) {}

  // draw
  void draw(CDrawContext *iContext) override;

  //! id for the parameter tied to offset percent
  ParamID getOffsetPercentTag() const { return fOffsetPercentTag; }
  void setOffsetPercentTag(ParamID offsetPercentTag);

  //! id for the parameter tied to zoom percent
  ParamID getZoomPercentTag() const { return fZoomPercentTag; }
  void setZoomPercentTag(ParamID zoomPercentTag);

  //! Amount of space (in pixels) to draw around the full scrollbar (includes handles)
  Margin const &getMargin() const { return fMargin; }
  void setMargin(Margin  const &iMargin) { fMargin = iMargin; needsRecomputing();}

  //! the color of the scrollbar itself (the rectangle)
  const CColor &getScrollbarColor() const { return fScrollbarColor; }
  void setScrollbarColor(const CColor &iColor) { fScrollbarColor = iColor; needsRecomputing(); }

  //! Minimum size for the scrollbar (in pixels). If -1 it will be auto-computed.
  CCoord getScrollbarMinSize() const { return fScrollbarMinSize; }
  void setScrollbarMinSize(CCoord iScrollbarMinSize) { fScrollbarMinSize = iScrollbarMinSize; needsRecomputing();}

  //! Spacing (in pixels) between the scrollbar and zoom handles (only drawn when zoom handles)
  CCoord getScrollbarGutterSpacing() const { return fScrollbarGutterSpacing; }
  void setScrollbarGutterSpacing(CCoord iScrollbarGutterSpacing) { fScrollbarGutterSpacing = iScrollbarGutterSpacing; needsRecomputing();}

  // showHandles
  bool showHandles() const { return fZoomHandlesSize != 0; }

  //! Zoom handles color
  const CColor &getZoomHandlesColor() const { return fZoomHandlesColor; }
  void setZoomHandlesColor(const CColor &iColor) { fZoomHandlesColor = iColor; needsRecomputing(); }

  //! Zoom handles size.  If -1 it will be auto-computed. Set to 0 to disable handles entirely.
  CCoord getZoomHandlesSize() const { return fZoomHandlesSize; }
  void setZoomHandlesSize(CCoord iSize) { fZoomHandlesSize = iSize; needsRecomputing(); }

  /**
   * Defines how much to slow down (if less than 1) or accelerate (if more than 1) when shift is held when dragging.
   *
   * @note This attribute defines a range (which can be a degenerate range when both `from` and `to` are the same)
   *       in the event you want the factor to vary depending on how zoomed the scrollbar is.
   *       For example: `shift-drag-factor="0.01,0.5"`
   */
  Range const &getShiftDragFactor() const { return fShiftDragFactor; }

  //! Attribute `shift-drag-factor`.
  void setShiftDragFactor(Range const &iShiftDragFactor) { fShiftDragFactor = iShiftDragFactor; }

  //! `true` to allow zooming on double click on the scrollbar
  bool getEnableZoomDoubleClick() const { return fEnableZoomDoubleClick; }
  void setEnableZoomDoubleClick(bool iEnableZoomDoubleClick) { fEnableZoomDoubleClick = iEnableZoomDoubleClick; }

  // offsetPercent accessor
  double getOffsetPercent() const;
  void setOffsetPercent(double iOffsetPercent);

  // zoomPercent accessor
  double getZoomPercent() const;
  void setZoomPercent(double iZoomPercent);

  CLASS_METHODS_NOCOPY(ScrollbarView, CustomView)

protected:
  // setViewSize
  void setViewSize(const CRect &rect, bool invalid) override;

  // registerParameters
  void registerParameters() override;

  // onParameterChange
  void onParameterChange(ParamID iParamID) override;

  // onMouseDown
  CMouseEventResult onMouseDown(CPoint &where, const CButtonState &buttons) override;

  // onMouseMoved
  CMouseEventResult onMouseMoved(CPoint &where, const CButtonState &buttons) override;

  // onMouseUp
  CMouseEventResult onMouseUp(CPoint &where, const CButtonState &buttons) override;

  // onMouseCancel
  CMouseEventResult onMouseCancel() override;

protected:
  enum class DragType { kNone, kScroll, kZoomLeft, kZoomRight, kStretchLeft, kStretchRight };

  /**
   * Represents the box (dimension and position point of view) containing the zoom handles and scrollbar
   */
  struct ZoomBox
  {
    CRect fViewSize;
    RelativeCoord fMinCenter;
    RelativeCoord fMaxCenter;
    RelativeCoord fCenter;
    CCoord fMinWidth;
    CCoord fMaxWidth;
    CCoord fHalfWidth;
    CCoord fZoomHandlesSize;

    // when the box is full it cannot be moved anymore
    bool isFull() const { return fMinCenter == fMaxCenter; }

    // left position
    RelativeCoord getLeft() const { return fCenter - fHalfWidth; }
    RelativeCoord getMinLeft() const { return fMinCenter - fHalfWidth; }
    bool isMinLeft() const { return fCenter == fMinCenter; }

    // right position
    RelativeCoord getRight() const { return fCenter + fHalfWidth; }
    RelativeCoord getMaxRight() const { return fMaxCenter + fHalfWidth; }
    bool isMaxRight() const { return fCenter == fMaxCenter; }

    // height of the box
    CCoord getHeight() const { return fViewSize.getHeight(); }

    // width of the box
    CCoord getWidth() const { return fHalfWidth * 2.0; }

    // the relative view containing the box (which is the original view minus the margin)
    RelativeView getRelativeView() const { return RelativeView{fViewSize}; }

    // compute position from offsetPercent
    RelativeCoord computeCenter(double iOffsetPercent) const
    {
      return Utils::mapValueDP(iOffsetPercent, 0.0, 1.0, fMinCenter, fMaxCenter);
    }

    // compute the offsetPercent from the position
    double computeOffsetPercent() const
    {
      return Utils::mapValueDP(fCenter, fMinCenter, fMaxCenter, 0.0, 1.0);
    }

    // compute size from zoomPercent
    CCoord computeWidth(double iZoomPercent) const
    {
      return Utils::mapValueDP(iZoomPercent, 1.0, 0.0, fMinWidth, fMaxWidth);
    }

    // compute zoomPercent from size
    double computeZoomPercent() const
    {
      return Utils::mapValueDP(getWidth(), fMinWidth, fMaxWidth, 1.0, 0.0);
    }

    // move the box by iDeltaX (can be positive or negative)
    void move(CCoord iDeltaX)
    {
      fCenter = Utils::clamp(fCenter + iDeltaX, fMinCenter, fMaxCenter);
    }

    // move the box to a specific location
    void moveTo(RelativeCoord const &iNewCenter)
    {
      fCenter = Utils::clamp(iNewCenter, fMinCenter, fMaxCenter);
    }

    /**
     * stretch the box by iDeltaX (applied to the left or right)
     *
     * @return true if the box was stretched, false otherwise
     */
    bool stretch(CCoord iDeltaX, DragType iDragType);

    // zoom to max and move to specific location
    void maxZoom(RelativeCoord const &iNewCenter);

    // zoom to minimum (since the box is full, there is no specific location)
    void minZoom();
  };

protected:
  // drawLeftHandle - can be overridden to draw something different but should fit in fLeftHandleRect
  virtual void drawLeftHandle(CDrawContext *iContext);

  // drawRightHandle - can be overridden to draw something different but should fit in fRightHandleRect
  virtual void drawRightHandle(CDrawContext *iContext);

  // drawScrollbar - can be overridden to draw something different but should fit in fScrollbarRect
  virtual void drawScrollbar(CDrawContext *iContext);

  // needsRecomputing when something changes that requires recomputing the 3 AbsoluteRect
  void needsRecomputing() { fNeedsRecomputing = true; markDirty(); }

  // recompute the 3 AbsoluteRect
  void recompute();

  // computeZoomBox
  ZoomBox computeZoomBox() const;

  // returns the width of just the scrollbar (excluding handles and gutter)
  CCoord getScrollbarWidth() const;

protected:
  // set to true when the 3 AbsoluteRect need to be recomputed
  bool fNeedsRecomputing{true};
  ZoomBox fZoomBox{};

  // the 3 Rect representing the position and size of the 3 pieces of the rendering
  AbsoluteRect fLeftHandleRect;
  AbsoluteRect fRightHandleRect;
  AbsoluteRect fScrollbarRect;

  // look and feel
  Margin fMargin{};
  CColor fScrollbarColor{kWhiteCColor};
  CCoord fScrollbarMinSize{-1}; // -1 means it will be computed
  CCoord fScrollbarGutterSpacing{1};

  CColor fZoomHandlesColor{kWhiteCColor};
  CCoord fZoomHandlesSize{-1}; // -1 means it will be computed, 0 means no show

  // how much to slow down (if less than 1) or accelerate (if more than 1) when shift is held when dragging
  Range fShiftDragFactor{1.0};
  bool fEnableZoomDoubleClick{true};

  // offsetPercent tag/param/value + editor and editor value ("value" is used when no param)
  ParamID fOffsetPercentTag{UNDEFINED_PARAM_ID};
  GUIOptionalParam<ParamValue> fOffsetPercentParam{};
  GUIOptionalParamEditor<ParamValue> fOffsetPercentEditor{};

  // zoomPercent tag/param/value + editor and editor value ("value" is used when no param)
  ParamID fZoomPercentTag{UNDEFINED_PARAM_ID};
  GUIOptionalParam<ParamValue> fZoomPercentParam{};
  GUIOptionalParamEditor<ParamValue> fZoomPercentEditor{};

  // used when dragging
  RelativeCoord fDragGestureX{-1.0};
  DragType fDragType{DragType::kNone};

public:
  class Creator : public CustomViewCreator<ScrollbarView, CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerTagAttribute("offset-percent-tag", &ScrollbarView::getOffsetPercentTag, &ScrollbarView::setOffsetPercentTag);
      registerTagAttribute("zoom-percent-tag", &ScrollbarView::getZoomPercentTag, &ScrollbarView::setZoomPercentTag);
      registerMarginAttribute("margin", &ScrollbarView::getMargin, &ScrollbarView::setMargin);
      registerColorAttribute("scrollbar-color", &ScrollbarView::getScrollbarColor, &ScrollbarView::setScrollbarColor);
      registerDoubleAttribute("scrollbar-min-size", &ScrollbarView::getScrollbarMinSize, &ScrollbarView::setScrollbarMinSize);
      registerDoubleAttribute("scrollbar-gutter-spacing", &ScrollbarView::getScrollbarGutterSpacing, &ScrollbarView::setScrollbarGutterSpacing);
      registerColorAttribute("zoom-handles-color", &ScrollbarView::getZoomHandlesColor, &ScrollbarView::setZoomHandlesColor);
      registerDoubleAttribute("zoom-handles-size", &ScrollbarView::getZoomHandlesSize, &ScrollbarView::setZoomHandlesSize);
      registerRangeAttribute("shift-drag-factor", &ScrollbarView::getShiftDragFactor, &ScrollbarView::setShiftDragFactor);
      registerBooleanAttribute("enable-zoom-double-click", &ScrollbarView::getEnableZoomDoubleClick, &ScrollbarView::setEnableZoomDoubleClick);
    }
  };
};

}