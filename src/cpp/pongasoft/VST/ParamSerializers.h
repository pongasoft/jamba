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

#include <pongasoft/logging/logging.h>
#include <pongasoft/Utils/Constants.h>
#include <pongasoft/Utils/Misc.h>
#include <pongasoft/Utils/Metaprogramming.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <map>
#include <vector>

namespace pongasoft::VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * Interface that defines a converter from a type `T` to an `int32` given a number of steps
 * (provided by `getStepCount`). Any Jmb parameter providing this interface can be used as a discrete parameter
 * (for example, in a step button view). Note that this interface does not need to be implemented if `T` can behave
 * like an `int32` (see `StaticCastDiscreteConverter`).
 */
template<typename T>
class IDiscreteConverter
{
public:
  virtual int32 getStepCount() const = 0;
  virtual tresult convertFromDiscreteValue(int32 iDiscreteValue, T &oValue) const = 0;
  virtual tresult convertToDiscreteValue(T const &iValue, int32 &oDiscreteValue) const = 0;
};

/**
 * This implementation simply cast `T` to an `int32` (and vice-versa). For example, it works for any numerical types
 * or enums. And it works for any `struct` or `class` that defines:
 *
 * ```
 *   explicit T(int32 x); // constructor for cast from int32 to T
 *   explicit operator int32() const; // operator int32 for cast from T to int32
 * ```
 */
template<typename T>
class StaticCastDiscreteConverter : public IDiscreteConverter<T>
{
public:
  // Constructor
  explicit StaticCastDiscreteConverter(int32 iStepCount) : fStepCount(iStepCount)
  {
    DCHECK_F(fStepCount > 0);
  }

  // getStepCount
  int32 getStepCount() const override { return fStepCount; }

  // convertFromDiscreteValue
  tresult convertFromDiscreteValue(int32 iDiscreteValue, T &oValue) const override
  {
    iDiscreteValue = Utils::clamp(iDiscreteValue, Utils::ZERO_INT32, fStepCount);
    oValue = static_cast<T>(iDiscreteValue);
    return kResultOk;
  }

  // convertToDiscreteValue
  tresult convertToDiscreteValue(T const &iValue, int32 &oDiscreteValue) const override
  {
    oDiscreteValue = static_cast<int32>(iValue);
    oDiscreteValue = Utils::clamp(oDiscreteValue, Utils::ZERO_INT32, fStepCount);
    return kResultOk;
  }

protected:
  int32 fStepCount;
};


/**
 * A vst parameter is represented by a ParamValue type which is a double in the range [0,1].
 * This interface represents a way to extend the concept of parameter by allowing any type that cannot be converted
 * to ParamValue (for example a string). The parameter simply needs to provide an implementation of this interface
 * which represents a way to serialize/deserialize the type to the stream.
 *
 * @tparam T the actual type that the parameter represents
 */
template<typename T>
class IParamSerializer
{
public:
  using ParamType = T;
  virtual tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const = 0;
  virtual tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const = 0;

  /**
   * By default, this implementation simply writes the value to the stream IF it is
   * possible (determined at compilation time). Doesn't do anything if not.
   * Subclasses can redefine this behavior.
   */
  virtual void writeToStream(ParamType const &iValue, std::ostream &oStream) const
  {
    if constexpr(Utils::is_operator_write_to_ostream_defined<ParamType>)
    {
      oStream << iValue;
    }
  }

  virtual std::string toString(ParamType const &iValue, int32 iPrecision) const
  {
    std::ostringstream s;
    if(iPrecision >= 0)
    {
      s.precision(iPrecision);
      s.setf(std::ios::fixed);
    }
    writeToStream(iValue, s);
    return s.str();
  }
};

/**
 * IBStreamHelper - Helper functions
 */
