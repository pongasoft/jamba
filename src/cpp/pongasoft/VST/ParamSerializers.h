#pragma once

#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * Conceptually a ParamSerializer needs to be defined like this
 *
 * template<typename T>
 * class ParamSerializer
 * {
 *   public:
 *     using ParamType = T;
 *     inline static ParamType readFromStream(IBStreamer &iStreamer, ParamType const &iDefaultValue);
 *     inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer);
 * };
 */

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
