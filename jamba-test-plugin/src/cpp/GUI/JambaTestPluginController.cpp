/*
 * Copyright (c) 2019-2020 pongasoft
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
#include "JTPTextButtonController.h"

namespace pongasoft::test::jamba::GUI {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
JambaTestPluginController::JambaTestPluginController(JambaTestPluginParameters const &iParams) : GUIController("JambaTestPlugin.uidesc"), fParams{iParams}, fState{fParams}
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

//------------------------------------------------------------------------
// JambaTestPluginController::createCustomController
//------------------------------------------------------------------------
IController *JambaTestPluginController::createCustomController(UTF8StringPtr iName,
                                                               IUIDescription const *iDescription,
                                                               IController *iBaseController)
{
  if(UTF8StringView(iName) == "TextButtonController")
  {
    return new JTPTextButtonController(iBaseController);
  }

  return nullptr;
}

}
