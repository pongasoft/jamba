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

#include <vstgui4/vstgui/uidescription/delegationcontroller.h>
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.hpp>
#include "PluginAccessor.h"

namespace pongasoft::VST::GUI::Views {

/**
 * Base class that a custom controller can inherit from, providing access to params
 */
class CustomController : public VSTGUI::DelegationController, public GUIParamCxAware
{
public:
  // Constructor
  explicit CustomController(IController *iBaseController) : DelegationController(iBaseController) {}
};


/**
 * When implementing a CustomController specific to a given plugin, you can use this class instead to get direct
 * access to the state and parameters registered with the plugin via the fState/fParams member.
 *
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
 */
template<typename TGUIPluginState>
class PluginCustomController : public CustomController, public PluginAccessor<TGUIPluginState>
{
public:
  // Constructor
  explicit PluginCustomController(IController *iBaseController) : CustomController(iBaseController) {}

  /**
   * Allow for registering an arbitrary callback on an arbitrary view without having to inherit from the view.
   * The registration will automatically be discarded when the view is deleted. This is a convenient call
   * that simpply delegates to the state.
   *
   * Example usage:
   *
   * ```
   * TextButtonView *button = ....;
   * registerConnectionFor(button)->registerCallback<int>(fParams->fMyParam,
   *   [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
   *   iButton->setMouseEnabled(iParam > 3);
   * });
   *```
   *
   * @tparam TView should be a subclass of VSTGUI::CView
   * @return a pointer to an object for registering callbacks, listener and params.
   *         Note: You should not keep this pointer around as it be will automatically be deleted when the view
   *         goes away.
   */
  template<typename TView>
  inline ViewGUIParamCxAware<TView> *registerConnectionFor(TView *iView) {
    return PluginAccessor<TGUIPluginState>::fState->registerConnectionFor(iView);
  }

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    CustomController::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};

}