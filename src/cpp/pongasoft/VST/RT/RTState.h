#pragma once

#include "RTParameter.h"

#include <pongasoft/Utils/Concurrent/Concurrent.h>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/NormalizedState.h>

#include <map>

namespace pongasoft {
namespace VST {
namespace RT {

using namespace Utils;

/**
 * Manages the state used by the processor: you add all the parameters that the state manages using the add method.
 * Combined with the RTProcessor class, everything will be handled for you (reading/writing the state, updating previous
 * value, etc...).
 */
class RTState
{
public:
  explicit RTState(Parameters const &iParameters);

  /**
   * This method is called for each parameter managed by RTState. The order in which this method is called is
   * important and reflects the order that will be used when reading/writing state to the stream
   */
  template<typename ParamConverter>
  RTParam<ParamConverter> add(ParamDefSPtr<ParamConverter> iParamDef);

  /**
   * Call this method after adding all the parameters. If using the RT processor, it will happen automatically. */
  tresult init();

  /**
   * This method should be call at the beginning of the process(ProcessData &data) method before doing anything else.
   * The goal of this method is to update the current state with a state set by the UI (typical use case is to
   * initialize the plugin when being loaded)
   *
   * @return true if the state of the plugin changed
   * */
  virtual bool beforeProcessing();

  /**
   * This method should be called in every frame when there are parameter changes to update this state accordingly
   */
  virtual bool applyParameterChanges(IParameterChanges &inputParameterChanges);

  /**
   * This method should be called at the end of process(ProcessData &data) method. It will update the previous state
   * to the current one and save the latest changes (if necessary) so that it is accessible via writeLatestState.
   */
  virtual void afterProcessing();

  /**
   * This method should be called from Processor::setState to update this state to the state stored in the stream.
   * Note that this method is called from the UI thread so the update is queued until the next frame.
   */
  virtual tresult readNewState(IBStreamer &iStreamer);

  /**
   * This method should be called from Processor::getState to store the latest state to the stream. Note that this
   * method is called from the UI thread and gets the "latest" state as of the end of the last frame.
   */
  virtual tresult writeLatestState(IBStreamer &oStreamer);

protected:
  // the order in which to save the state
  Parameters::SaveStateOrder fSaveStateOrder;

  // contains all the registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<RTRawParameter>> fParameters{};

  // add raw parameter to the structures
  void addRawParameter(std::shared_ptr<RTRawParameter> const &iParameter);

private:
  // need to allocate memory only at creation time for RT!
  NormalizedState fNormalizedStateRT;

  // computeLatestState
  void computeLatestState();

private:
  // this queue is used to propagate a Processor::setState call (made from the UI thread) to this state
  // the check happens in beforeProcessing
  Concurrent::WithSpinLock::SingleElementQueue<NormalizedState> fStateUpdate;

  // this atomic value always hold the most current (and consistent) version of this state so that the UI thread
  // can access it in Processor::getState. It is updated in afterProcessing.
  Concurrent::WithSpinLock::AtomicValue<NormalizedState> fLatestState;
};

//------------------------------------------------------------------------
// RTState::add
//------------------------------------------------------------------------
template<typename ParamConverter>
RTParam<ParamConverter> RTState::add(ParamDefSPtr<ParamConverter> iParamDef)
{
  auto rtParam = std::make_shared<RTParameter<ParamConverter>>(iParamDef);
  addRawParameter(rtParam);
  return rtParam;
}

}
}
}
