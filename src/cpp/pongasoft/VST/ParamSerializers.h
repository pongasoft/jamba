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
#pragma once

#include <pongasoft/logging/logging.h>
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

  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    oStream << iValue;
  }
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

}
}
