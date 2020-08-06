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

#include <unordered_map>
#include <vector>
#include <pongasoft/VST/FObjectCx.h>
#include <vstgui4/vstgui/lib/iviewlistener.h>
#include <pongasoft/VST/GUI/Params/GUIJmbParameter.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include <pongasoft/VST/GUI/Params/ParamAware.h>
#include <pongasoft/VST/GUI/Views/GlobalKeyboardHook.h>

namespace pongasoft::VST::GUI {

using namespace VSTGUI;
using namespace Params;

/**
 * This class manages the views that have been made "param aware". This means that callbacks can be registered against
 * a view (CView). The idea is to be able to register callbacks for the life of a view without having to inherit from
 * the view to implement a similar behavior (which is much more involved).
 *
 * Example1 usage:
 *
 *     TextButtonView *button = ....;
 *     fState->makeParamAware(button)->registerCallback<int>(fParams->fMyParam,
 *       [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
 *       iButton->setMouseEnabled(iParam > 3);
 *     });
 *
 * This examples sets up a callback on the button view, so that when the (Vst) parameter "my param"
 * changes, the callback is invoked. Since the view and the parameter are part of the callback API, it is easy to
 * implement simple behaviors like this. Usually this can be done from the main controller (or sub controller) in the
 * verifyView method.
 *
 * Example2 usage:
 *
 *     TextButtonView *button = ....;
 *     auto pa = fState->makeParamAware(button);
 *     pa->registerParam(fParams->fMyVstParam);
 *     pa->registerParam(fState->fMyJmbParam);
 *     pa->registerListener([this] (TextButtonView *iButton, ParamID iParamID) {
 *       if(iParamID == fParams->fMyVstParam.getParamID())
 *         // do something... iButton->xxx
 *       if(iParamID == fParams->fState->fMyJmbParam())
 *         // do something else... iButton->xxx
 *      });
 *      pa->invokeAll(); // optionally invoke the listener right away to initialize the button
 *
 * This examples sets up the button view as being interested in changes to 2 parameters and when they change,
 * the listener will be invoked.
 *
 * An alternative would be to inherit from the view which is more work (especially if the only purpose is add a
 * simple callback like behavior):
 *
 *     class MyView : public TextButtonView, StateAware<MyGUIState>
 *     {
 *       public:
 *         // constructor
 *         MyView(...);
 *
 *         void registerParameters() override
 *         {
 *            registerVstCallback(fParams->fMyParam, [this] (GUIVstParam<int> &iParam) {
 *              setMouseEnabled(iParam > 3);
 *            });
 *         }
 *     };
 */
class ParamAwareViews : private ViewListenerAdapter
{
public:
  /**
   * @tparam TView should be a subclass of VSTGUI::CView
   * @return a pointer (owned by this class) to an object for registering callbacks, listener and params.
   *         Note: You should not keep this pointer around. It is owned by this class and will automatically be deleted
   *         when the view goes away.
   */
  template<typename TView>
  ParamAwareView<TView> *makeParamAware(TView *iView, GUIState *iGUIState);

  /**
   * Close all previously established connections
   */
  void closeAll();

private:
  /**
   * Called by VSTGUI when a view that was previously registered is being deleted => all its connections will be closed
   */
  void viewWillDelete(CView *iView) override;

private:
  std::unordered_map<CView *, std::unique_ptr<ParamAware>> fParamAwareViews{};
};

//------------------------------------------------------------------------
// ParamAwareViews::makeParamAware
//------------------------------------------------------------------------
template<typename TView>
ParamAwareView<TView> *ParamAwareViews::makeParamAware(TView *iView, GUIState *iGUIState)
{
  if(iView == nullptr)
    return nullptr;

  auto iter = fParamAwareViews.find(iView);

  if(iter == fParamAwareViews.end())
  {
    iView->registerViewListener(this);
    fParamAwareViews[iView] = std::make_unique<ParamAwareView<TView>>(iView);
    fParamAwareViews[iView]->initState(iGUIState);
  }

  return static_cast<ParamAwareView<TView> *>(fParamAwareViews[iView].get());
}


}