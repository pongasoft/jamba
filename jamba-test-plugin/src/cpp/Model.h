/*
 * Copyright (c) 2019 pongasoft
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

#include <pongasoft/VST/ParamConverters.h>
#include <pongasoft/VST/ParamSerializers.h>
#include <pluginterfaces/base/ftypes.h>
#include <pongasoft/VST/AudioUtils.h>
#include <sstream>

namespace pongasoft::test::jamba {

using namespace Steinberg;
using namespace Steinberg::Vst;

enum class ETabs {
  kTabAll,
  kTabToggleButtonView,
  kTabMomentaryButtonView,
  kTabStepButtonView,
  kTabDiscreteButtonView,
  kTabTextButtonView,
};

enum EEnum {
  kEnum0,
  kEnum1,
  kEnum2,
  kEnum3
};

enum ENonContiguousEnum {
  kNonContiguousEnum0 = 10,
  kNonContiguousEnum1 = -2,
  kNonContiguousEnum2 = 20,
  kNonContiguousEnum3 = -4
};

enum class EEnumClass {
  kEnumClass0,
  kEnumClass1,
  kEnumClass2,
  kEnumClass3
};

/**
 * Converts a sample to its db value
 */
inline double sampleToDb(double valueInSample)
{
  return std::log10(valueInSample) * 20.0;
}

/**
 * @return a string representation of the sample in dB
 */
inline std::string toDbString(double iSample, int iPrecision)
{
  if(iSample < 0)
    iSample = -iSample;

  std::ostringstream s;

  if(iSample >= VST::Sample64SilentThreshold)
  {
    s.precision(iPrecision);
    s.setf(std::ios::fixed);
    s << std::showpos << sampleToDb(iSample) << "dB";
  }
  else
    s << "-oo";
  return s.str();
}

/**
 * Encapsulates the notion of gain
 */
class Gain
{
public:
  static constexpr double Unity = 1.0;
  static constexpr double Factor = 0.7;

  constexpr explicit Gain(double iValue = Unity) noexcept : fValue{iValue} {}

  inline double getValue() const { return fValue; }
  inline double getValueInDb() const { return sampleToDb(fValue); }
  inline ParamValue getNormalizedValue() const
  {
    // value = (gain ^ 1/3) * 0.7
    return std::pow(fValue, 1.0/3) * Factor;
  }

private:
  double fValue;
};

constexpr Gain DEFAULT_GAIN = Gain{};

/**
 * Gain Param Converter (for vst)
 */
class GainParamConverter : public VST::IParamConverter<Gain>
{
public:
  /**
   * Gain uses an x^3 curve with 0.7 (Param Value) being unity gain
   */
  Gain denormalize(ParamValue value) const override
  {
    if(std::fabs(value - Gain::Factor) < 1e-5)
      return Gain{};

    // gain = (value / 0.7) ^ 3
    double correctedGain = value / Gain::Factor;
    return Gain{correctedGain * correctedGain * correctedGain};
  }

  // normalize
  ParamValue normalize(Gain const &iGain) const override
  {
    return iGain.getNormalizedValue();
  }

  // toString
  inline void toString(ParamType const &iValue, String128 iString, int32 iPrecision) const override
  {
    auto s = toDbString(iValue.getValue(), iPrecision);
    Steinberg::UString wrapper(iString, str16BufferSize (String128));
    wrapper.fromAscii(s.c_str());
  }
};

/**
 * Gain Param Serializer (for jmb)
 */
class GainParamSerializer : public VST::IParamSerializer<Gain>
{
public:
  GainParamSerializer(int32 iPrecision = 4) : fPrecision{iPrecision} {}

  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    double gain;
    auto res = VST::IBStreamHelper::readDouble(iStreamer, gain);
    if(res == kResultOk)
    {
      oValue = Gain(gain);
      return kResultOk;
    }
    else
      return res;
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeDouble(iValue.getValue());
    return kResultOk;
  }

  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    oStream << toDbString(iValue.getValue(), fPrecision);
  }

protected:
  int32 fPrecision;
};

}