namespace IBStreamHelper {

// readDouble - contrary to IBStreamer.readDouble, this method does NOT modify oValue if cannot be read
inline tresult readDouble(IBStreamer &iStreamer, double &oValue)
{
  double value;
  if(!iStreamer.readDouble(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

// readFloat - contrary to IBStreamer.readFloat, this method does NOT modify oValue if cannot be read
inline tresult readFloat(IBStreamer &iStreamer, float &oValue)
{
  float value;
  if(!iStreamer.readFloat(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

// readFloatArray - contrary to IBStreamer.readFloatArray, this method returns tresult and can use any Int type
template<typename Int>
inline tresult readFloatArray(IBStreamer &iStreamer, float *oValue, Int iCount)
{
  for(Int i = 0; i < iCount; i++)
  {
    if(!iStreamer.readFloat(oValue[i]))
      return kResultFalse;
  }
  return kResultOk;
}


// readInt64 - contrary to IBStreamer.readInt64, this method does NOT modify oValue if cannot be read
inline tresult readInt64(IBStreamer &iStreamer, int64 &oValue)
{
  int64 value;
  if(!iStreamer.readInt64(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

// readInt64 - contrary to IBStreamer.readInt64, this method does NOT modify oValue if cannot be read
inline tresult readInt64u(IBStreamer &iStreamer, uint64 &oValue)
{
  uint64 value;
  if(!iStreamer.readInt64u(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

// readInt32 - contrary to IBStreamer.readInt32, this method does NOT modify oValue if cannot be read
inline tresult readInt32(IBStreamer &iStreamer, int32 &oValue)
{
  int32 value;
  if(!iStreamer.readInt32(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

// readBool - contrary to IBStreamer.readBool, this method does NOT modify oValue if cannot be read
inline tresult readBool(IBStreamer &iStreamer, bool &oValue)
{
  bool value;
  if(!iStreamer.readBool(value))
    return kResultFalse;
  oValue = value;
  return kResultOk;
}

}

/**
 * This parameter handles serializing a raw parameter (ParamValue)
 */
class RawParamSerializer : public IParamSerializer<ParamValue>
{
public:
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return IBStreamHelper::readDouble(iStreamer, oValue);
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeDouble(iValue);
    return kResultOk;
  }
};

/**
 * This parameter handles serializing a double parameter
 */
class DoubleParamSerializer : public IParamSerializer<double>
{
public:
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return IBStreamHelper::readDouble(iStreamer, oValue);
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeDouble(iValue);
    return kResultOk;
  }
};

/**
 * This parameter handles serializing a int32 parameter
 */
class Int32ParamSerializer : public IParamSerializer<int32>
{
public:
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return IBStreamHelper::readInt32(iStreamer, oValue);
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeInt32(iValue);
    return kResultOk;
  }
};

/**
 * This parameter handles serializing a int64 parameter
 */
class Int64ParamSerializer : public IParamSerializer<int64>
{
public:
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return IBStreamHelper::readInt64(iStreamer, oValue);
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeInt64(iValue);
    return kResultOk;
  }
};

/**
 * This parameter handles serializing a bool parameter
 */
class BooleanParamSerializer : public IParamSerializer<bool>, public IDiscreteConverter<bool>
{
public:
  explicit BooleanParamSerializer(std::string iFalseString = "Off",
                                  std::string iTrueString = "On") :
    fFalseString{std::move(iFalseString)},
    fTrueString{std::move(iTrueString)}
  {}

  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return IBStreamHelper::readBool(iStreamer, oValue);
  }

  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeBool(iValue);
    return kResultOk;
  }

  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    oStream << (iValue ? fTrueString : fFalseString);
  }

private:
  inline int32 getStepCount() const override { return 1; }

  tresult convertFromDiscreteValue(int32 iDiscreteValue, bool &oValue) const override
  {
    oValue = iDiscreteValue != 0;
    return kResultOk;
  }

  tresult convertToDiscreteValue(const bool &iValue, int32 &oDiscreteValue) const override
  {
    oDiscreteValue = iValue ? 1 : 0;
    return kResultOk;
  }

protected:
  std::string fFalseString;
  std::string fTrueString;
};


/**
 * A parameter backed by a C type string (char[size]). No memory allocation happens in this case.
 *
 * @tparam size of the string saved/restored
 */
template<int size = 128>
class CStringParamSerializer : public IParamSerializer<char[size]>
{
public:
  using ParamType = char[size];

  // readFromStream
  inline tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    if(iStreamer.readRaw(static_cast<void*>(oValue), size) == size)
    {
      oValue[size - 1] = 0; // making sure it is null terminated
      return kResultOk;
    }
    else
      return kResultFalse;
  }

  // writeToStream - IBStreamer
  inline tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    if(oStreamer.writeRaw(static_cast<void const *>(iValue), size) == size)
      return kResultOk;
    else
      return kResultFalse;
  }

  // writeToStream - std::ostream
  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    if(std::find(std::begin(iValue), std::end(iValue), 0) != std::end(iValue))
    {
      // this means that the string is null terminated... we are good
      oStream << iValue;
    }
    else
    {
      char8 str[size];
      std::copy(std::begin(iValue), std::end(iValue), std::begin(str));
      str[size - 1] = 0; // make the copy null terminated
      oStream << str;
      DLOG_F(WARNING, "%s not properly null terminated!", str);
    }
  }
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
 * JmbParam<ETabs> fTab;
 *
 * // ...
 * fTab =
 *   jmb<ETabs>(EJambaTestPluginParamID::kTab, STR16("Tab"))
 *     .serializer<DiscreteTypeParamSerializer<ETabs>>(
 *                                          {
 *                                            {ETabs::kTabAll,              "All Controls"},
 *                                            {ETabs::kTabToggleButtonView, "ToggleButtonView"}
 *                                          })
 *     .add();
 *
 * ```
 */
template<typename T, class Compare = std::less<T>>
class DiscreteTypeParamSerializer : public IParamSerializer<T>, public IDiscreteConverter<T>
{
public:
  /**
   * Maintains the map of possible values of T (defined in constructor) */
  using TMap = std::map<T, std::pair<std::string, int32>, Compare>;

  /**
   * Defines the mapping: discrete value [0, stepCount] to T */
  using TList = std::vector<T>;

  /**
   * Defines the type for the constructor argument : `{ { t, "abc" }, ... }` */
  using ConstructorType = std::initializer_list<std::pair<const T, std::string>> const &;

  using ParamType = T;

  // DiscreteTypeParamSerializer
  DiscreteTypeParamSerializer(ConstructorType iInitList)
  {
    int32 stepCount = iInitList.size() - 1;

    // by definition, a discrete parameter has a step count > 0
    DCHECK_F(stepCount > 0);

    int32 i = 0;
    for(auto &pair : iInitList)
    {
      fMap[pair.first] = std::make_pair(pair.second, i);
      fList.emplace_back(pair.first);
      i++;
    }

    // sanity check... if not the same size it means that 2 entries in the list were the same!
    DCHECK_F(fList.size() == fMap.size());
  }

  // getStepCount
  inline int32 getStepCount() const override { return fMap.size() - 1; }

  // convertFromDiscreteValue
  tresult convertFromDiscreteValue(int32 iDiscreteValue, ParamType &oValue) const override
  {
    if(iDiscreteValue < 0 || iDiscreteValue > getStepCount())
      return kResultFalse;
    oValue = fList[iDiscreteValue];
    return kResultOk;
  }

  // convertToDiscreteValue
  tresult convertToDiscreteValue(ParamType const &iValue, int32 &oDiscreteValue) const override
  {
    auto iter = fMap.find(iValue);
    if(iter != fMap.cend())
    {
      oDiscreteValue = std::get<1>(iter->second);
      return kResultOk;
    }
    else
    {
      DLOG_F(WARNING, "could not convertToDiscreteValue...");
      return kResultFalse;
    }
  }

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    int32 discreteValue;
    if(IBStreamHelper::readInt32(iStreamer, discreteValue) == kResultOk)
    {
      return convertFromDiscreteValue(discreteValue, oValue);
    }
    return kResultFalse;
  }

  // writeToStream
  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    int32 discreteValue;
    if(convertToDiscreteValue(iValue, discreteValue) == kResultOk)
    {
      if(oStreamer.writeInt32(discreteValue))
        return kResultOk;
    }

    return kResultFalse;
  }

  // writeToStream
  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    auto iter = fMap.find(iValue);
    if(iter != fMap.cend())
    {
      oStream << std::get<0>(iter->second);
    }
  }

private:
  TMap fMap{};
  TList fList{};
};

}
