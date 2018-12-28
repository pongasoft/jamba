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

#include <vstgui/uidescription/icontroller.h>
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.hpp>
#include "PluginAccessor.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

/**
 * Base class that a custom controller can inherit from, providing access to params
 */
class CustomController : public VSTGUI::IController, public GUIParamCxAware
{
public:
  ~CustomController() override = default;

  // valueChanged => does nothing by default
  void valueChanged(VSTGUI::CControl *pControl) override { }
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
  ~PluginCustomController() override = default;

protected:
  // initState - overridden to extract fParams
  void initState(GUIState *iGUIState) override
  {
    CustomController::initState(iGUIState);
    PluginAccessor<TGUIPluginState>::initState(iGUIState);
  }
};

}
}
}
}