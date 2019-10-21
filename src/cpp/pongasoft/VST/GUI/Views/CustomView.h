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
 * - `onParameterChange()` to react to parameters that have changed (don't forget to call `markDirty()` or delegate to
 *   this class for the view to be redrawn).
 *
 * This class exposes the following attributes:
 *
 * Attribute | Description | More
 * --------- | ----------- | ----
 * `custom-view-tag` | optional tag to differentiate between views when implementing a custom controller | `getCustomViewTag()`
 * `editor-mode` | a flag whose purpose is to render/log information during development when the flag is set to `true` | `getEditorMode()`
 * `back-color` | background color for the view (transparent for no background) | `getBackColor()`
 *
 * @see `CustomViewCreator` for details on how a custom view gets created
 */
class CustomView : public CView, public ParamAware, public ICustomViewLifecycle
{
public:
  // Constructor
  explicit CustomView(const CRect &iSize);

  // Deleting for now... not sure there is an actual use for it
  CustomView(const CustomView &c) = delete;

  /**
   * Returns the back color (background) for the view. The `draw()` method in this class simply delegates
   * to `drawBackColor()` which paints the background of the view with this color (if not set to transparent).
   */
  CColor const &getBackColor() const { return fBackColor; }

  /**
   * @see getBackColor() */
  void setBackColor(CColor const &iColor);

  /**
   * @see getCustomViewTag() */
  void setCustomViewTag (TagID iTag) { fTag = iTag; }

  /**
   * Returns the tag associated to this custom view. This tag (which is **not** related to a parameter), is optional
   * and can be used to differentiate between views when implementing a custom controller.
   *
   * ```
   * // Example
   * CView *SampleEditController::verifyView(CView *iView,
   *                                         const UIAttributes &iAttributes,
   *                                         const IUIDescription *iDescription)
   * {
   *   auto button = dynamic_cast<Views::TextButtonView *>(iView);
   *
   *   if(button) {
   *     switch(button->getCustomViewTag()) {
   *       case ESampleSplitterParamID::kNormalize0Action:
   *         initButton(button, SampleDataAction::Type::kNormalize0, false);
   *         break;
   *
   *         // ....
   *      }
   *   }
   * }
   * ```
   */
  TagID getCustomViewTag () const { return fTag; }

  /**
   * @see getEditorMode() */
  void setEditorMode(bool iEditorMode);

  /**
   * Editor mode is a flag whose purpose is to render/log information during development when the flag is set to
   * `true`. It can be flipped directly in the %VSTGUI Editor without having to recompile the code.
   *
   * ```
   * // Example
   * void SampleEditView::draw(CDrawContext *iContext) {
   * // ...
   * #if EDITOR_MODE
   *
   *  if(getEditorMode())
   *  {
   *    auto rdc = pongasoft::VST::GUI::RelativeDrawContext{this, iContext};
   *    rdc.debug("SampleRange: [%.3f,%.3f] | PixelRange: [%.3f,%.3f] | Visible: [%.3f,%.3f] | BPM: %f",
   *              fState->fWESelectedSampleRange->fFrom, fState->fWESelectedSampleRange->fTo,
   *              fSelectedPixelRange.fFrom, fSelectedPixelRange.fTo,
   *              fVisibleSampleRange.fFrom, fVisibleSampleRange.fTo,
   *              fHostInfo->fTempo);
   *
   *  }
   *
   * #endif
   * }
   * ```
   *
   * @note Since `getEditorMode()` always returns `false` in release build mode, it is recommended to enclose such
   *       code in an <tt>\#if / \#endif</tt> block as shown in the example.
   */
  bool getEditorMode() const;

#if EDITOR_MODE
  /**
   * Overrides this method if you want to implement a specific behavior when editor mode is changed. */
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
   * Callback when a parameter changes. By default simply marks the view as dirty. This method is intended to be
   * overriden to implement specific behavior.
   */
  void onParameterChange(ParamID iParamID) override;

  /**
   * Marks this view dirty which will (at the appropriate time in the rendering lifecycle) trigger a call to `draw()`
   *
   * @warning You should not call `draw()` yourself but instead call this method which will invoke `draw()` at the
   *          appropriate time (for example, calling `markDirty()` 3 times will not invoke `draw()` 3 times...).
   */
  inline void markDirty() { setDirty(true); }

