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

#include <vstgui4/vstgui/lib/cview.h>
#include <map>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.hpp>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include "CustomViewCreator.h"
#include "PluginAccessor.h"
#include "CustomViewLifecycle.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * Base class that all custom views will inherit from. Defines a basic back color.
 * The custom view tag should be a tag associated to the view itself not a parameter (like it is the case for `CControl`).
 * The CustomView::registerParameters method is the method that you inherit from to register which Vst parameters your view will
 * use. By default each parameter will be also be registered to listen for changes which will trigger the view to be
 * redrawn: the CustomView::onParameterChange method can be overridden to react differently (or additionally) to handle parameter changes.
 * You use the convenient `registerParam` or `registerCallback` methods to register each parameter.
 */
class CustomView : public CView, public GUIParamCxAware, public ICustomViewLifecycle
{
public:
  // Constructor
  explicit CustomView(const CRect &iSize);

  // Deleting for now... not sure there is an actual use for it
  CustomView(const CustomView &c) = delete;

  // setBackColor / getBackColor
  void setBackColor(CColor const &iColor);
  CColor const &getBackColor() const { return fBackColor; }

  // setCustomViewTag / getCustomViewTag
  void setCustomViewTag (TagID iTag) { fTag = iTag; }
  TagID getCustomViewTag () const { return fTag; }

  // setEditorMode / getEditorMode
  void setEditorMode(bool iEditorMode);
  bool getEditorMode() const;

  /**
   * Implement this if you want to have a specific behavior when editor mode is changed.
   */
#if EDITOR_MODE
  virtual void onEditorModeChanged() {}
#endif

  /**
   * The basic draw method which will erase the background with the back color. You override this method
   * to implement your additional own logic.
   */
  void draw(CDrawContext *iContext) override;

  /**
   * Draws the back color (if not set to transparent)
   */
  virtual void drawBackColor(CDrawContext *iContext);

  /**
   * Called when the draw style is changed (simply marks the view dirty)
   */
  void drawStyleChanged();

  /**
   * Callback when a parameter changes. By default simply marks the view as dirty.
   */
  void onParameterChange(ParamID iParamID) override;

  // markDirty
  inline void markDirty() { setDirty(true); }

  /**
   * Once all the attributes have been set, we call `registerParameters`. Subclasses can extend the behavior
   * but should end up calling this method otherwise the view will most likely not behave as expected. */
  void afterApplyAttributes() override
  {
    registerParameters();
    markDirty();
  }

public:
  CLASS_METHODS_NOCOPY(CustomView, CControl)

protected:
  using CView::sizeToFit; // fixes overload hiding warning

  /**
   * Convenient call to size to fit this view according to the and height provided
   */
  void sizeToFit(CCoord iWidth, CCoord iHeight)
  {
    CRect vs(getViewSize());
    vs.setWidth(iWidth);
    vs.setHeight(iHeight);
    setViewSize(vs, true);
    setMouseableArea(vs);
  }

  /**
   * Convenient call to size to fit this view to match the bitmap
   */
  bool sizeToFit(BitmapPtr iBitmap, int iFrameCount = 1)
  {
    if(iBitmap)
    {
      sizeToFit(iBitmap->getWidth(), iBitmap->getHeight() / iFrameCount);
      return true;
    }
    return false;
  }

protected:
  TagID fTag;
#if EDITOR_MODE
  bool fEditorMode{false};
#endif
  CColor fBackColor;

public:
  class Creator : public TCustomViewCreator<CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      TCustomViewCreator<CustomView>(iViewName, iDisplayName)
    {
      registerTagAttribute("custom-view-tag", &CustomView::getCustomViewTag, &CustomView::setCustomViewTag);
#if EDITOR_MODE
      registerBooleanAttribute("editor-mode", &CustomView::getEditorMode, &CustomView::setEditorMode);
#endif
      registerColorAttribute(UIViewCreator::kAttrBackColor, &CustomView::getBackColor, &CustomView::setBackColor);
    }
  };
};

