#pragma once

#include <base/source/timer.h>
#include <memory>

namespace pongasoft {
namespace VST {

/**
 * Simple wrapper class around Steinberg::Timer which will automatically release the timer on delete
 */
class AutoReleaseTimer
{
public:
  // Constructor
  AutoReleaseTimer(Steinberg::Timer *iTimer) : fTimer{iTimer} {}

  // Destructor => auto release
  ~AutoReleaseTimer()
  {
    stop();

    if(fTimer)
      fTimer->release();
  }

  // stops the timer
  inline void stop()
  {
    if(fTimer)
      fTimer->stop();
  }

  /**
   * Creates and return an auto release timer
   */
  static std::unique_ptr<AutoReleaseTimer> create(Steinberg::ITimerCallback* iCallback,
                                                  Steinberg::uint32 iIntervalMilliseconds)
  {
    return std::make_unique<AutoReleaseTimer>(Steinberg::Timer::create(iCallback, iIntervalMilliseconds));
  }

  // disabling copy
  AutoReleaseTimer(AutoReleaseTimer const &) = delete;
  AutoReleaseTimer& operator=(AutoReleaseTimer const &) = delete;

private:
  Steinberg::Timer *fTimer;
};

}
}