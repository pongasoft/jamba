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

//------------------------------------------------------------------------------------------------------------
// This file contains the standard boilerplate code that VST3 sdk requires to instantiate the plugin
// components
//------------------------------------------------------------------------------------------------------------
#include "JambaTestPluginCIDs.h"

#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <public.sdk/source/main/pluginfactoryvst3.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>

#include "version.h"
#include "RT/JambaTestPluginProcessor.h"
#include "GUI/JambaTestPluginController.h"

using namespace Steinberg::Vst;


#define stringPluginName "JambaTestPlugin"

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
//  VST Plug-in Entry
//------------------------------------------------------------------------
BEGIN_FACTORY_DEF ("pongasoft",
                   "https://www.pongasoft.com",
                   "support@pongasoft.com")

    // JambaTestPluginProcessor processor
    DEF_CLASS2 (INLINE_UID_FROM_FUID(pongasoft::test::jamba::JambaTestPluginProcessorUID),
                PClassInfo::kManyInstances,  // cardinality
                kVstAudioEffectClass,        // the component category (do not changed this)
                stringPluginName,            // here the Plug-in name (to be changed)
                Vst::kDistributable,         // means that component and controller could be distributed on different computers
                Vst::PlugType::kFx,          // Subcategory for this Plug-in (to be changed)
                FULL_VERSION_STR,            // Plug-in version (to be changed)
                kVstVersionString,           // the VST 3 SDK version (do not changed this, use always this define)
                pongasoft::test::jamba::RT::JambaTestPluginProcessor::createInstance)  // function pointer called when this component should be instantiated

    // JambaTestPluginController controller
    DEF_CLASS2 (INLINE_UID_FROM_FUID(pongasoft::test::jamba::JambaTestPluginControllerUID),
                PClassInfo::kManyInstances,  // cardinality
                kVstComponentControllerClass,// the Controller category (do not changed this)
                stringPluginName
                "Controller",  // controller name (could be the same than component name)
                0,            // not used here
                "",            // not used here
                FULL_VERSION_STR,    // Plug-in version (to be changed)
                kVstVersionString,    // the VST 3 SDK version (do not changed this, use always this define)
                pongasoft::test::jamba::GUI::JambaTestPluginController::createInstance)// function pointer called when this component should be instantiated

END_FACTORY
