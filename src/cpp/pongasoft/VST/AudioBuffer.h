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

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pongasoft/logging/logging.h>
#include <algorithm>

#include "AudioUtils.h"

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

// defines the default setup channels (0 is left, 1 is right)
constexpr int32 DEFAULT_LEFT_CHANNEL = 0;
constexpr int32 DEFAULT_RIGHT_CHANNEL = 1;

/**
 * Represents all the buffers (example for a stereo channel there is 2 underlying sample buffers)
 *
 * @tparam SampleType
 */
template<typename SampleType>
class AudioBuffers
{
public:
  class Channel
  {
  public:
    Channel(AudioBuffers &iBuffers, int32 iChannel) : fBuffers{iBuffers}, fChannel{iChannel}
    {
    }

    /**
     * @return number of samples in the buffer
     */
    inline int32 getNumSamples() const
    {
      return fBuffers.getNumSamples();
    }

    /**
     * Note that this pointer is NOT guaranteed to be not null as demonstrated by this piece of logic in
     * the Audio Unit wrapper code:
     * ...
			 processData.inputs[i].channelBuffers32[channel] =
			   input->IsActive () ? (Sample32*)input->GetBufferList ().mBuffers[channel].mData : 0;
     * ...
     * @return the underlying sample buffer or nullptr if not active
     */
    inline SampleType *getBuffer() const
    {
      return fChannel < fBuffers.getNumChannels() ? fBuffers.getBuffer()[fChannel] : nullptr;
    }

    /**
     * @return true if this channel is silent (according to silenceFlags)
     */
    inline bool isSilent() const { return fBuffers.isSilent(fChannel); }

    /**
     * Sets a single channel silence flag
     */
    inline void setSilenceFlag(bool iSilent)
    {
      fBuffers.setSilenceFlag(fChannel, iSilent);
    }

    /**
     * @return true if this channel is active, meaning
     */
    inline bool isActive() const
    {
      return fChannel < fBuffers.getNumChannels() && getBuffer() != nullptr;
    }

    /**
     * Copy the content of THIS channel to the provided channel (up to num samples)
     */
    inline tresult copyTo(Channel &toChannel) const { return toChannel.copyFrom(*this); };

    /**
     * Copy the content of the provided channel to THIS channel (up to num samples)
     */
    tresult copyFrom(Channel const &fromChannel)
    {
      int32 numSamples = std::min(getNumSamples(), fromChannel.getNumSamples());

      auto ptrFrom = fromChannel.getBuffer();
      auto ptrTo = getBuffer();

      // sanity check
      if(!ptrFrom || !ptrTo)
        return kResultFalse;

      std::copy(ptrFrom, ptrFrom + numSamples, ptrTo);

      return kResultOk;
    }

    /**
     * Clears the channel (and sets the silence flag) */
    inline void clear()
    {
      auto buffer = getBuffer();

      // sanity check
      if(!buffer)
        return;

      std::fill(buffer, buffer + getNumSamples(), 0);

      setSilenceFlag(true);
    }

  private:
    AudioBuffers &fBuffers;
    int32 fChannel;
  };

  typedef AudioBuffers<SampleType> class_type;


public:
  AudioBuffers(AudioBusBuffers &buffer, int32 numSamples) : fBuffer(buffer), fNumSamples(numSamples)
  {}

  // returns true if the buffer is silent (meaning all channels are silent => set to 1) (according to silenceFlags)
  inline bool isSilent() const
  {
    return fBuffer.numChannels == 0 || fBuffer.silenceFlags == (static_cast<uint64>(1) << fBuffer.numChannels) - 1;
  }

  /**
   * Computes and adjust the silence flags
   * @return true if the buffer is silent (meaning all channels are silent)
   */
  inline bool adjustSilenceFlags()
  {
    uint64 silenceFlags = 0;

    auto buffer = getBuffer();

    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      bool silent = true;

      auto ptr = buffer[channel];

      if(!ptr)
        continue;

      for(int j = 0; j < getNumSamples(); ++j, ptr++)
      {
        auto sample = *ptr;

        if(silent && !pongasoft::VST::isSilent(sample))
          silent = false;
      }

      if(silent)
        BIT_SET(silenceFlags, channel);
    }

    fBuffer.silenceFlags = silenceFlags;

