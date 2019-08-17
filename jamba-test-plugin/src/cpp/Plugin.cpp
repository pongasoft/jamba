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

#include "Plugin.h"

namespace pongasoft {
namespace test {
namespace jamba {

//------------------------------------------------------------------------
// JambaTestPluginParameters::JambaTestPluginParameters
//------------------------------------------------------------------------
JambaTestPluginParameters::JambaTestPluginParameters()
{
  // bypass
  fBypassParam =
    vst<BooleanParamConverter>(EJambaTestPluginParamID::kBypass, STR16 ("Bypass"))
      .defaultValue(false)
      .flags(ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass)
      .shortTitle(STR16 ("Bypass"))
      .add();


  // fTab
  fTab =
    vst<DiscreteValueParamConverter<1>>(EJambaTestPluginParamID::kTab, STR16("Tab"))
      .guiOwned()
      .add();

  setRTSaveStateOrder(PROCESSOR_STATE_VERSION,
                      fBypassParam);

  // same for GUI - note that if the GUI does not save anything then you don't need this
  setGUISaveStateOrder(CONTROLLER_STATE_VERSION,
                       fTab);
}

}
}
}