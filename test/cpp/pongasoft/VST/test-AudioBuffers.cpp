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
#include <pongasoft/VST/AudioBuffer.h>
#include <gtest/gtest.h>
#include <algorithm>

namespace pongasoft {
namespace VST {

/**
 * Takes care of allocating/deallocating memory
 */
struct InternalBuffer
{
  InternalBuffer(Steinberg::int32 iNumChannels, Steinberg::int32 iNumSamples) : fAudioBusBuffers{}, fNumSamples{iNumSamples}
  {
    fAudioBusBuffers.numChannels = iNumChannels;
    if(iNumChannels > 0)
    {
      fAudioBusBuffers.channelBuffers32 = new Sample32 *[iNumChannels]{};
      for(int i = 0; i < fAudioBusBuffers.numChannels; i++)
      {
        fAudioBusBuffers.channelBuffers32[i] = new Sample32[iNumSamples];
      }
    }

    AudioBuffers32 b{fAudioBusBuffers, fNumSamples};
    b.clear();
  }

  ~InternalBuffer()
  {
    if(fAudioBusBuffers.numChannels > 0)
    {
      for(int i = 0; i < fAudioBusBuffers.numChannels; i++)
        delete []fAudioBusBuffers.channelBuffers32[i];
      delete []fAudioBusBuffers.channelBuffers32;
    }
  }

  AudioBuffers32 toAudioBuffers() { return {fAudioBusBuffers, fNumSamples}; }

