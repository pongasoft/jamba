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

#include "JambaTestPluginController.h"

namespace pongasoft {
namespace test {
namespace jamba {
namespace GUI {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
JambaTestPluginController::JambaTestPluginController() : GUIController("JambaTestPlugin.uidesc"), fParams{}, fState{fParams}
{
  DLOG_F(INFO, "JambaTestPluginController()");
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
JambaTestPluginController::~JambaTestPluginController()
{
  DLOG_F(INFO, "~JambaTestPluginController()");
}

//------------------------------------------------------------------------
// JambaTestPluginController::initialize
//------------------------------------------------------------------------
tresult JambaTestPluginController::initialize(FUnknown *context)
{
  tresult res = GUIController::initialize(context);

  //------------------------------------------------------------------------
  // In debug mode this code displays the order in which the GUI parameters
  // will be saved
  //------------------------------------------------------------------------
#ifndef NDEBUG
  if(res == kResultOk)
  {
    using Key = Debug::ParamDisplay::Key;
    DLOG_F(INFO, "GUI Save State - Version=%d --->\n%s",
           fParams.getGUISaveStateOrder().fVersion,
           Debug::ParamTable::from(getGUIState(), true).keys({Key::kID, Key::kTitle}).full().toString().c_str());
  }
#endif

  return res;
}

}
}
}
}
