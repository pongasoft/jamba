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
#include <pongasoft/VST/GUI/Params/ParamAware.hpp>
#include "StateAware.h"

namespace pongasoft::VST::GUI::Views {

/**
 * Base class that a custom controller can inherit from.
 *
 * Like `CustomView`, this base class offers access to parameter registration and Jamba will automatically
 * call the proper lifecycle methods when such a controller is created.
 */
class CustomController : public VSTGUI::DelegationController, public ParamAware
{
public:
  // Constructor
  explicit CustomController(IController *iBaseController) : DelegationController(iBaseController) {}
};


/**
 * Override from this class if you need to implement a controller specific to a given plugin.
 *
 * Like `StateAwareView`, by inheriting from this class you automatically get access to all the parameters
 * of your plugin (via `StateAware::fParams`) as well as the %GUI state (via `StateAware::fState`).
 *
 * @tparam TGUIState type of the gui state for the plugin (must be a subclass of `GUIState`)
 * @see `StateAwareView` for more information on parameter registration
 */
template<typename TGUIState>
class StateAwareCustomController : public CustomController, public StateAware<TGUIState>
{
  // ensures that TGUIState is a subclass of GUIState
  static_assert(std::is_convertible<TGUIState *, GUIState*>::value, "TGUIState must be a subclass of GUIState");

public:
  // Constructor
  explicit StateAwareCustomController(IController *iBaseController) : CustomController(iBaseController) {}

  /**
   * Allow for registering an arbitrary callback on an arbitrary view without having to inherit from the view.
   * The registration will automatically be discarded when the view is deleted. This is a convenient call
   * that simply delegates to the state.
   *
   * Example usage:
   *
   * ```
   * TextButtonView *button = ....;
   * makeParamAware(button)->registerCallback<int>(fParams->fMyParam,
   *   [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
   *   iButton->setMouseEnabled(iParam > 3);
   * });
   * ```
   * @note You should not keep the pointer returned by this method around as it will be automatically deleted when
   *       the view goes away.
   *
   * @tparam TView must be a subclass of `VSTGUI::CView`
   * @return a pointer to an object for registering callbacks, listener and params.
   */
  template<typename TView>
  inline ParamAwareView<TView> *makeParamAware(TView *iView) {
    return StateAware<TGUIState>::fState->makeParamAware(iView);
  }

protected:
  /**
   * Overriden to call both `ParamAware::initState()` and `StateAware::initState()` */
  void initState(GUIState *iGUIState) override
  {
    CustomController::initState(iGUIState);
    StateAware<TGUIState>::initState(iGUIState);
  }
};

/**
 * @deprecated Use StateAwareCustomController instead
 */
template<typename TGUIState>
class [[deprecated("Since 4.0.0 - Use StateAwareCustomController instead")]] PluginCustomController : public StateAwareCustomController<TGUIState>
{
public:
  explicit PluginCustomController(IController *iBaseController) :
    StateAwareCustomController<TGUIState>(iBaseController)
  {}
};
}