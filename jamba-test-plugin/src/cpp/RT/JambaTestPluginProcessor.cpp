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

#include <pongasoft/VST/AudioBuffer.h>
#include <pongasoft/VST/Debug/ParamTable.h>
#include <pongasoft/VST/Debug/ParamLine.h>


#include "JambaTestPluginProcessor.h"

#include "version.h"
#include "jamba_version.h"

namespace pongasoft::test::jamba::RT {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
JambaTestPluginProcessor::JambaTestPluginProcessor(JambaTestPluginParameters const &iParams) : RTProcessor(JambaTestPluginControllerUID), fParams{iParams}, fState{fParams}
{
  DLOG_F(INFO, "[%s] JambaTestPluginProcessor() - jamba: %s - plugin: v%s", stringPluginName, JAMBA_GIT_VERSION_STR, FULL_VERSION_STR);

  // in Debug mode we display the parameters in a table
#ifndef NDEBUG
  DLOG_F(INFO, "Parameters ---> \n%s", Debug::ParamTable::from(fParams).full().toString().c_str());
#endif
}

//------------------------------------------------------------------------
// Destructor - purely for debugging purposes
//------------------------------------------------------------------------
JambaTestPluginProcessor::~JambaTestPluginProcessor()
{
  DLOG_F(INFO, "~JambaTestPluginProcessor()");
}

//------------------------------------------------------------------------
// JambaTestPluginProcessor::initialize - define input/outputs
//------------------------------------------------------------------------
tresult JambaTestPluginProcessor::initialize(FUnknown *context)
{
  DLOG_F(INFO, "JambaTestPluginProcessor::initialize()");

  tresult result = RTProcessor::initialize(context);
  if(result != kResultOk)
  {
    return result;
  }

  //------------------------------------------------------------------------
  // This is where you define inputs and outputs
  //------------------------------------------------------------------------
  addAudioInput(STR16 ("Stereo In"), SpeakerArr::kStereo);
  addAudioOutput(STR16 ("Stereo Out"), SpeakerArr::kStereo);

  //------------------------------------------------------------------------
  // Displays the order in which the RT parameters will be saved (debug only)
  //------------------------------------------------------------------------
#ifndef NDEBUG
  using Key = Debug::ParamDisplay::Key;
  DLOG_F(INFO, "RT Save State - Version=%d --->\n%s",
         fParams.getRTSaveStateOrder().fVersion,
         Debug::ParamTable::from(getRTState(), true).keys({Key::kID, Key::kTitle}).full().toString().c_str());
#endif

  return result;
}

//------------------------------------------------------------------------
// JambaTestPluginProcessor::terminate - purely for debugging purposes
//------------------------------------------------------------------------
tresult JambaTestPluginProcessor::terminate()
{
  DLOG_F(INFO, "JambaTestPluginProcessor::terminate()");

  return RTProcessor::terminate();
}

//------------------------------------------------------------------------
// JambaTestPluginProcessor::setupProcessing
//------------------------------------------------------------------------
tresult JambaTestPluginProcessor::setupProcessing(ProcessSetup &setup)
{
  tresult result = RTProcessor::setupProcessing(setup);

  if(result != kResultOk)
    return result;

  DLOG_F(INFO,
         "JambaTestPluginProcessor::setupProcessing(%s, %s, maxSamples=%d, sampleRate=%f)",
         setup.processMode == kRealtime ? "Realtime" : (setup.processMode == kPrefetch ? "Prefetch" : "Offline"),
         setup.symbolicSampleSize == kSample32 ? "32bits" : "64bits",
         setup.maxSamplesPerBlock,
         setup.sampleRate);

  return result;
}

//------------------------------------------------------------------------
// JambaTestPluginProcessor::genericProcessInputs
// Implementation of the generic (32 and 64 bits) logic.
//------------------------------------------------------------------------
template<typename SampleType>
tresult JambaTestPluginProcessor::genericProcessInputs(ProcessData &data)
{
  if(data.numInputs == 0 || data.numOutputs == 0)
  {
    // nothing to do
    return kResultOk;
  }

  AudioBuffers<SampleType> in(data.inputs[0], data.numSamples);
  AudioBuffers<SampleType> out(data.outputs[0], data.numSamples);

  // simply copy input into output
  out.copyFrom(in);
  
  // use fState.fBypass to disable plugin effect...

  return kResultOk;
}

}
