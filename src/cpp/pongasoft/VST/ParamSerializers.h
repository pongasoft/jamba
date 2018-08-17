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
#pragma once

#include <pongasoft/Utils/Reflection/Reflection.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

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
  // by default does nothing -- subclasses can override
  virtual void writeToStream(ParamType const &iValue, std::ostream &oStream) const {}
  virtual std::string toString(ParamType const &iValue, int32 iPrecision) const
  {
    std::ostringstream s;
    s.precision(iPrecision);
    s.setf(std::ios::fixed);
    writeToStream(iValue, s);
    return s.str();
  }
};


//------------------------------------------------------------------------
// Implementation (not exposed) details
// YP Note: this is quite convoluted but I could not really find a better
// way
//------------------------------------------------------------------------
namespace Impl
{
// represents the api void T::writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
template <typename T>
using writeToStream_t = decltype(T::writeToStream(std::declval<typename T::ParamType const &>(), std::declval<std::ostream &>()));

// this will be "true" if T has the writeToStream API
template <typename T>
using has_writeToStream = Utils::Reflection::detect<T, writeToStream_t>;


//------------------------------------------------------------------------
// Defining OStreamer type: only 1 will be defined based on condition
//------------------------------------------------------------------------
namespace StaticParamSerializer {
// this is the empty implementation when there is no ParamSerializer::writeToStream method or
// ostream << ParamSerializer::ParamType method
template<typename ParamSerializer, class Enable = void>
struct OStreamer
{
  inline static void writeToStream(typename ParamSerializer::ParamType const &iValue, std::ostream &oStream)
  {
  }
};

// this is the one that will delegate to ParamSerializer::writeToStream (note that it takes precedence over ostream)
template<typename ParamSerializer>
struct OStreamer<ParamSerializer, typename std::enable_if_t<has_writeToStream<ParamSerializer>::value>>
{
  inline static void writeToStream(typename ParamSerializer::ParamType const &iValue, std::ostream &oStream)
  {
    ParamSerializer::writeToStream(iValue, oStream);
  }
};

// this is the one that will delegate to ostream << ParamSerializer::ParamType
template<typename ParamSerializer>
struct OStreamer<ParamSerializer, typename std::enable_if_t<Utils::Reflection::has_ostream<typename ParamSerializer::ParamType>::value &&
                                                            !has_writeToStream<ParamSerializer>::value>>
{
  inline static void writeToStream(typename ParamSerializer::ParamType const &iValue, std::ostream &oStream)
  {
    oStream << iValue;
  }
};
}

}

/**
 * Wrapper/convenient class using a class containing static method instead.
 *
 * @tparam ParamConverter the type of the static class
 */
template<typename ParamSerializer>
class StaticParamSerializer : public IParamSerializer<typename ParamSerializer::ParamType>
{
public:
  using ParamType = typename ParamSerializer::ParamType;

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    return ParamSerializer::readFromStream(iStreamer, oValue);
  }

  // writeToStream
  tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    return ParamSerializer::writeToStream(iValue, oStreamer);
  }

  // writeToStream
  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    // delegate to the conditional OStreamer
    Impl::StaticParamSerializer::OStreamer<ParamSerializer>::writeToStream(iValue, oStream);
  }
};

/**
 * Simple function to create a param serializer from a class with static methods
 */
template<typename ParamSerializer>
inline static std::unique_ptr<StaticParamSerializer<ParamSerializer>> createParamSerializer()
{
  return std::make_unique<StaticParamSerializer<ParamSerializer>>();
}

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

// readInt64 - contrary to IBStreamer.readInt64, this method does NOT modify oValue if cannot be read
inline tresult readInt64(IBStreamer &iStreamer, int64 &oValue)
{
  int64 value;
  if(!iStreamer.readInt64(value))
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
class RawParamSerializer
{
public:
  using ParamType = ParamValue;

  inline static tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue)
  {
    return IBStreamHelper::readDouble(iStreamer, oValue);
  }

  inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    oStreamer.writeDouble(iValue);
    return kResultOk;
  }
};

/**
 * A parameter backed by a C type string (char[size]). No memory allocation happens in this case.
 *
 * @tparam size of the string saved/restored
 */
template<int size = 128>
class CStringParamSerializer
{
public:
  using ParamType = char[size];

  // readFromStream
  inline static tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue)
  {
    if(iStreamer.readRaw(static_cast<void*>(oValue), size) == size)
      return kResultOk;
    else
      return kResultFalse;
  }

  // writeToStream
  inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    if(oStreamer.writeRaw(static_cast<void const *>(iValue), size) == size)
      return kResultOk;
    else
      return kResultFalse;
  }
};

}
}
