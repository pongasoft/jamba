/*
 * Copyright (c) 2019 pongasoft
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

namespace pongasoft::VST::GUI::Views {

/**
 * This class can be used to get access to the GUI state and parameters with their actual type
 *
 * @tparam TGUIState type of the gui state for the plugin (should be a subclass of `GUIState`)
 */
template<typename TGUIState>
class StateAware
{
  static_assert(std::is_convertible_v<TGUIState *, GUIState *>);
  
public:
  // initState
  virtual void initState(GUIState *iGUIState)
  {
    // because of the static assert, we know that this will not return nullptr
    fState = dynamic_cast<TGUIState *>(iGUIState);
    DCHECK_F(fState != nullptr); // sanity check
    fParams = &fState->fParams;
  }

public:
  // direct access to state (ex: fState->fLabelA)
  TGUIState *fState{};

  // direct access to parameters (ex: fParams->fBypassParam)
  typename TGUIState::PluginParameters const *fParams{};
};

}
