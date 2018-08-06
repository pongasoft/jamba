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

#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>
#include <memory>

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
  virtual tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) = 0;
  virtual tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) = 0;
};

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
  virtual tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue)
  {
    return ParamSerializer::readFromStream(iStreamer, oValue);
  }

  virtual tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    return ParamSerializer::writeToStream(iValue, oStreamer);
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
 * This parameter handles serializing a raw parameter (ParamValue)
 */
class RawParamSerializer
{
public:
  using ParamType = ParamValue;

  inline static tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue)
  {
    double value;
    if(!iStreamer.readDouble(value))
      return kResultFalse;
    oValue = value;
    return kResultOk;
  }

  inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    oStreamer.writeDouble(iValue);
    return kResultOk;
  }
};

}
}
