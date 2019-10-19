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
#include <pongasoft/VST/GUI/Params/ParamAware.hpp>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include "CustomViewCreator.h"
#include "StateAware.h"
#include "CustomViewLifecycle.h"

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * Class you should inherit from if you want to write a custom view. It offers the following functionalities:
 *
 * - a back color that can be set (disabled by setting it to transparent)
 * - a tag that can be associated with the view itself (can be used for example when writing custom controller)
 * - an editor mode which can be used while developing the custom view (for example, draw debug info...)
 * - gives access to parameter registration via `Params::ParamAware`
 * - handle default parameter listener (mark the view dirty which triggers a redraw)
 *
 * In general, when inheriting from this class you will typically override:
 *
 * - `draw()` to handle the look and feel of the view
 * - `registerParameters()` to register the parameters this view depends on
 * - `onParameterChange()` to react to parameters that have changed (don't forget call `markDirty()` or delegate to
 *   this class for the view to be redrawn).
 */
class CustomView : public CView, public ParamAware, public ICustomViewLifecycle
{
public:
  // Constructor
  explicit CustomView(const CRect &iSize);

  // Deleting for now... not sure there is an actual use for it
  CustomView(const CustomView &c) = delete;


  /**
   * Returns the back color (background) for the view. The `draw` method in this class simply delegates
   * to `drawBackColor` which paints the background if thie view with this color (if not set to transparent).
   */
  CColor const &getBackColor() const { return fBackColor; }

  /**
   * @see getBackColor() */
  void setBackColor(CColor const &iColor);

  // setCustomViewTag / getCustomViewTag
  void setCustomViewTag (TagID iTag) { fTag = iTag; }

  /**
   * Returns the tag associated to this custom view. This tag (which is **not** related to a parameter), is optional
   * and can be used
   * @return
   */
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
    unregisterAll();
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
  ParamID fTag;
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
 * @tparam TView the parent view (should be a subclass of `CView`)
 * @tparam TGUIState type of the gui state for the plugin (should be a subclass of `GUIState`)
 */
template<typename TView, typename TGUIState>
class StateAwareView : public TView, public StateAware<TGUIState>
{
  // ensures that TView is a subclass of CView
  static_assert(std::is_convertible<TView *, CView*>::value, "TView must be a subclass of CView");

  // ensures that TGUIState is a subclass of GUIState
  static_assert(std::is_convertible<TGUIState *, GUIState*>::value, "TGUIState must be a subclass of GUIState");

public:
  // Constructor
  explicit StateAwareView(const CRect &iSize) : TView(iSize) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    TView::initState(iGUIState);
    StateAware<TGUIState>::initState(iGUIState);
  }
};

/**
 * When implementing a CustomView specific to a given plugin, you can use this class instead to get direct
 * access to the state and parameters registered with the plugin via the fState/fParams member.
 *
 * @tparam TGUIState type of the gui state for the plugin (should be a subclass of `GUIState`)
 */
template<typename TGUIState>
using StateAwareCustomView = StateAwareView<CustomView, TGUIState>;

/**
 * This class can be used to extend VST SDK classes directly while still benefiting from the extensions added by
 * this framework (multiple param access and state access)
 *
 * @tparam TView the view class (for ex: CTextEdit)
 */
template<typename TView>
class CustomViewAdapter : public TView, public ParamAware, public ICustomViewLifecycle
{
  // ensures that TView is a subclass of CView
  static_assert(std::is_convertible<TView *, CView*>::value, "TView must be a subclass of CView");

public:
  // Constructor
  template<typename... Args>
  explicit CustomViewAdapter(const CRect &iSize, Args&& ...args) : TView(iSize, std::forward<Args>(args)...), fTag{UNDEFINED_PARAM_ID} {}

  // markDirty
  inline void markDirty() { TView::setDirty(true); }

  // setCustomViewTag / getCustomViewTag
  void setCustomViewTag (ParamID iTag) { fTag = iTag; }
  ParamID getCustomViewTag () const { return fTag; }

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
    unregisterAll();
    registerParameters();
    markDirty();
  }

protected:
  ParamID fTag;
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
 * @tparam TGUIState type of the gui state for the plugin (should be a subclass of `GUIState`)
 */
template<typename TView, typename TGUIState>
class StateAwareCustomViewAdapter : public CustomViewAdapter<TView>, public StateAware<TGUIState>
{
  // ensures that TView is a subclass of CView
  static_assert(std::is_convertible<TView *, CView*>::value, "TView must be a subclass of CView");

  // ensures that TGUIState is a subclass of GUIState
  static_assert(std::is_convertible<TGUIState *, GUIState*>::value, "TGUIState must be a subclass of GUIState");

public:
  // Constructor
  template<typename... Args>
  explicit StateAwareCustomViewAdapter(const CRect &iSize, Args&& ...args) :
    CustomViewAdapter<TView>(iSize, std::forward<Args>(args)...) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    ParamAware::initState(iGUIState);
    StateAware<TGUIState>::initState(iGUIState);
  }
};

/**
 * @deprecated Use StateAwareView instead
 */
template<typename TView, typename TGUIState>
class [[deprecated("Since 4.0.0 - Use StateAwareView instead")]] PluginView : public StateAwareView<TView, TGUIState>
{
public:
  explicit PluginView(const CRect &iSize) : StateAwareView<TView, TGUIState>(iSize) {}
};

/**
 * @deprecated Use StateAwareCustomView instead
 */
template<typename TGUIState>
class [[deprecated("Since 4.0.0 - Use StateAwareCustomView instead")]] PluginCustomView : public StateAwareCustomView<TGUIState>
{
public:
  explicit PluginCustomView(const CRect &iSize) : StateAwareCustomView<TGUIState>(iSize) {}
};

/**
 * @deprecated Use StateAwareCustomViewAdapter instead
 */
template<typename TView, typename TGUIState>
class [[deprecated("Since 4.0.0 - Use StateAwareCustomViewAdapter instead")]] PluginCustomViewAdapter : public StateAwareCustomViewAdapter<TView, TGUIState>
{
public:
  template<typename... Args>
  explicit PluginCustomViewAdapter(const CRect &iSize, Args &&... args) :
    StateAwareCustomViewAdapter<TView, TGUIState>(iSize, std::forward<Args>(args)...)
  {}
};

}