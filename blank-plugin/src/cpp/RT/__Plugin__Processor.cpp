#include <pongasoft/VST/AudioBuffer.h>
#include <pongasoft/VST/Debug/ParamTable.h>
#include <pongasoft/VST/Debug/ParamLine.h>


#include "[-name-]Processor.h"

#include "version.h"
#include "jamba_version.h"

[-namespace_start-]
namespace RT {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
[-name-]Processor::[-name-]Processor() : RTProcessor([-name-]ControllerUID), fParameters{}, fState{fParameters}
{
  DLOG_F(INFO, "[-name-]Processor() - jamba: %s - plugin: v%s", JAMBA_GIT_VERSION_STR, FULL_VERSION_STR);

  // in Debug mode we display the parameters in a table
#ifndef NDEBUG
  DLOG_F(INFO, "Parameters ---> \n%s", Debug::ParamTable::from(fParameters).full().toString().c_str());
#endif
}

//------------------------------------------------------------------------
// Destructor - purely for debugging purposes
//------------------------------------------------------------------------
[-name-]Processor::~[-name-]Processor()
{
  DLOG_F(INFO, "~[-name-]Processor()");
}

//------------------------------------------------------------------------
// [-name-]Processor::initialize - define input/outputs
//------------------------------------------------------------------------
tresult [-name-]Processor::initialize(FUnknown *context)
{
  DLOG_F(INFO, "[-name-]Processor::initialize()");

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
         fParameters.getRTSaveStateOrder().fVersion,
         Debug::ParamTable::from(getRTState(), true).keys({Key::kID, Key::kTitle}).full().toString().c_str());
#endif

  return result;
}

//------------------------------------------------------------------------
// [-name-]Processor::terminate - purely for debugging purposes
//------------------------------------------------------------------------
tresult [-name-]Processor::terminate()
{
  DLOG_F(INFO, "[-name-]Processor::terminate()");

  return RTProcessor::terminate();
}

//------------------------------------------------------------------------
// [-name-]Processor::setupProcessing
//------------------------------------------------------------------------
tresult [-name-]Processor::setupProcessing(ProcessSetup &setup)
{
  tresult result = RTProcessor::setupProcessing(setup);

  if(result != kResultOk)
    return result;

  DLOG_F(INFO,
         "[-name-]Processor::setupProcessing(%s, %s, maxSamples=%d, sampleRate=%f)",
         setup.processMode == kRealtime ? "Realtime" : (setup.processMode == kPrefetch ? "Prefetch" : "Offline"),
         setup.symbolicSampleSize == kSample32 ? "32bits" : "64bits",
         setup.maxSamplesPerBlock,
         setup.sampleRate);

  return result;
}

//------------------------------------------------------------------------
// [-name-]Processor::genericProcessInputs
// Implementation of the generic (32 and 64 bits) logic.
//------------------------------------------------------------------------
template<typename SampleType>
tresult [-name-]Processor::genericProcessInputs(ProcessData &data)
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
[-namespace_end-]
