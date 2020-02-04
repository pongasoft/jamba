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

#include <pongasoft/Utils/Misc.h>
#include <pongasoft/VST/Types.h>
#include <pongasoft/VST/VstUtils/Utils.h>

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
#include <map>
#include <type_traits>
#include <pongasoft/Utils/Constants.h>

namespace pongasoft::VST {

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
    s[0] = 0;
    toString(iValue, s, iPrecision);
    return VstUtils::toUT8String(s);
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
    return Utils::clamp(iNormalizedValue, 0.0, 1.0);
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
    return toBoolean(iNormalizedValue);
  }

  inline void toString(bool const &iValue, String128 oString, int32 /* iPrecision */) const override
  {
    Steinberg::UString wrapper(oString, str16BufferSize(String128));
    if(iValue)
      wrapper.assign(fTrueString.c_str());
    else
      wrapper.assign(fFalseString.c_str());
  }

  /**
   * Converts a normalized value to a boolean according to the rule: `false` for `[0.0, 0.5[` and `true`
   * for `[0.5, 1.0]` */
  inline static bool toBoolean(ParamValue iNormalizedValue) { return iNormalizedValue >= 0.5; }

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
static inline ParamValue convertDiscreteValueToNormalizedValue(int32 iStepCount, int32 iDiscreteValue)
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
static inline int32 convertNormalizedValueToDiscreteValue(int32 iStepCount, ParamValue iNormalizedValue)
{
  // ParamValue must remain within its bounds
  auto value = Utils::clamp(iNormalizedValue, 0.0, 1.0);
  auto discreteValue = std::floor(std::min(static_cast<ParamValue>(iStepCount), value * (iStepCount + 1)));
  return static_cast<int32>(discreteValue);
}

/**
 * A converter to deal with a discrete value which has StepCount steps. It follows the formulas given in the SDK
 * documentation. Note that the number of steps is always -1 from the number of values.
 * For example for 3 values (0, 1, 2) the number of steps is 2.
 */
template<int32 StepCount, typename IntType = int32>
class DiscreteValueParamConverter : public IParamConverter<IntType>
{
public:
  using ParamType = IntType;

  using IParamConverter<IntType>::toString;

  /**
   * Defines the type for the constructor argument. Example: `{ STR16("abc"), ... }` */
  using ConstructorType = std::array<VstString16, StepCount + 1> const &;

  // Constructor - you can provide an offset for the toString conversion (ex: counting from 1 instead of 0)
  explicit DiscreteValueParamConverter(IntType iToStringOffset = 0) : fToStringOffset{iToStringOffset} {}

  // Constructor with printf style format where the parameter (%d) will be (value + offset)
  explicit DiscreteValueParamConverter(VstString16 iFormat, IntType iToStringOffset = 0) :
    fToStringOffset{iToStringOffset}, fFormat{std::move(iFormat)} {}

  // Constructor with all values defined
  explicit DiscreteValueParamConverter(ConstructorType iToStringValues) :
    fToStringValues(iToStringValues.cbegin(), iToStringValues.cend()) {}

  inline int32 getStepCount() const override { return StepCount; }

  inline ParamValue normalize(ParamType const &iDiscreteValue) const override
  {
    return convertDiscreteValueToNormalizedValue(StepCount, static_cast<int32>(iDiscreteValue));
  }

  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return static_cast<ParamType>(convertNormalizedValueToDiscreteValue(StepCount, iNormalizedValue));
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
 * This converters maps a list of values of type `T` to discrete values. It can be used with any `T` that is
 * comparable (note that you can optionally provide your own `Compare`). For example, `T` can be an enum, enum class,
 * struct, class, etc...
 *
 * Example:
 * ```
 * enum class ETabs {
 *  kTabAll = 100,
 *  kTabToggleButtonView = 150
 * };
 *
 * // ...
 * VstParam<ETabs> fTab;
 *
 * // ...
 * fTab =
 *   vst<DiscreteTypeParamConverter<ETabs>>(EJambaTestPluginParamID::kTab, STR16("Tab"),
 *                                          {
 *                                            {ETabs::kTabAll,              STR16("All Controls")},
 *                                            {ETabs::kTabToggleButtonView, STR16("ToggleButtonView")}
 *                                          })
 *     .add();
 *
 * ```
 */
template<typename T, class Compare = std::less<T>>
class DiscreteTypeParamConverter : public IParamConverter<T>
{
public:
  /**
   * Maintains the map of possible values of T (defined in constructor) */
  using TMap = std::map<T, std::tuple<VstString16, ParamValue, int32>, Compare>;

