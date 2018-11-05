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

#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

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


}
}
}
}
