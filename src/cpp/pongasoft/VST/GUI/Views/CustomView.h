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
#pragma once

#include <vstgui4/vstgui/lib/cview.h>
#include <map>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.h>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include "CustomViewCreator.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;
using namespace Params;

/**
 * Base class that all custom views will inherit from. Defines a basic back color.
 * The custom view tag should be a tag associated to the view itself not a parameter (like it is the case for CControl).
 * The registerParameters method is the method that you inherit from to register which VST parameters your view will
 * use. By default each parameter will be also be registered to listen for changes which will trigger the view to be
 * redrawn: the onParameterChange method can be overridden to react differently (or additionally) to handle parameter changes.
 * You use the convenient registerXXParameter methods to register each parameter.
 * Ex:
 * ...
 * std::unique_ptr<BooleanParameter> fMyBoolParameter{nullptr};
 * std::unique_ptr<PercentParameter> fMyPercentParameter{nullptr};
 * std::unique_ptr<PercentParameter::Editor> fMyPercentParameterEditor{nullptr};
 * ...
 * void registerParameters() override
 * {
 *   fMyBoolParameter = registerBooleanParameter(EParamIDs::kMyBoolParamID);
 *   fMyPercentParameter = registerPercentParameter(EParamIDs::kMyPercentParamID);
 * }
 * ...
 * void draw() override
 * {
 *   CustomView::draw();
 *
 *   if(fMyBoolParameter->getValue())
 *     ... do something ...
 *
 *   CColor c = CColor{255, 255, 255, fMyPercentParameter->getValue() * 255);
 *   ...
 * }
 *
 * void onMouseDown(CPoint &where, const CButtonState &buttons)
 * {
 *    double percent = ...; // for example using where.y compute a percentage value
 *    fMyPercentParameterEditor = fMyPercentParameter->edit(percent);
 * }
 *
 * void onMouseMoved(CPoint &where, const CButtonState &buttons)
 * {
 *   if(fMyPercentParameterEditor)
 *   {
 *     double percent = ...; // for example using where.y compute a percentage value
 *     fMyPercentParameterEditor->setValue(percent);
 *   }
 * }
 *
 * void onMouseUp(CPoint &where, const CButtonState &buttons)
 * {
 *   if(fMyPercentParameterEditor)
 *   {
 *     double percent = ...; // for example using where.y compute a percentage value
 *     fMyPercentParameterEditor->commit(percent);
 *     fMyPercentParameterEditor = nullptr;
 *   }
 * }
 *
 * void onMouseCancel()
 * {
 *   if(fMyPercentParameterEditor)
 *   {
 *     fMyPercentParameterEditor->rollback();
 *     fMyPercentParameterEditor = nullptr;
 *   }
 * }
 */
class CustomView : public CView, public GUIParamCxAware, public CustomViewInitializer
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
  void setCustomViewTag (int32_t iTag) { fTag = iTag; }
  int32_t getCustomViewTag () const { return fTag; }

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
   * Called when the draw style is changed (simply marks the view dirty)
   */
  void drawStyleChanged();

  /**
   * Callback when a parameter changes. By default simply marks the view as dirty.
   */
  void onParameterChange(ParamID iParamID) override;

  // markDirty
  inline void markDirty() { setDirty(true); }

public:

  CLASS_METHODS_NOCOPY(CustomView, CControl)

  void afterCreate(UIAttributes const &iAttributes, IUIDescription const *iDescription) override;

  // beforeApply
  void beforeApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) override;

  // afterApply
  void afterApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) override;

protected:
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
  int32_t fTag;
  bool fEditorMode;
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
 * This class can be used to get access to the plugin GUI state and parameters
 *
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TGUIPluginState>
class PluginAccessor
{
public:
  // initState
  void initState(GUIState *iGUIState)
  {
    fState = dynamic_cast<TGUIPluginState *>(iGUIState);
    if(fState)
      fParams = &fState->fParams;
  }

public:
  // direct access to state (ex: fState->fLabelA)
  TGUIPluginState *fState{};

  // direct access to parameters (ex: fParams->fBypassParam)
  typename TGUIPluginState::PluginParameters const *fParams{};
};

/**
 * When implementing a CustomView specific to a given plugin, you can use this class instead to get direct
 * access to the state and parameters registered with the plugin via the fState/fParams member.
 *
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TGUIPluginState>
class PluginCustomView : public CustomView, public PluginAccessor<TGUIPluginState>
{
public:
  // Constructor
  explicit PluginCustomView(const CRect &iSize) : CustomView(iSize) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    CustomView::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};

/**
 * This class can be used to extend VST SDK classes directly while still benefiting from the extensions added by
 * this framework (multiple param access and state access)
 *
 * @tparam TView the view class (for ex: CTextEdit)
 */
template<typename TView>
class CustomViewAdapter : public TView, public GUIParamCxAware, public CustomViewInitializer
{
public:
  // Constructor
  template<typename... Args>
  explicit CustomViewAdapter(const CRect &iSize, Args... args) : TView(iSize, args...) {}

  // markDirty
  inline void markDirty() { TView::setDirty(true); }

  /**
   * Callback when a parameter changes. By default simply marks the view as dirty.
   */
  void onParameterChange(ParamID iParamID) override { markDirty(); };

  void afterCreate(UIAttributes const &iAttributes, IUIDescription const *iDescription) override;

  // beforeApply
  void beforeApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) override {};

  // afterApply
  void afterApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) override;
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
  explicit PluginCustomViewAdapter(const CRect &iSize, Args... args) : CustomViewAdapter<TView>(iSize, args...) {}

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    GUIParamCxAware::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};

//------------------------------------------------------------------------
// CustomViewAdapter::afterCreate
//------------------------------------------------------------------------
template<typename TView>
void CustomViewAdapter<TView>::afterCreate(UIAttributes const &iAttributes, IUIDescription const *iDescription)
{
  auto provider = dynamic_cast<GUIStateProvider const *>(iDescription->getViewFactory());
  if(provider)
    initState(provider->getGUIState());
}

//------------------------------------------------------------------------
// CustomViewAdapter::afterApply
//------------------------------------------------------------------------
template<typename TView>
void CustomViewAdapter<TView>::afterApply(UIAttributes const &iAttributes, IUIDescription const *iDescription)
{
  if(fParamCxMgr)
    registerParameters();
}


}
}
}
}