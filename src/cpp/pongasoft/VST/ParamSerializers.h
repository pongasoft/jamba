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
  virtual ParamType readFromStream(IBStreamer &iStreamer, ParamType const &iDefaultValue) = 0;
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
  virtual ParamType readFromStream(IBStreamer &iStreamer, ParamType const &iDefaultValue)
  {
    return ParamSerializer::readFromStream(iStreamer, iDefaultValue);
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

  inline static ParamType readFromStream(IBStreamer &iStreamer, ParamType const &iDefaultValue)
  {
    double value;
    if(!iStreamer.readDouble(value))
      value = iDefaultValue;
    return value;
  }

  inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    oStreamer.writeDouble(iValue);
    return kResultOk;
  }
};

}
}
