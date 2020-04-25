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

//------------------------------------------------------------------------------------------------------------
// This file contains the standard boilerplate code that VST3 sdk requires to instantiate the plugin
// components
//------------------------------------------------------------------------------------------------------------
#include "JambaTestPluginCIDs.h"

#include "version.h"
#include "RT/JambaTestPluginProcessor.h"
#include "GUI/JambaTestPluginController.h"

#include <pongasoft/VST/PluginFactory.h>

using namespace pongasoft::VST;

//------------------------------------------------------------------------
//  Module init/exit
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// called after library was loaded
bool InitModule()
{
  return true;
}

//------------------------------------------------------------------------
// called after library is unloaded
bool DeinitModule()
{
  return true;
}

//------------------------------------------------------------------------
//  VST3 Plugin Main entry point
//------------------------------------------------------------------------
EXPORT_FACTORY Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
{
  return JambaPluginFactory::GetVST3PluginFactory<
    pongasoft::test::jamba::RT::JambaTestPluginProcessor, // processor class (Real Time)
    pongasoft::test::jamba::GUI::JambaTestPluginController // controller class (GUI)
  >("pongasoft", // vendor
    "https://www.pongasoft.com", // url
    "support@pongasoft.com", // email
    stringPluginName, // plugin name
    FULL_VERSION_STR, // plugin version
    Vst::PlugType::kFx // plugin category (can be changed to other like kInstrument, etc...)
   );
}
