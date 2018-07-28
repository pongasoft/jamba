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
      DCHECK_LT_F(fChannel, iBuffers.getNumChannels());
    }

    /**
     * @return number of samples in the buffer
     */
    inline int32 getNumSamples() const
    {
      return fBuffers.getNumSamples();
    }

    /**
     * @return the underlying sample buffer
     */
    inline SampleType *getBuffer() const
    {
      return fBuffers.getBuffer()[fChannel];
    }

    /**
     * Sets a single channel silence flag
     */
    inline void setSilenceFlag(bool iSilent)
    {
      fBuffers.setSilenceFlag(fChannel, iSilent);
    }

  private:
    AudioBuffers &fBuffers;
    int32 fChannel;
  };

  typedef AudioBuffers<SampleType> class_type;


public:
  AudioBuffers(AudioBusBuffers &buffer, int32 numSamples) : fBuffer(buffer), fNumSamples(numSamples)
  {}

  // returns true if the buffer is silent (meaning all channels are silent => set to 1)
  inline bool isSilent() const
  {
    return fBuffer.silenceFlags == (static_cast<uint64>(1) << fBuffer.numChannels) - 1;
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

  /**
   * Sets a single channel silence flag
   */
  inline void setSilenceFlag(int32 iChannel, bool iSilent)
  {
    if(iSilent)
      BIT_SET(fBuffer.silenceFlags, iChannel);
    else
      BIT_CLEAR(fBuffer.silenceFlags, iChannel);
  }

  /**
   * @return the audio channel provided its channel (make sure that iChannel is <  getNumChannels!)
   */
  inline Channel getAudioChannel(int32 iChannel)
  {
    DCHECK_LT_F(iChannel, getNumChannels());
    return Channel{*this, iChannel};
  }

  /**
   * @return the left channel (using the fact that the left channel is 0)
   */
  inline Channel getLeftChannel()
  {
    return getAudioChannel(DEFAULT_LEFT_CHANNEL);
  }

  /**
   * @return the right channel (using the fact that the left channel is 1)
   */
  inline Channel getRightChannel()
  {
    return getAudioChannel(DEFAULT_RIGHT_CHANNEL);
  }

  // returns the underlying buffer
  inline SampleType **getBuffer() const;

  /**
   * @return number of channels (2 for stereo) of the underlying buffer
   */
  inline int32 getNumChannels() const
  { return fBuffer.numChannels; }

  /**
   * @return number of samples in the buffer
   */
  inline int32 getNumSamples() const
  { return fNumSamples; }

  /**
   * Copy the content of THIS buffer to the provided buffer (up to num samples)
   */
  inline tresult copyTo(class_type &toBuffer) const
  { return toBuffer.copyFrom(*this); };

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

    int32 numChannels = std::min(getNumChannels(), fromBuffer.getNumChannels());
    int32 numSamples = std::min(getNumSamples(), fromBuffer.getNumSamples());

    for(int32 channel = 0; channel < numChannels; channel++)
    {
      auto ptrFrom = fromSamples[channel];
      auto ptrTo = toSamples[channel];

      for(int i = 0; i < numSamples; ++i, ptrFrom++, ptrTo++)
      {
        *ptrTo = *ptrFrom;
      }
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

    for(int32 channel = 0; channel < getNumChannels(); channel++)
    {
      auto ptr = buffer[channel];

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

private:
  AudioBusBuffers &fBuffer;
  const int32 fNumSamples;
};

template<>
inline Sample32 **AudioBuffers<Sample32>::getBuffer() const
{ return fBuffer.channelBuffers32; }

template<>
inline Sample64 **AudioBuffers<Sample64>::getBuffer() const
{ return fBuffer.channelBuffers64; }

typedef AudioBuffers<Sample32> AudioBuffers32;
typedef AudioBuffers<Sample64> AudioBuffers64;

}
}