    return isSilent();
  }

  // getSilenceFlags
  inline uint64 getSilenceFlags() const { return fBuffer.silenceFlags; }

  // setSilenceFlags
  inline void setSilenceFlags(uint64 iFlags) const { fBuffer.silenceFlags = iFlags; }

  /**
   * Sets a single channel silence flag
   */
  inline void setSilenceFlag(int32 iChannel, bool iSilent)
  {
    if(iChannel < getNumChannels())
    {
      if(iSilent)
        BIT_SET(fBuffer.silenceFlags, iChannel);
      else
        BIT_CLEAR(fBuffer.silenceFlags, iChannel);
    }
  }

  /**
   * @return true if the channel is silent (according to silenceFlags)
   */
  inline bool isSilent(int32 iChannel) const
  {
    if(iChannel < getNumChannels())
    {
      return BIT_TEST(fBuffer.silenceFlags, iChannel);
    }
    return true;
  }

  /**
   * @return the audio channel provided its channel (make sure that iChannel is <  getNumChannels!)
   */
  inline Channel getAudioChannel(int32 iChannel)
  {
    return Channel{*this, iChannel};
  }

  /**
   * @return the left channel (using the fact that the left channel is 0)
   */
  inline Channel getLeftChannel() { return getAudioChannel(DEFAULT_LEFT_CHANNEL); }

  /**
   * @return the right channel (using the fact that the left channel is 1)
   */
  inline Channel getRightChannel() { return getAudioChannel(DEFAULT_RIGHT_CHANNEL); }

  // returns the underlying buffer
  inline SampleType **getBuffer() const;

  /**
   * @return number of channels (2 for stereo) of the underlying buffer
   */
  inline int32 getNumChannels() const { return fBuffer.numChannels; }

  /**
   * @return number of samples in the buffer
   */
  inline int32 getNumSamples() const { return fNumSamples; }

  /**
   * Copy the content of THIS buffer to the provided buffer (up to num samples)
   */
  inline tresult copyTo(class_type &toBuffer) const { return toBuffer.copyFrom(*this); };

  /**
   * Copy the content of the provided buffer to THIS buffer (up to num samples)
   */
  tresult copyFrom(class_type const &fromBuffer)
  {
    SampleType **fromSamples = fromBuffer.getBuffer();
    SampleType **toSamples = getBuffer();

    // there are cases when the 2 buffers could be identical.. no need to copy
    if(fromSamples == toSamples)
      return kResultOk;

    // sanity check
    if(!fromSamples || !toSamples)
      return kResultFalse;

    int32 numChannels = std::min(getNumChannels(), fromBuffer.getNumChannels());
    int32 numSamples = std::min(getNumSamples(), fromBuffer.getNumSamples());

    for(int32 channel = 0; channel < numChannels; channel++)
    {
      auto ptrFrom = fromSamples[channel];
      auto ptrTo = toSamples[channel];

      // sanity check
      if(!ptrFrom || !ptrTo)
        continue;

      std::copy(ptrFrom, ptrFrom + numSamples, ptrTo);
    }

    return kResultOk;
  }

  /**
   * @return the max sample (absolute) across all channels
   */
  inline SampleType absoluteMax()
  {
    SampleType max = 0;

    auto buffer = getBuffer();

    // sanity check
    if(!buffer)
      return max;

    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      auto ptr = buffer[channel];

      // sanity check
      if(!ptr)
        continue;

      for(int j = 0; j < getNumSamples(); ++j, ptr++)
      {
        auto sample = *ptr;
        if(sample < 0)
          sample -= sample;

        max = std::max(max, sample);
      }
    }

    return max;
  }

  /**
   * Clears the buffer (and sets the silence flag) */
  inline tresult clear()
  {
    auto buffer = getBuffer();

    // sanity check
    if(!buffer)
      return kResultFalse;

    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      auto ptr = buffer[channel];

      // sanity check
      if(!ptr)
        continue;

      std::fill(ptr, ptr + getNumSamples(), 0);
    }

    if(getNumChannels() > 0)
      fBuffer.silenceFlags == (static_cast<uint64>(1) << getNumChannels()) - 1;

    return kResultOk;
  }

private:
  AudioBusBuffers &fBuffer;
  const int32 fNumSamples;
};

template<>
inline Sample32 **AudioBuffers<Sample32>::getBuffer() const { return fBuffer.channelBuffers32; }

template<>
inline Sample64 **AudioBuffers<Sample64>::getBuffer() const { return fBuffer.channelBuffers64; }

typedef AudioBuffers<Sample32> AudioBuffers32;
typedef AudioBuffers<Sample64> AudioBuffers64;

}
}