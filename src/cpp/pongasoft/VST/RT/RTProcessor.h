/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

#include <base/source/timer.h>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <pongasoft/VST/Timer.h>
#include "RTState.h"

namespace pongasoft {
namespace VST {
namespace RT {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * Base class from which the actual processor inherits from. Handles most of the "framework" logic,
 * (state loading/saving in a thread safe manner, setting up GUI thread if messaging to the GUI is required, etc...) so
 * that the actual processor code deals mostly with business logic.
 */
class RTProcessor : public AudioEffect, public IMessageProducer
{
public:
  explicit RTProcessor(Steinberg::FUID const &iControllerUID);

  ~RTProcessor() override = default;

  /**
   * Subclasses must implement this method to return the state
   */
  virtual RTState *getRTState() = 0;

  /** Called at first after constructor (setup input/output) */
  tresult PLUGIN_API initialize(FUnknown *context) override;

  /** Switch the Plug-in on/off */
  tresult PLUGIN_API setActive(TBool state) override;

  /** Here we go...the process call */
  tresult PLUGIN_API process(ProcessData &data) override;

  /** Asks if a given sample size is supported see \ref SymbolicSampleSizes. */
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) override;

  /** Restore the state (ex: after loading preset or project) */
  tresult PLUGIN_API setState(IBStream *state) override;

  /** Called to save the state (before saving a preset or project) */
  tresult PLUGIN_API getState(IBStream *state) override;

protected:
  /**
   * @return true if you can handle 32 bits (true buy default) */
  virtual bool canProcess32Bits() const { return true; }

  /**
   * @return true if you can handle 64 bits (true buy default) */
  virtual bool canProcess64Bits() const { return true; }

  /**
   * Processes inputs (step 2 always called after processing the parameters)
   * Delegate to processInputs32Bits or processInputs64Bits accordingly
   */
  virtual tresult processInputs(ProcessData &data);

  /**
   * Processes inputs (step 2 always called after processing the parameters) for 32 bits
   */
  virtual tresult processInputs32Bits(ProcessData &data) { return kResultOk; }

  /**
   * Processes inputs (step 2 always called after processing the parameters) for 64 bits
   */
  virtual tresult processInputs64Bits(ProcessData &data) { return kResultOk; }

  /**
   * Subclass will implement this method to respond to the GUI timer firing
   * /////// WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! //////
   * /////// this method WILL be called from the UI thread so do not modify the processor state  //////
   * /////// WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! WARNING !!!!! //////
   * */
  virtual void onGUITimer() {}

  /**
   * Call this method to enable the GUI timer (onGUITimer will be called at the specified frequency)
   * Should be called in the constructor or setupProcessing method as it will take effect in setActive
   */
  void enableGUITimer(uint32 iUIFrameRateMs);

  /**
   * Called (from a GUI timer) to send the messages to the GUI (SerParam for the moment) */
   virtual void sendPendingMessages() { getRTState()->sendPendingMessages(this); }

protected:
  // interval for gui message timer (can be changed by subclass BEFORE calling initialize)
  uint32 fGUIMessageTimerIntervalMs;

public:
  // allocateMessage
  IPtr<IMessage> allocateMessage() override;

  // sendMessage
  tresult sendMessage(IPtr<IMessage> iMessage) override;

private:
  using RTProcessorCallback = void (RTProcessor::*)();

  // wrapper class to dispatch the callback
  template<RTProcessorCallback Callback>
  class GUITimerCallback : public ITimerCallback
  {
  public:
    explicit GUITimerCallback(RTProcessor *iProcessor) : fProcessor{iProcessor} {}

    void onTimer(Timer *timer) override
    {
      (fProcessor->*Callback)();
    }
  private:
    RTProcessor *fProcessor;
  };



private:
  // the generic gui timer (enabled with enableGUITimer)
  GUITimerCallback<&RTProcessor::onGUITimer> fGUITimerCallback;
  uint32 fGUITimerIntervalMs;
  std::unique_ptr<AutoReleaseTimer> fGUITimer;

  // the timer that will handle sending messages (enabled when there are messages to handle)
  GUITimerCallback<&RTProcessor::sendPendingMessages> fGUIMessageTimerCallback;
  std::unique_ptr<AutoReleaseTimer> fGUIMessageTimer;

  bool fActive;

  int32 fSymbolicSampleSize = -1;
};

}
}
}
