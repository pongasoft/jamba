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

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/vsttypes.h>

namespace pongasoft {
namespace test {
namespace jamba {

//------------------------------------------------------------------------
// These 2 IDs are used in JambaTestPlugin_VST2.cpp and JambaTestPlugin_VST3.cpp to create
// the processor (RT) and controller (GUI). Those IDs are unique and have
// been generated automatically.
//------------------------------------------------------------------------
static const ::Steinberg::FUID JambaTestPluginProcessorUID(0xaa3926ff, 0x6d324b93, 0x80f3f867, 0x9545fa05);
static const ::Steinberg::FUID JambaTestPluginControllerUID(0xbe691f77, 0x73d04d92, 0x9ef55ad4, 0x4cc563c8);

//------------------------------------------------------------------------
// Parameters and Custom view ids
//------------------------------------------------------------------------
enum EJambaTestPluginParamID : Steinberg::Vst::ParamID
{
  // although NOT a requirement, I like to start at 1000 so that they are all 4 digits.
  // the grouping and numbering is arbitrary and you can use whatever makes sense for your case.

  // the bypass parameter which has a special meaning to the host
  kBypass = 1000,
};

}
}
}