  AudioBusBuffers fAudioBusBuffers;
  Steinberg::int32 fNumSamples;
};

// AudioBuffers - testActive
TEST(AudioBuffers, testActive) {
  constexpr Steinberg::int32 NUM_SAMPLES = 64;

  InternalBuffer noChannels{0, NUM_SAMPLES};

  auto buf0 = noChannels.toAudioBuffers();

  ASSERT_EQ(nullptr, buf0.getBuffer());

  auto c0 = buf0.getLeftChannel();
  ASSERT_FALSE(c0.isActive());
  ASSERT_EQ(nullptr, c0.getBuffer());

  InternalBuffer oneChannel{1, NUM_SAMPLES};

  auto buf1 = oneChannel.toAudioBuffers();

  ASSERT_NE(nullptr, buf1.getBuffer());
  auto c1L = buf1.getLeftChannel();
  ASSERT_TRUE(c1L.isActive());
  ASSERT_NE(nullptr, c1L.getBuffer());

  auto c1R = buf1.getRightChannel();
  ASSERT_FALSE(c1R.isActive());
  ASSERT_EQ(nullptr, c1R.getBuffer());
}

// AudioBuffers - testCopy
TEST(AudioBuffers, testCopy) {
  constexpr Steinberg::int32 NUM_SAMPLES = 64;

  InternalBuffer noChannels{0, NUM_SAMPLES};
  InternalBuffer oneChannel{1, NUM_SAMPLES};
  InternalBuffer twoChannels{2, NUM_SAMPLES / 2};

  AudioBuffers32 noChannelsBuffers = noChannels.toAudioBuffers();
  AudioBuffers32 oneChannelBuffers = oneChannel.toAudioBuffers();
  AudioBuffers32 twoChannelBuffers = twoChannels.toAudioBuffers();

  for(int i = 0; i < oneChannel.fNumSamples; i++)
  {
    ASSERT_EQ(0, oneChannelBuffers.getLeftChannel().getBuffer()[i]);
  }

  for(int i = 0; i < twoChannels.fNumSamples; i++)
  {
    auto b = twoChannelBuffers;
    ASSERT_EQ(0, b.getLeftChannel().getBuffer()[i]);
    b.getLeftChannel().getBuffer()[i] = i;
    ASSERT_EQ(0, b.getRightChannel().getBuffer()[i]);
    b.getRightChannel().getBuffer()[i] = 100 - i;
  }

  // copying 2 channels into 1 => only channel is copied and since twoChannels has less samples
  // only the number of samples from twoChannels are copied
  ASSERT_EQ(kResultOk, oneChannelBuffers.copyFrom(twoChannelBuffers));

  for(int i = 0; i < oneChannel.fNumSamples / 2; i++)
  {
    ASSERT_EQ(i, oneChannelBuffers.getLeftChannel().getBuffer()[i]);
  }

  for(int i = oneChannel.fNumSamples / 2 + 1; i < oneChannel.fNumSamples; i++)
  {
    ASSERT_EQ(0, oneChannelBuffers.getLeftChannel().getBuffer()[i]);
  }

  // copying the right channel of twoChannelBuffers
  oneChannelBuffers.getLeftChannel().copyFrom(twoChannelBuffers.getRightChannel());

  for(int i = 0; i < oneChannel.fNumSamples / 2; i++)
  {
    ASSERT_EQ(100 - i, oneChannelBuffers.getLeftChannel().getBuffer()[i]);
  }

  for(int i = oneChannel.fNumSamples / 2 + 1; i < oneChannel.fNumSamples; i++)
  {
    ASSERT_EQ(0, oneChannelBuffers.getLeftChannel().getBuffer()[i]);
  }

  ASSERT_EQ(kResultFalse, oneChannelBuffers.copyFrom(noChannelsBuffers));
}

// AudioBuffers - testForEachSample
TEST(AudioBuffers, testForEachSample) {
  constexpr Steinberg::int32 NUM_SAMPLES = 64;

  InternalBuffer oneChannel{1, NUM_SAMPLES};

  AudioBuffers32 oneChannelBuffers = oneChannel.toAudioBuffers();

  for(int i = 0; i < oneChannel.fNumSamples; i++)
  {
    oneChannelBuffers.getLeftChannel().getBuffer()[i] = i;
  }

  // create a function operator which applies gain and compute max
  struct GainMaxOp
  {
    void operator()(Sample32 &iSample)
    {
      iSample *= fGain;
      fMax = std::max(fMax, iSample);
      fCount++;
    }

    double fGain{0.5};
    Sample32 fMax{0};
    int fCount{0};
  };

  auto res = oneChannelBuffers.getLeftChannel().forEachSample(GainMaxOp());

  ASSERT_EQ(NUM_SAMPLES, res.fCount); // called NUM_SAMPLES times
  ASSERT_EQ((NUM_SAMPLES - 1) * res.fGain, res.fMax); // max is the last sample (after gain)

  for(int i = 0; i < oneChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i * res.fGain, oneChannelBuffers.getLeftChannel().getBuffer()[i]); // each sample has been properly multiplied
  }

  // using a non existent channel
  res = oneChannelBuffers.getRightChannel().forEachSample(GainMaxOp());
  ASSERT_EQ(0, res.fCount); // should not be called at all
}

// AudioBuffers_Channel - testCopyFrom
TEST(AudioBuffers_Channel, testCopyFrom) {
  constexpr Steinberg::int32 NUM_SAMPLES = 64;

  InternalBuffer inChannel{1, NUM_SAMPLES};
  InternalBuffer outChannel{1, NUM_SAMPLES};

  AudioBuffers32 inBuffers = inChannel.toAudioBuffers();

  for(int i = 0; i < inChannel.fNumSamples; i++)
  {
    inBuffers.getLeftChannel().getBuffer()[i] = i;
  }

  ASSERT_EQ(NUM_SAMPLES - 1, inBuffers.absoluteMax());
  ASSERT_EQ(NUM_SAMPLES - 1, inBuffers.getLeftChannel().absoluteMax());

  AudioBuffers32 outBuffers = outChannel.toAudioBuffers();

  for(int i = 0; i < outChannel.fNumSamples; i++)
  {
    outBuffers.getLeftChannel().getBuffer()[i] = 0;
  }

  ASSERT_EQ(0, outBuffers.absoluteMax());

  // create a function operator which applies gain
  struct GainOp
  {
    Sample32 operator()(Sample32 iSample)
    {
      fCount++;
      return iSample * fGain;
    }

    Sample32 fGain{2};
    int fCount{0};
  };

  auto res = outBuffers.getLeftChannel().copyFrom(inBuffers.getLeftChannel(), GainOp());

  ASSERT_EQ(NUM_SAMPLES, res.fCount); // called NUM_SAMPLES times
  ASSERT_EQ(2.0, res.fGain); // sanity check

  // each sample has been properly multiplied
  for(int i = 0; i < outChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i * res.fGain, outBuffers.getLeftChannel().getBuffer()[i]);
  }

