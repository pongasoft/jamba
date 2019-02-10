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
#ifndef __PONGASOFT_VST_PARAM_CONVERTERS_H__
#define __PONGASOFT_VST_PARAM_CONVERTERS_H__

#include <pongasoft/Utils/Misc.h>
#include <pongasoft/VST/Types.h>

#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/base/ustring.h>
#include <base/source/fstring.h>
#include <pluginterfaces/base/ftypes.h>

#include <cmath>
#include <algorithm>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <type_traits>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * A vst parameter is represented by a ParamValue type which is a double in the range [0,1].
 * This interface represents a converter which knows how to convert from this normalized value to any other kind of
 * value (T)
 *
 * @tparam T the actual type that the vst parameter represents
 */
template<typename T>
class IParamConverter
{
public:
  using ParamType = T;
  virtual int32 getStepCount() const { return 0; }
  virtual ParamValue normalize(ParamType const &iValue) const = 0;
  virtual ParamType denormalize(ParamValue iNormalizedValue) const = 0;
  virtual void toString(ParamType const &iValue, String128 iString, int32 iPrecision) const { }
  virtual std::string toString(ParamType const &iValue, int32 iPrecision) const
  {
    String128 s;
    toString(iValue, s, iPrecision);
    return String(s).text8();
  }
};

/**
 * This parameter is just a no-op wrapper to the ParamValue to adapt it to the use of the ParamConverter concept
 */
class RawParamConverter : public IParamConverter<ParamValue>
{
public:

  using IParamConverter<ParamValue>::toString;

  inline ParamValue normalize(ParamValue const &iValue) const override
  {
    return Utils::clamp(iValue, 0.0, 1.0);
  }

  inline ParamValue denormalize(ParamValue iNormalizedValue) const override
  {
    return Utils::clamp(iNormalizedValue, 0.0, 1.0);;
  }

  inline void toString(ParamValue const &iValue, String128 oString, int32 iPrecision) const override
  {
    staticToString(iValue, oString, iPrecision);
  }

  static inline void staticToString(ParamValue const &iValue, String128 oString, int32 iPrecision)
  {
    Steinberg::UString wrapper(oString, str16BufferSize(String128));
    if(!wrapper.printFloat(iValue, iPrecision))
      oString[0] = 0;
  }
};

/**
 * Manages the very common case when a param represents a boolean value. To denormalize the range [0.0,1.0] this
 * implementation uses false for [0.0, 0.5[ and true for [0.5, 1.0] so that it matches a DiscreteValueParamConverter
 * with a step count of 1.
 */
class BooleanParamConverter : public IParamConverter<bool>
{
public:
  using IParamConverter<bool>::toString;

  explicit BooleanParamConverter(VstString16 iFalseString = STR16("Off"),
                                 VstString16 iTrueString = STR16("On")) :
    fFalseString{std::move(iFalseString)},
    fTrueString{std::move(iTrueString)}
  {}

  inline int32 getStepCount() const override { return 1; }

  inline ParamValue normalize(bool const &iValue) const override
  {
    return iValue ? 1.0 : 0;
  }

  inline bool denormalize(ParamValue iNormalizedValue) const override
  {
    return iNormalizedValue >= 0.5;
  }

  inline void toString(bool const &iValue, String128 oString, int32 /* iPrecision */) const override
  {
    Steinberg::UString wrapper(oString, str16BufferSize(String128));
    if(iValue)
      wrapper.assign(fTrueString.c_str());
    else
      wrapper.assign(fFalseString.c_str());
  }

protected:
  VstString16 fFalseString;
  VstString16 fTrueString;
};

/**
 * Percent type represented by a double
 */
using Percent = double;

/**
 * A trivial percent converter. The toString method returns the value as a percentage (precision is used to adjust
 * how many digits to use for display)
 */
class PercentParamConverter : public IParamConverter<Percent>
{
public:

  using IParamConverter<Percent>::toString;

  inline ParamValue normalize(double const &iValue) const override
  {
    return Utils::clamp(iValue, 0.0, 1.0);
  }

  inline double denormalize(ParamValue iNormalizedValue) const override
  {
    return Utils::clamp(iNormalizedValue, 0.0, 1.0);
  }

  inline void toString(ParamType const &iValue, String128 oString, int32 iPrecision) const override
  {
    Steinberg::UString wrapper(oString, str16BufferSize (String128));
    wrapper.printFloat(iValue * 100, iPrecision);
    wrapper.append(STR16("%"));
  }
};

/**
 * Implements the algorithm described in the VST documentation on how to interpret a
 * discrete value into a normalized value
 */
