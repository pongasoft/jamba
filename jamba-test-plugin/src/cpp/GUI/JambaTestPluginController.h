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

#include <pongasoft/VST/GUI/GUIController.h>
#include "../Plugin.h"

namespace pongasoft {
namespace test {
namespace jamba {
namespace GUI {

using namespace pongasoft::VST::GUI;

//------------------------------------------------------------------------
// JambaTestPluginController - Main GUI Controller
//------------------------------------------------------------------------
class JambaTestPluginController : public GUIController
{
public:
  //------------------------------------------------------------------------
  // Factory method used in JambaTestPlugin_VST3.cpp to create the controller
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void * /*context*/) { return (IEditController *) new JambaTestPluginController(); }

public:
  // Constructor
  JambaTestPluginController();

  // Destructor -- overridden for debugging purposes only
  ~JambaTestPluginController() override;

  // getGUIState
  GUIState *getGUIState() override { return &fState; }

protected:
  // initialize
  tresult PLUGIN_API initialize(FUnknown *context) override;

private:
  // The controller gets its own copy of the parameters (defined in Plugin.h)
  JambaTestPluginParameters fParams;

  // The state accessible in the controller and views
  JambaTestPluginGUIState fState;
};

}
}
}
}