/**
 * When implementing a View specific to a given plugin, you can use this class instead to get direct
 * access to the state and parameters registered with the plugin via the fState/fParams member.
 *
 * @tparam TView the parent view (should be a subclass of GUIParamCxAware
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TView, typename TGUIPluginState>
class PluginView : public TView, public PluginAccessor<TGUIPluginState>
{
public:
  // Constructor
  explicit PluginView(const CRect &iSize) : TView(iSize) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    TView::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};

/**
 * When implementing a CustomView specific to a given plugin, you can use this class instead to get direct
 * access to the state and parameters registered with the plugin via the fState/fParams member.
 *
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TGUIPluginState>
using PluginCustomView = PluginView<CustomView, TGUIPluginState>;

/**
 * This class can be used to extend VST SDK classes directly while still benefiting from the extensions added by
 * this framework (multiple param access and state access)
 *
 * @tparam TView the view class (for ex: CTextEdit)
 */
template<typename TView>
class CustomViewAdapter : public TView, public GUIParamCxAware, public ICustomViewLifecycle
{
  // ensures that TView is a subclass of CView
  static_assert(std::is_convertible<TView *, CView*>::value, "TView must be a subclass of CView");

public:
  // Constructor
  template<typename... Args>
  explicit CustomViewAdapter(const CRect &iSize, Args&& ...args) : TView(iSize, std::forward<Args>(args)...), fTag{UNDEFINED_TAG_ID} {}

  // markDirty
  inline void markDirty() { TView::setDirty(true); }

  // setCustomViewTag / getCustomViewTag
  void setCustomViewTag (TagID iTag) { fTag = iTag; }
  TagID getCustomViewTag () const { return fTag; }

  // setEditorMode
  void setEditorMode(bool iEditorMode)
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

  // getEditorMode
  bool getEditorMode() const
  {
#if EDITOR_MODE
    return fEditorMode;
#else
    return false;
#endif
  }

  /**
   * Implement this if you want to have a specific behavior when editor mode is changed.
   */
#if EDITOR_MODE
  virtual void onEditorModeChanged() {}
#endif

  /**
   * Callback when a parameter changes. By default simply marks the view as dirty.
   */
  void onParameterChange(ParamID iParamID) override { markDirty(); };

  /**
   * Once all the attributes have been set, we call `registerParameters`. Subclasses can extend the behavior
   * but should end up calling this method otherwise the view will most likely not behave as expected. */
  void afterApplyAttributes() override
  {
    registerParameters();
    markDirty();
  }

protected:
  TagID fTag;
#if EDITOR_MODE
  bool fEditorMode{false};
#endif

public:
  using creator_super_type = TCustomViewCreator<CustomViewAdapter>;

  class Creator : public creator_super_type
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      creator_super_type(iViewName, iDisplayName)
    {
      creator_super_type::registerTagAttribute("custom-view-tag", &CustomViewAdapter::getCustomViewTag, &CustomViewAdapter::setCustomViewTag);
#if EDITOR_MODE
      creator_super_type::registerBooleanAttribute("editor-mode", &CustomViewAdapter::getEditorMode, &CustomViewAdapter::setEditorMode);
#endif
    }
  };
};

/**
 * This class can be used to extend VST SDK classes directly while still benefiting from the extensions added by
 * this framework (multiple param access and state access)
 *
 * @tparam TView the view class (for ex: CTextEdit)
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TView, typename TGUIPluginState>
class PluginCustomViewAdapter : public CustomViewAdapter<TView>, public PluginAccessor<TGUIPluginState>
{
public:
  // Constructor
  template<typename... Args>
  explicit PluginCustomViewAdapter(const CRect &iSize, Args&& ...args) : CustomViewAdapter<TView>(iSize, std::forward<Args>(args)...) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    GUIParamCxAware::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};


}