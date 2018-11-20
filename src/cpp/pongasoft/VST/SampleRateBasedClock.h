/*
 * Copyright (c) 2018 pongasoft
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

#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <cmath>
#include <chrono>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * The purpose of this class is to deal with timing based on the sample rate and do the proper conversions.
 */
class SampleRateBasedClock
{
public:
  /**
   * Keeps track of the time in number of samples processed vs sample rate. Typical usage is:
   *
   * // in setup
   * rateLimiter = clock.getRateLimiter(250); // 4 times a second
   *
   * // in process (every frame)
   * if(rateLimiter.shouldUpdate(processData.numSamples))
   * {
   *   // execute rate limited code (ex: update UI)
   * }
   */
  class RateLimiter
  {
  public:
    explicit RateLimiter(uint32 iRateLimitInSamples = 0) : fRateLimitInSamples{iRateLimitInSamples}, fSampleCount{0}
    {}

    /**
     * Calls this method when a new batch of samples is processed and returns true if the limit (in samples) is
     * achieved
     */
    bool shouldUpdate(uint32 numSamples)
    {
      fSampleCount += numSamples;
      if(fSampleCount >= fRateLimitInSamples)
      {
        fSampleCount -= fRateLimitInSamples;
        return true;
      }
      return false;
    }

  private:
    uint32 fRateLimitInSamples;
    uint32 fSampleCount;
  };

  // Constructor
  explicit SampleRateBasedClock(SampleRate iSampleRate) : fSampleRate{iSampleRate}
  {

  }

  // getSampleCountFor: how many samples for iMillis seconds
  uint32 getSampleCountFor(uint32 iMillis) const
  {
    return static_cast<uint32>(ceil(fSampleRate * iMillis / 1000.0));
  }

  // getSampleCountFor1Bar: how many samples for 1 bar (given the tempo and optional time signature)
  uint32 getSampleCountFor1Bar(double iTempo, int32 iTimeSigNumerator = 4, int32 iTimeSigDenominator = 4) const
  {
    auto oneBarInSeconds = 240.0 / iTempo;
    if(iTimeSigNumerator != iTimeSigDenominator && iTimeSigDenominator != 0)
    {
      oneBarInSeconds *= (static_cast<double>(iTimeSigNumerator) / static_cast<double>(iTimeSigDenominator));
    }
    return static_cast<uint32>(ceil(fSampleRate * oneBarInSeconds));
  }

  // getTimeForSampleCount: inverse of getSampleCountFor
  uint32 getTimeForSampleCount(uint32 iSampleCount) const
  {
    return static_cast<uint32>(ceil(iSampleCount * 1000.0 / fSampleRate));
  }

  // getSampleRate
  SampleRate getSampleRate() const
  {
    return fSampleRate;
  }

  // setSampleRate
  void setSampleRate(SampleRate iSampleRate)
  {
    fSampleRate = iSampleRate;
  }

  // getRateLimiter
  RateLimiter getRateLimiter(uint32 iMillis) const
  {
    return RateLimiter{getSampleCountFor(iMillis)};
  }

private:
  SampleRate fSampleRate;
};

}
}
