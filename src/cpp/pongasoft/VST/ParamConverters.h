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
#ifndef __PONGASOFT_VST_PARAM_CONVERTERS_H__
#define __PONGASOFT_VST_PARAM_CONVERTERS_H__

#include <pongasoft/Utils/Misc.h>

#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/base/ustring.h>
#include <base/source/fstring.h>
#include <pluginterfaces/base/ftypes.h>

#include <cmath>
#include <algorithm>
#include <memory>
#include <string>

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
  virtual int getStepCount() const { return 0; }
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

  explicit BooleanParamConverter(char16 const *iFalseString = STR16("Off"),
                                 char16 const *iTrueString = STR16("On")) :
    fFalseString{iFalseString},
    fTrueString{iTrueString}
  {}

  inline int getStepCount() const override { return 1; }

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
      wrapper.assign(fTrueString);
    else
      wrapper.assign(fFalseString);
  }

protected:
  char16 const *fFalseString;
  char16 const *fTrueString;
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
 * A converter to deal with a discrete value which has StepCount steps. It follows the formulas given in the SDK
 * documentation.
 */
template<int StepCount>
class DiscreteValueParamConverter : public IParamConverter<int>
{
public:
  using ParamType = int;

  using IParamConverter<int>::toString;

  inline int getStepCount() const override { return StepCount; }

  inline ParamValue normalize(int const &iDiscreteValue) const override
  {
    auto value = Utils::clamp(iDiscreteValue, 0, StepCount);
    return value / static_cast<double>(StepCount);
  }

  inline int denormalize(ParamValue iNormalizedValue) const override
  {
    // ParamValue must remain within its bounds
    auto value = Utils::clamp(iNormalizedValue, 0.0, 1.0);
    return static_cast<int>(std::floor(std::min(static_cast<ParamValue>(StepCount),
                                                value * (StepCount + 1))));
  }

  void toString(ParamType const &iValue, String128 oString, int32 /* iPrecision */) const override
  {
    Steinberg::UString wrapper(oString, str16BufferSize (String128));
    if(!wrapper.printInt(iValue))
      oString[0] = 0;
  }
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

  using IParamConverter<Enum>::toString;

  inline int getStepCount() const override { return fConverter.getStepCount(); }

  inline ParamValue normalize(ParamType const &iDiscreteValue) const override
  {
    return fConverter.normalize(iDiscreteValue);
  }

  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return static_cast<ParamType>(fConverter.denormalize(iNormalizedValue));
  }

  void toString(ParamType const &iValue, String128 oString, int32 iPrecision) const override
  {
    fConverter.toString(iValue, oString, iPrecision);
  }

private:
  DiscreteValueParamConverter<MaxValue> fConverter;
};

}
}

#endif // __PONGASOFT_VST_PARAM_CONVERTERS_H__