  ASSERT_EQ((NUM_SAMPLES - 1) * res.fGain, outBuffers.absoluteMax());

  // inChannel should be left untouched
  for(int i = 0; i < inChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i, inBuffers.getLeftChannel().getBuffer()[i]);
  }

  res = outBuffers.getRightChannel().copyFrom(inBuffers.getLeftChannel(), GainOp());
  ASSERT_EQ(0, res.fCount); // no out right channel

  res = outBuffers.getLeftChannel().copyFrom(inBuffers.getRightChannel(), GainOp());
  ASSERT_EQ(0, res.fCount); // no in right channel

  // test with a smaller input channel
  InternalBuffer inSmallChannel{1, 10};
  AudioBuffers32 inSmallBuffers = inSmallChannel.toAudioBuffers();
  for(int i = 0; i < inSmallChannel.fNumSamples; i++)
  {
    inSmallBuffers.getLeftChannel().getBuffer()[i] = i;
  }

  res = outBuffers.getLeftChannel().copyFrom(inSmallBuffers.getLeftChannel(), GainOp{3});
  ASSERT_EQ(inSmallChannel.fNumSamples, res.fCount);

  // make sure that the 10 first samples have been transformed
  for(int i = 0; i < inSmallChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i * 3, outBuffers.getLeftChannel().getBuffer()[i]);
  }

  // make sure that remaining samples remained has they were before the call
  for(int i = inSmallChannel.fNumSamples; i < outChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i * 2, outBuffers.getLeftChannel().getBuffer()[i]);
  }
}

// AudioBuffers - testCopyFrom
TEST(AudioBuffers, testCopyFrom) {
  constexpr Steinberg::int32 NUM_SAMPLES = 64;

  InternalBuffer inChannel{2, NUM_SAMPLES};
  InternalBuffer outChannel{2, NUM_SAMPLES};

  AudioBuffers32 inBuffers = inChannel.toAudioBuffers();

  for(int i = 0; i < inChannel.fNumSamples; i++)
  {
    inBuffers.getLeftChannel().getBuffer()[i] = i;
    inBuffers.getRightChannel().getBuffer()[i] = NUM_SAMPLES - i;
  }

  AudioBuffers32 outBuffers = outChannel.toAudioBuffers();
  outBuffers.clear();

  // create a function operator which applies gain
  struct GainOp
  {
    Sample32 operator()(Sample32 iSample)
    {
      fCount++;
      return iSample * fGain;
    }

    Sample32 fGain{2};
    int fCount{0};
  };

  auto res = outBuffers.copyFrom(inBuffers, GainOp());

  ASSERT_EQ(NUM_SAMPLES * 2, res.fCount); // called 2 * NUM_SAMPLES times (2 channels)
  ASSERT_EQ(2.0, res.fGain); // sanity check

  // each sample has been properly multiplied
  for(int i = 0; i < outChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i * res.fGain, outBuffers.getLeftChannel().getBuffer()[i]);
    ASSERT_EQ((NUM_SAMPLES - i) * res.fGain, outBuffers.getRightChannel().getBuffer()[i]);
  }

  // inChannel should be left untouched
  for(int i = 0; i < inChannel.fNumSamples; i++)
  {
    ASSERT_EQ(i, inBuffers.getLeftChannel().getBuffer()[i]);
    ASSERT_EQ(NUM_SAMPLES - i, inBuffers.getRightChannel().getBuffer()[i]);
  }
}

}
}