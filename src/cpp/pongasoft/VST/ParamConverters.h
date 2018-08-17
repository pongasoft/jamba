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
#ifndef __PONGASOFT_VST_PARAM_CONVERTERS_H__
#define __PONGASOFT_VST_PARAM_CONVERTERS_H__

#include <pongasoft/Utils/Misc.h>

#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/base/ustring.h>
#include <pluginterfaces/base/ftypes.h>

#include <cmath>
#include <algorithm>
#include <memory>

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
  virtual void toString(ParamType const &iValue, String128 iString, int32 iPrecision) const
  {
  }
};

/**
 * This parameter is just a no-op wrapper to the ParamValue to adapt it to the use of the ParamConverter concept
 */
class RawParamConverter : public IParamConverter<ParamValue>
{
public:

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
    Steinberg::UString wrapper(oString, str16BufferSize (String128));
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
      wrapper.assign(STR16("On"));
    else
      wrapper.assign(STR16("Off"));
  }
};

/**
 * Percent type represented by a double
 */
using Percent = double;

/**
 * A trivial percent converter. The toString method returns the value as a percentage (precision is used to adjust
 * how many digits to use for display)
 */
class PercentParamConverter : IParamConverter<Percent>
{
public:

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
class DiscreteValueParamConverter : IParamConverter<int>
{
public:
  using ParamType = int;

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

}
}

#endif // __PONGASOFT_VST_PARAM_CONVERTERS_H__