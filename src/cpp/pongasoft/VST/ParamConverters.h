#ifndef __PONGASOFT_VST_PARAM_CONVERTERS_H__
#define __PONGASOFT_VST_PARAM_CONVERTERS_H__

#include <pongasoft/Utils/Misc.h>

#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/base/ustring.h>
#include <pluginterfaces/base/ftypes.h>

#include <cmath>
#include <algorithm>

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
  virtual ParamValue normalize(ParamType const &iValue) const = 0;
  virtual ParamType denormalize(ParamValue iNormalizedValue) const = 0;
  virtual void toString(ParamType const &iValue, String128 iString, int32 iPrecision) const = 0;
};


/**
 * Wrapper/convenient class using a class containing static method instead.
 *
 * @tparam ParamConverter the type of the static class
 */
template<typename ParamConverter>
class StaticParamConverter : public IParamConverter<typename ParamConverter::ParamType>
{
public:
  using ParamType = typename ParamConverter::ParamType;
  inline ParamValue normalize(ParamType const &iValue) const override { return ParamConverter::normalize(iValue); }
  inline ParamType denormalize(ParamValue iNormalizedValue) const override { return ParamConverter::denormalize(iNormalizedValue); }
  inline void toString(ParamType const &iValue, String128 iString, int32 iPrecision) const override { return ParamConverter::toString(iValue, iString, iPrecision); }
};

/**
 * Simple function to create a param converter from a class with static methods
 */
template<typename ParamConverter>
inline static std::unique_ptr<StaticParamConverter<ParamConverter>> createParamConverter()
{
  return std::make_unique<StaticParamConverter<ParamConverter>>();
}

/**
 * This parameter is just a no-op wrapper to the ParamValue to adapt it to the use of the ParamConverter concept
 */
class RawParamConverter
{
public:
  using ParamType = ParamValue;

  inline static ParamValue normalize(ParamValue const &iValue)
  {
    return Utils::clamp(iValue, 0.0, 1.0);
  }

  inline static ParamValue denormalize(ParamValue iNormalizedValue)
  {
    return Utils::clamp(iNormalizedValue, 0.0, 1.0);;
  }

  inline static void toString(ParamValue const &iValue, String128 oString, int32 iPrecision)
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
class BooleanParamConverter
{
public:
  using ParamType = bool;

  inline static ParamValue normalize(bool const &iValue)
  {
    return iValue ? 1.0 : 0;
  }

  inline static bool denormalize(ParamValue iNormalizedValue)
  {
    return iNormalizedValue >= 0.5;
  }

  inline static void toString(bool const &iValue, String128 oString, int32 /* iPrecision */)
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
class PercentParamConverter
{
public:
  using ParamType = Percent;

  inline static ParamValue normalize(double const &iValue)
  {
    return Utils::clamp(iValue, 0.0, 1.0);
  }

  inline static double denormalize(ParamValue iNormalizedValue)
  {
    return Utils::clamp(iNormalizedValue, 0.0, 1.0);
  }

  inline static void toString(ParamType const &iValue, String128 oString, int32 iPrecision)
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
class DiscreteValueParamConverter
{
public:
  using ParamType = int;

  static inline int getStepCount() { return StepCount; }

  static inline ParamValue normalize(int const &iDiscreteValue)
  {
    auto value = Utils::clamp(iDiscreteValue, 0, StepCount);
    return value / static_cast<double>(StepCount);
  }

  static inline int denormalize(ParamValue iNormalizedValue)
  {
    // ParamValue must remain within its bounds
    auto value = Utils::clamp(iNormalizedValue, 0.0, 1.0);
    return static_cast<int>(std::floor(std::min(static_cast<ParamValue>(StepCount),
                                                value * (StepCount + 1))));
  }

  inline static void toString(ParamType const &iValue, String128 oString, int32 /* iPrecision */)
  {
    Steinberg::UString wrapper(oString, str16BufferSize (String128));
    if(!wrapper.printInt(iValue))
      oString[0] = 0;
  }
};

}
}

#endif // __PONGASOFT_VST_PARAM_CONVERTERS_H__