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

#include <pongasoft/VST/RT/RTProcessor.h>
#include "../JambaTestPlugin.h"

namespace pongasoft {
namespace test {
namespace jamba {
namespace RT {

using namespace pongasoft::VST::RT;

//------------------------------------------------------------------------
// JambaTestPluginProcessor - Real Time Processor
//------------------------------------------------------------------------
class JambaTestPluginProcessor : public RTProcessor
{
public:
  //------------------------------------------------------------------------
  // Factory method used in JambaTestPlugin_VST3.cpp to create the processor
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void * /*context*/) { return (IAudioProcessor *) new JambaTestPluginProcessor(); }

public:
  // Constructor
  JambaTestPluginProcessor();

  // Destructor
  ~JambaTestPluginProcessor() override;

  // getRTState
  RTState *getRTState() override { return &fState; }

  /** Called at first after constructor (setup input/output) */
  tresult PLUGIN_API initialize(FUnknown *context) override;

  // Called at the end before destructor
  tresult PLUGIN_API terminate() override;

  // This is where the setup happens which depends on sample rate, etc..
  tresult PLUGIN_API setupProcessing(ProcessSetup &setup) override;

protected:

  // genericProcessInputs<SampleType>
  template<typename SampleType>
  tresult genericProcessInputs(ProcessData &data);

  // processInputs32Bits
  tresult processInputs32Bits(ProcessData &data) override { return genericProcessInputs<Sample32>(data); }

  // processInputs64Bits
  tresult processInputs64Bits(ProcessData &data) override { return genericProcessInputs<Sample64>(data); }

private:
  // The processor gets its own copy of the parameters (defined in Plugin.h)
  JambaTestPluginParameters fParams;

  // The state
  JambaTestPluginRTState fState;
};

}
}
}
}

