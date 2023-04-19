/*
 * Copyright (c) 2018-2019 pongasoft
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
  /**
   * Unary operator adapter for computing the absolute max
   */
  struct AbsoluteMaxOp
  {
    void operator()(SampleType iSample)
    {
      fAbsoluteMax = std::max(fAbsoluteMax, iSample < 0 ? -iSample : iSample);
    }

    SampleType fAbsoluteMax = 0;
  };

  /**
   * Represents a single channel (for example left audio channel).
   *
   * Note an instance of this class may represent an invalid/non active channel in which case
   * the various methods will react accordingly (ex: `getBuffer` returns `nullptr`,
   * `forEachSample` doesn't do anything, etc...)
   */
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
     *
     *     ...
		 *	   processData.inputs[i].channelBuffers32[channel] =
		 *     input->IsActive () ? (Sample32*)input->GetBufferList ().mBuffers[channel].mData : 0;
     *     ...
     *
     * @return the underlying sample buffer or `nullptr` if not active
     */
    inline SampleType *getBuffer()
    {
      return fChannel < fBuffers.getNumChannels() ? fBuffers.getBuffer()[fChannel] : nullptr;
    }

    /**
     * Note that this pointer is NOT guaranteed to be not null as demonstrated by this piece of logic in
     * the Audio Unit wrapper code:
     *
     *     ...
     *	   processData.inputs[i].channelBuffers32[channel] =
     *     input->IsActive () ? (Sample32*)input->GetBufferList ().mBuffers[channel].mData : 0;
     *     ...
     *
     * @return the underlying sample buffer or `nullptr` if not active
     */
    inline SampleType const *getBuffer() const
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
     * @return true if this channel is active, meaning there is such a channel and its buffer is not null
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
     * @return the max sample (absolute) for this channel
     */
    inline SampleType absoluteMax() const
    {
      return forEachSample(AbsoluteMaxOp()).fAbsoluteMax;
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

    /**
     * Applies the provided unary function to each sample (if the channel is active).
     *
     * Example:
     *
     *     double gain = 0.5;
     *     leftChannel.forEachSample([gain](SampleType &iSample) { iSample *= gain; });
     *
     * @tparam UnaryFunction can be a lambda, a function object (etc...) but should provide an
     *                       api similar to `std::function<void(SampleType)>` or `std::function<void(SampleType &)>`
     * @return the function (similar api as `std::for_each`)
     */
    template<typename UnaryFunction>
    inline UnaryFunction forEachSample(UnaryFunction f)
    {
      auto buffer = getBuffer();

      if(buffer)
      {
        return std::for_each(buffer, buffer + getNumSamples(), f);
      }
      else
        return f;
    }

    /**
     * Applies the provided unary function to each sample (if the channel is active). Because the method is const,
     * the unary function cannot modify the elements.
     *
     * Example:
     *
     *     auto max = leftChannel.forEachSample(AbsoluteMaxOp()).fAbsoluteMax;
     *
     * @tparam UnaryFunction can be a lambda, a function object (etc...) but should provide an
     *                       api similar to `std::function<void(SampleType)>` or `std::function<void(SampleType const &)>`
     * @return the function (similar api as `std::for_each`)
     */
    template<typename UnaryFunction>
    inline UnaryFunction forEachSample(UnaryFunction f) const
    {
      auto buffer = getBuffer();

      if(buffer)
      {
        return std::for_each(buffer, buffer + getNumSamples(), f);
      }
      else
        return f;
    }

    /**
     * Copy `iFromChannel` to this channel, applying `f` to each sample or in other words, for each sample
     *
     *     getBuffer[i] = f(iFromChannel.getBuffer[i]);
     *
     * This handles properly inactive channels (nothing done if either of the channel is inactive) and channels
     * of different sizes.
     *
     * Example:
     *
     *     AudioBuffers<SampleType> in(data.inputs[0], data.numSamples);
     *     AudioBuffers<SampleType> out(data.outputs[0], data.numSamples);
     *
     *     double gain = 0.5;
     *     out.getLeftChannel().copyFrom(in.getLeftChannel(), [gain](SampleType iSample) { return iSample * gain; });
     *
     * @tparam UnaryOperation can be a lambda, a function object (etc...) but should provide an
     *                        api similar to `std::function<SampleType(SampleType)>`
     * @return the function provided
     */
    template<typename UnaryOperation>
    inline UnaryOperation copyFrom(Channel const &iFromChannel, UnaryOperation f)
    {
      auto outputBuffer = getBuffer();
      auto inputBuffer = iFromChannel.getBuffer();

      if(outputBuffer && inputBuffer)
      {
        int32 const numSamples = std::min(getNumSamples(), iFromChannel.getNumSamples());

        // implementation note: adding 'UnaryOperation &' as the template type forces f to be passed
        // by reference otherwise it would be copied which would loose the state (ex: for functors).
        std::transform<decltype(inputBuffer), decltype(outputBuffer), UnaryOperation &>(inputBuffer, inputBuffer + numSamples, outputBuffer, f);
      }

      return f;
    }

    /**
     * Same as `copyFrom` with the roles reversed
     */
    template<typename UnaryOperation>
    inline UnaryOperation copyTo(Channel &oToChannel, UnaryOperation f) const
    {
      return oToChannel.copyFrom(*this, f);
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
   * Makes the buffer NON silent (by setting the flag to 0). A flag is set to 1 when silent, so when set to 0 it is non
   * silent...
   */
  inline void clearSilentFlag()
  {
    fBuffer.silenceFlags = 0;
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
   * Return the audio channel for the provided channel. Note that this method never fails and will return an object
   * where Channel::isActive returns `false` in the event the channel is not active.
   *
   * @return the audio channel provided its channel
   */
  inline Channel getAudioChannel(int32 iChannel)
  {
    return Channel{*this, iChannel};
  }

  /**
   * Return the audio channel for the provided channel. Note that this method never fails and will return an object
   * where Channel::isActive returns `false` in the event the channel is not active.
   *
   * @return the audio channel provided its channel
   */
  inline const Channel getAudioChannel(int32 iChannel) const
  {
    // implementation note: removing const since Channel accepts only non const
    return Channel{*const_cast<class_type *>(this), iChannel};
  }

  /**
   * @return the left channel (using the fact that the left channel is 0)
   */
  inline Channel getLeftChannel() { return getAudioChannel(DEFAULT_LEFT_CHANNEL); }

  /**
   * @return the left channel (using the fact that the left channel is 0)
   */
  inline const Channel getLeftChannel() const { return getAudioChannel(DEFAULT_LEFT_CHANNEL); }

  /**
   * @return the right channel (using the fact that the left channel is 1)
   */
  inline Channel getRightChannel() { return getAudioChannel(DEFAULT_RIGHT_CHANNEL); }

  /**
   * @return the right channel (using the fact that the left channel is 1)
   */
  inline const Channel getRightChannel() const { return getAudioChannel(DEFAULT_RIGHT_CHANNEL); }

  //! Returns the underlying (sample) buffer
  inline SampleType **getBuffer();

  // returns the underlying (sample) buffer (const version)
  inline SampleType const * const *getBuffer() const;

  //! Returns the `AudioBusBuffers` original buffer
  inline AudioBusBuffers &getAudioBusBuffers() { return fBuffer; }

  //! Returns the `AudioBusBuffers` original buffer (const version)
  inline AudioBusBuffers const &getAudioBusBuffers() const { return fBuffer; }

  /**
   * @return number of channels (2 for stereo) of the underlying buffer
   */
  inline int32 getNumChannels() const { return fBuffer.numChannels; }

  /**
   * @return number of samples in the buffer
   */
  inline int32 getNumSamples() const { return fNumSamples; }

  /**
   * Applies the provided unary function to each sample of each channel
   *
   * Example:
   *
   *     double gain = 0.5;
   *     out.forEachSample([gain](SampleType &iSample) { iSample *= gain; });
   *
   * @tparam UnaryFunction can be a lambda, a function object (etc...) but should provide an
   *                       api similar to `std::function<void(SampleType)>` or `std::function<void(SampleType &)>`
   * @return the function (similar api as `std::for_each`)
   */
  template<typename UnaryFunction>
  inline UnaryFunction forEachSample(UnaryFunction f)
  {
    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      f = getAudioChannel(channel).forEachSample(f);
    }

    return f;
  }

  /**
   * Applies the provided unary function to each sample of each channel. Because the method is const, the unary function
   * cannot modify the elements.
   *
   * Example:
   *
   *     auto max = out.forEachSample(AbsoluteMaxOp()).fAbsoluteMax;
   *
   * @tparam UnaryFunction can be a lambda, a function object (etc...) but should provide an
   *                       api similar to `std::function<void(SampleType)>` or `std::function<void(SampleType const &)>`
   * @return the function (similar api as `std::for_each`)
   */
  template<typename UnaryFunction>
  inline UnaryFunction forEachSample(UnaryFunction f) const
  {
    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      f = getAudioChannel(channel).forEachSample(f);
    }

    return f;
  }

  /**
   * Copy the content of THIS buffer to the provided buffer (up to num samples)
   */
  inline tresult copyTo(class_type &toBuffer) const { return toBuffer.copyFrom(*this); };

  /**
   * Copy the content of the provided buffer to THIS buffer (up to num samples)
   */
  tresult copyFrom(class_type const &fromBuffer)
  {
    auto fromSamples = fromBuffer.getBuffer();
    auto toSamples = getBuffer();

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
   * Copy `iFromBuffer` to this buffer, applying `f` to each sample for each channel or in other words,
   * for each channel c and each sample i
   *
   *     getChannel(c).getBuffer[i] = f(iFromBuffer.getChannel(c).getBuffer[i]);
   *
   * Example:
   *
   *     AudioBuffers<SampleType> in(data.inputs[0], data.numSamples);
   *     AudioBuffers<SampleType> out(data.outputs[0], data.numSamples);
   *
   *     double gain = 0.5;
   *     out.copyFrom(in, [gain](SampleType iSample) { return iSample * gain; });
   *
   * @tparam UnaryOperation can be a lambda, a function object (etc...) but should provide an
   *                        api similar to `std::function<SampleType(SampleType)>`
   * @return the function provided
   */
  template<typename UnaryOperation>
  inline UnaryOperation copyFrom(class_type const &iFromBuffer, UnaryOperation f)
  {
    int32 numChannels = std::min(getNumChannels(), iFromBuffer.getNumChannels());

    for(int32 channel = 0; channel < numChannels; channel++)
    {
      f = getAudioChannel(channel).copyFrom(iFromBuffer.getAudioChannel(channel), f);
    }

    return f;
  }

  /**
   * Same as `copyFrom` with the roles reversed
   */
  template<typename UnaryOperation>
  inline UnaryOperation copyTo(class_type &iToBuffer, UnaryOperation f) const { return iToBuffer.copyFrom(*this, f); };

  /**
   * @return the max sample (absolute) across all channels
   */
  inline SampleType absoluteMax() const
  {
    return forEachSample(AbsoluteMaxOp()).fAbsoluteMax;
  }

  /**
   * Clears the buffer (and sets the silence flag) */
  inline tresult clear()
  {
    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      getAudioChannel(channel).clear();
    }

    return kResultOk;
  }

private:
  AudioBusBuffers &fBuffer;
  const int32 fNumSamples;
};

template<>
inline Sample32 const * const *AudioBuffers<Sample32>::getBuffer() const { return fBuffer.channelBuffers32; }

template<>
inline Sample64 const * const *AudioBuffers<Sample64>::getBuffer() const { return fBuffer.channelBuffers64; }

template<>
inline Sample32 **AudioBuffers<Sample32>::getBuffer() { return fBuffer.channelBuffers32; }

template<>
inline Sample64 **AudioBuffers<Sample64>::getBuffer() { return fBuffer.channelBuffers64; }

typedef AudioBuffers<Sample32> AudioBuffers32;
typedef AudioBuffers<Sample64> AudioBuffers64;

}
}