template<typename IntType = int>
static inline ParamValue convertDiscreteValueToNormalizedValue(int32 iStepCount, IntType iDiscreteValue)
{
  auto value = Utils::clamp<int32, int32>(iDiscreteValue, 0, iStepCount);
  if(value == 0)
    return value;
  else
    return value / static_cast<ParamValue>(iStepCount);
}

/**
 * Implements the algorithm described in the VST documentation on how to interpret a
 * normalized value as a discrete value
 */
template<typename IntType = int>
static inline IntType convertNormalizedValueToDiscreteValue(int32 iStepCount, ParamValue iNormalizedValue)
{
  // ParamValue must remain within its bounds
  auto value = Utils::clamp(iNormalizedValue, 0.0, 1.0);
  auto discreteValue = std::floor(std::min(static_cast<ParamValue>(iStepCount), value * (iStepCount + 1)));
  return static_cast<IntType>(discreteValue);
}

/**
 * A converter to deal with a discrete value which has StepCount steps. It follows the formulas given in the SDK
 * documentation. Note that the number of steps is always -1 from the number of values.
 * For example for 3 values (0, 1, 2) the number of steps is 2.
 */
template<int32 StepCount, typename IntType = int>
class DiscreteValueParamConverter : public IParamConverter<IntType>
{
public:
  using ParamType = IntType;

  using IParamConverter<IntType>::toString;

  // Constructor - you can provide an offset for the toString conversion (ex: counting from 1 instead of 0)
  explicit DiscreteValueParamConverter(IntType iToStringOffset = 0) : fToStringOffset{iToStringOffset} {}

  // Constructor with printf style format where the parameter (%d) will be (value + offset)
  explicit DiscreteValueParamConverter(VstString16 iFormat, IntType iToStringOffset = 0) :
    fToStringOffset{iToStringOffset}, fFormat{std::move(iFormat)} {}

  // Constructor with all values defined
  explicit DiscreteValueParamConverter(std::array<VstString16, StepCount + 1> const &iToStringValues) :
    fToStringValues(iToStringValues.cbegin(), iToStringValues.cend()) {}

  inline int32 getStepCount() const override { return StepCount; }

  inline ParamValue normalize(ParamType const &iDiscreteValue) const override
  {
    return convertDiscreteValueToNormalizedValue<IntType>(StepCount, iDiscreteValue);
  }

  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return convertNormalizedValueToDiscreteValue<IntType>(StepCount, iNormalizedValue);
  }

  // toString
  void toString(ParamType const &iValue, String128 oString, int32 /* iPrecision */) const override
  {
    Steinberg::UString wrapper(oString, str16BufferSize (String128));
    if(!fFormat.empty())
    {
      String s;
      s.printf(fFormat.c_str(), iValue + fToStringOffset);
      wrapper.assign(s.text());
    }
    else
    {
      if(fToStringValues.empty())
      {
        if(!wrapper.printInt(iValue + fToStringOffset))
          oString[0] = 0;
      }
      else
      {
        wrapper.assign(fToStringValues[iValue].c_str());
      }
    }
  }

private:
  IntType fToStringOffset{};
  VstString16 fFormat{};
  std::vector<VstString16> fToStringValues{};
};

/**
 * A converter to deal with an enum (assumes that the enum is contiguous, starts at 0 and that MaxValue is the latest
 * value in the enum)
 */
template<typename Enum, Enum MaxValue>
class EnumParamConverter : public IParamConverter<Enum>
{
public:
  using ParamType = Enum;

  using IntType = std::underlying_type_t<Enum>;

  using IParamConverter<Enum>::toString;

  // Constructor - you can provide an offset for the toString conversion (ex: counting from 1 instead of 0)
  explicit EnumParamConverter(IntType iToStringOffset = 0) : fConverter{iToStringOffset} {}

  // Constructor with printf style format where the parameter (%d) will be (value + offset)
  explicit EnumParamConverter(VstString16 iFormat, IntType iToStringOffset = 0) : fConverter{std::move(iFormat), iToStringOffset} {}

  // Constructor with all values defined
  explicit EnumParamConverter(std::array<VstString16, MaxValue + 1> const &iToStringValues) : fConverter{iToStringValues} {}

  inline int32 getStepCount() const override { return MaxValue; }

  inline ParamValue normalize(ParamType const &iDiscreteValue) const override
  {
    return fConverter.normalize(static_cast<IntType>(iDiscreteValue));
  }

  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return static_cast<Enum>(fConverter.denormalize(iNormalizedValue));
  }

  void toString(ParamType const &iValue, String128 oString, int32 iPrecision) const override
  {
    fConverter.toString(static_cast<IntType>(iValue), oString, iPrecision);
  }

private:
  DiscreteValueParamConverter<MaxValue, IntType> fConverter;
};

}
}

#endif // __PONGASOFT_VST_PARAM_CONVERTERS_H__