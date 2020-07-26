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
// This file contains the standard boilerplate code that VST2 requires to instantiate the plugin
// components: it creates an adapter which wraps the VST3 plugin to adapt its API to the VST2 world.
//
// Note that VST2 requires a 4 letter code that needs to be registered with Steinberg =>
// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
//
// Steinberg is planning to officially kill VST2 in October 2018 =>
// https://sdk.steinberg.net/viewtopic.php?f=6&t=557
//------------------------------------------------------------------------------------------------------------
#include <pongasoft/VST/VST2/vst2wrapper_jamba.h>
#include "JambaTestPluginCIDs.h"

#define VST2_ID 'JTPx'

//------------------------------------------------------------------------
::AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
  return Steinberg::Vst::Jamba::Vst2Wrapper::create(GetPluginFactory(),
                                                    pongasoft::test::jamba::JambaTestPluginProcessorUID,
                                                    VST2_ID,
                                                    audioMaster);
}
