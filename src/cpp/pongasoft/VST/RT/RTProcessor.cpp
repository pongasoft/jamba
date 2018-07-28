#include "RTProcessor.h"

namespace pongasoft {
namespace VST {
namespace RT {

//------------------------------------------------------------------------
// RTProcessor::RTProcessor
//------------------------------------------------------------------------
RTProcessor::RTProcessor(Steinberg::FUID const &iControllerUID) :
  AudioEffect(),
  fGUITimerCallback{this},
  fGUITimerIntervalMs{0},
  fGUITimer{nullptr},
  fActive{false}
{
  setControllerClass(iControllerUID);
}

//------------------------------------------------------------------------
// RTProcessor::setActive
//------------------------------------------------------------------------
tresult RTProcessor::setActive(TBool iState)
{
  fActive = iState;
  fGUITimer = nullptr;

  // when the processor is activated, start the GUI thread
  if(fActive && fGUITimerIntervalMs > 0)
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "RTProcessor::setActive - Enabling GUI timer - interval [%d]", fGUITimerIntervalMs);
#endif
    fGUITimer = AutoReleaseTimer::create(&fGUITimerCallback, fGUITimerIntervalMs);
  }
  
  return kResultOk;
}

//------------------------------------------------------------------------
// RTProcessor::process
//------------------------------------------------------------------------
tresult RTProcessor::process(ProcessData &data)
{
  auto state = getRTState();

  // 1. we check if there was any state update (UI calls setState)
  state->beforeProcessing();

  // 2. process parameter changes (this will override any update in step 1.)
  if(data.inputParameterChanges != nullptr)
  {
    state->applyParameterChanges(*data.inputParameterChanges);
  }

  // 3. process inputs
  tresult res = processInputs(data);

  // 4. update the previous state
  state->afterProcessing();

  return res;
}

//------------------------------------------------------------------------
// RTProcessor::processInputs
//------------------------------------------------------------------------
tresult RTProcessor::processInputs(ProcessData &data)
{
  if(data.symbolicSampleSize == kSample32)
  {
#ifdef JAMBA_DEBUG_LOGGING
    if(fSymbolicSampleSize != data.symbolicSampleSize)
    {
      fSymbolicSampleSize = data.symbolicSampleSize;
      DLOG_F(INFO, "RTProcessor::processInputs - Using 32 bits processing");
    }
#endif
    return processInputs32Bits(data);
  }

  if(data.symbolicSampleSize == kSample64)
  {
#ifdef JAMBA_DEBUG_LOGGING
    if(fSymbolicSampleSize != data.symbolicSampleSize)
    {
      fSymbolicSampleSize = data.symbolicSampleSize;
      DLOG_F(INFO, "RTProcessor::processInputs - Using 64 bits processing");
    }
#endif
    return processInputs64Bits(data);
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTProcessor::canProcessSampleSize
//------------------------------------------------------------------------
tresult RTProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
  if(symbolicSampleSize == kSample32)
    return canProcess32Bits() ? kResultOk : kResultFalse;

  // we support double processing
  if(symbolicSampleSize == kSample64)
    return canProcess64Bits() ? kResultOk : kResultFalse;

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTProcessor::enableGUITimer
//------------------------------------------------------------------------
void RTProcessor::enableGUITimer(uint32 iUIFrameRateMs)
{
  fGUITimerIntervalMs = iUIFrameRateMs;
}

//------------------------------------------------------------------------
// RTProcessor::setState
//------------------------------------------------------------------------
tresult RTProcessor::setState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);
  return getRTState()->readNewState(streamer);
}

//------------------------------------------------------------------------
// RTProcessor::getState
//------------------------------------------------------------------------
tresult RTProcessor::getState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);
  return getRTState()->writeLatestState(streamer);
}

//------------------------------------------------------------------------
// RTProcessor::initialize
//------------------------------------------------------------------------
tresult RTProcessor::initialize(FUnknown *context)
{
  tresult result = AudioEffect::initialize(context);

  if(result != kResultOk)
    return result;

  return getRTState()->init();
}


}
}
}