  /**
   * Defines the mapping: discrete value [0, stepCount] to T */
  using TList = std::vector<T>;

  /**
   * Defines the type for the constructor argument. Example: `{ { t, STR16("abc") }, ... }` */
  using ConstructorType = std::initializer_list<std::pair<const T, VstString16>> const &;

  using ParamType = T;

  using IParamConverter<T>::toString;

  /**
   * This constructor will be called this way when initializing a vst or jmb parameter:
   */
  DiscreteTypeParamConverter(ConstructorType iInitList)
  {
    auto stepCount = static_cast<int32>(iInitList.size() - 1);

    // by definition, a discrete parameter has a step count > 0
    DCHECK_F(stepCount > 0);

    int32 i = 0;
    for(auto &pair : iInitList)
    {
      auto paramValue = convertDiscreteValueToNormalizedValue(stepCount, i);
      fMap[pair.first] = std::make_tuple(pair.second, paramValue, i);
      fList.emplace_back(pair.first);
      i++;
    }

    // sanity check... if not the same size it means that 2 entries in the list were the same!
    DCHECK_F(fList.size() == fMap.size());
  }

  // getStepCount
  inline int32 getStepCount() const override { return static_cast<int32>(fMap.size() - 1); }

  // normalize
  inline ParamValue normalize(ParamType const &iValue) const override
  {
    auto iter = fMap.find(iValue);
    if(iter != fMap.cend())
      return std::get<1>(iter->second);
    else
    {
      DLOG_F(WARNING, "could not normalize value...");
      return 0;
    }
  }

  // denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    auto index = convertNormalizedValueToDiscreteValue(getStepCount(), iNormalizedValue);
    return fList[index];
  }

  // toString
  void toString(ParamType const &iValue, String128 oString, int32 iPrecision) const override
  {
    auto iter = fMap.find(iValue);
    if(iter != fMap.cend())
    {
      Steinberg::UString wrapper(oString, str16BufferSize (String128));
      wrapper.assign(std::get<0>(iter->second).c_str());
    }
    else
      oString[0] = 0;
  }

private:
  TMap fMap{};
  TList fList{};
};

/**
 * A converter to deal with an enum (assumes that the enum is contiguous, starts at 0 and that MaxValue is the latest
 * value in the enum). Note that you can also use `DiscreteTypeParamConverter<Enum>` instead in case your enum
 * is not contiguous, you want to select only some values of the enum, or you want a simpler way to initialize it.
 */
template<typename Enum, Enum MaxValue>
class EnumParamConverter : public IParamConverter<Enum>
{
public:
  using ParamType = Enum;

  using IntType = std::underlying_type_t<Enum>;

  using IParamConverter<Enum>::toString;

  /**
   * Defines the type for the constructor argument. Example: `{ STR16("abc"), ... }` */
  using ConstructorType = std::array<VstString16, MaxValue + 1> const &;

  // Constructor - you can provide an offset for the toString conversion (ex: counting from 1 instead of 0)
  explicit EnumParamConverter(IntType iToStringOffset = 0) : fConverter{iToStringOffset} {}

  // Constructor with printf style format where the parameter (%d) will be (value + offset)
  explicit EnumParamConverter(VstString16 iFormat, IntType iToStringOffset = 0) : fConverter{std::move(iFormat), iToStringOffset} {}

  // Constructor with all values defined
  explicit EnumParamConverter(ConstructorType iToStringValues) : fConverter{iToStringValues} {}

  // getStepCount
  inline int32 getStepCount() const override { return MaxValue; }

  // normalize
  inline ParamValue normalize(ParamType const &iDiscreteValue) const override
  {
    return fConverter.normalize(static_cast<IntType>(iDiscreteValue));
  }

  // denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return static_cast<Enum>(fConverter.denormalize(iNormalizedValue));
  }

  // toString
  void toString(ParamType const &iValue, String128 oString, int32 iPrecision) const override
  {
    fConverter.toString(static_cast<IntType>(iValue), oString, iPrecision);
  }

private:
  DiscreteValueParamConverter<MaxValue, IntType> fConverter;
};


}