  /**
   * Handles the lifecycle behavior getting triggered once all the attributes have been set (which usually happens
   * after the XML file (uidesc) has been read/processed, or when you modify attributes in the %VSTGUI Editor).
   *
   * @note Subclasses are allowed to extend this behavior (if you want to do some extra setup when the view
   *       is initialized for example), but care should be taken in making sure that this method ends up being
   *       called, otherwise the view will most likely not behave as expected. */
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
  /**
   * Defines and registers the attributes exposed in the %VSTGUI Editor and XML file (`.uidesc`) for `CustomView`.
   *
   * @note Although not required, for clarity and consistency, each custom view defines the creator class with the same
   *       `%Creator` name as an inner class of the view.
   */
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
 * Override from this class if you need to implement a (custom) view specific to a given plugin.
 *
 * When writing a "generic" view (for example, a `MomentaryButtonView`), you deal with parameters by their ids, since
 * the view is meant to work with many parameters. But if you write a view that is very specific to a given plugin,
 * it makes the code easier to write and more type safe if you *tie* the view to the state.
 *
 * By inheriting from this class you automatically get access to all the parameters of your plugin
 * (via `StateAware::fParams`) as well as the %GUI state (via `StateAware::fState`).
 *
 * ```
 * // Example
 * class SliceSettingView : public StateAwareView<ToggleButtonView, SampleSplitterGUIState> {
 *
 * void registerParameters() {
 *   ToggleButtonView::registerParameters();
 *   fSelectedSlice = registerParam(fParams->fSelectedSlice);
 *   fSlicesSettings = registerParam(fState->fSlicesSettings);
 *   setToggleFromSetting();
 * }
 *
 * protected:
 *   GUIVstParam<int> fSelectedSlice{};
 *   GUIJmbParam<SlicesSettings> fSlicesSettings{};
 * };
 * ```
 *
 * @tparam TView the parent view (must be a subclass of `CView`)
 * @tparam TGUIState type of the gui state for the plugin (must be a subclass of `GUIState`)
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
  template<typename... Args>
  explicit StateAwareView(const CRect &iSize, Args&& ...args) : TView(iSize, std::forward<Args>(args)...) {}

protected:
  /**
   * Overriden to call both `ParamAware::initState()` and `StateAware::initState()`
   */
  void initState(GUIState *iGUIState) override
  {
    TView::initState(iGUIState);
    StateAware<TGUIState>::initState(iGUIState);
  }
};

/**
 * Shortcut alias when implementing a `StateAwareView` where the view is a `CustomView`
 *
 * @see `StateAwareView` */
template<typename TGUIState>
using StateAwareCustomView = StateAwareView<CustomView, TGUIState>;

/**
 * This class can be used to extend %VSTGUI classes directly while still benefiting from the extensions added by
 * Jamba.
 *
 * `CustomView` extends `CView` to create a completely custom class. If, on the other end, you simply want to extend
 * another class from %VSTGUI (ex: `CTextEdit`), you can use this class which implements most of the behavior
 * from `CustomView`.
 *
 * This class exposes the following attributes:
 *
 * Attribute | Description | More
 * --------- | ----------- | ----
 * `custom-view-tag` | optional tag to differentiate between views when implementing a custom controller | `getCustomViewTag()`
 * `editor-mode` | a flag whose purpose is to render/log information during development when the flag is set to `true` | `getEditorMode()`
 *
 * @tparam TView the view class (for ex: `CTextEdit`)
 * @see `CustomView`
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

  //! @copydoc pongasoft::VST::GUI::Views::CustomView::markDirty()
  inline void markDirty() { TView::setDirty(true); }

   //! @see getCustomViewTag()
  void setCustomViewTag (TagID iTag) { fTag = iTag; }

  //! @copydoc pongasoft::VST::GUI::Views::CustomView::getCustomViewTag()
  TagID getCustomViewTag () const { return fTag; }

  //! @see getEditorMode()
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

  //! @copydoc pongasoft::VST::GUI::Views::CustomView::getEditorMode()
  bool getEditorMode() const
  {
#if EDITOR_MODE
    return fEditorMode;
#else
    return false;
#endif
  }

#if EDITOR_MODE
  //! @copydoc pongasoft::VST::GUI::Views::CustomView::onEditorModeChanged()
  virtual void onEditorModeChanged() {}
#endif

  //! @copydoc pongasoft::VST::GUI::Views::CustomView::onParameterChange()
  void onParameterChange(ParamID iParamID) override { markDirty(); };

  //! @copydoc pongasoft::VST::GUI::Views::CustomView::afterApplyAttributes()
  void afterApplyAttributes() override
  {
    unregisterAll();
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

  /**
   * Defines and registers the attributes exposed in the %VSTGUI Editor and XML file (`.uidesc`) for `CustomViewAdapter`.
   */
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