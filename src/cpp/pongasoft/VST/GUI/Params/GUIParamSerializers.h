#pragma once

#include <vstgui4/vstgui/lib/cstring.h>

#include <pongasoft/VST/ParamDef.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

using namespace Steinberg;
using namespace VSTGUI;

/**
 * A parameter backed by a UTF8String
 * @tparam size max size of the string saved/restored
 */
template<int size = 128>
class UTF8StringParamSerializer
{
public:
  using ParamType = UTF8String;


  // readFromStream
  inline static ParamType readFromStream(IBStreamer &iStreamer, ParamType const &iDefaultValue)
  {
    char8 str[size];
    iStreamer.readString8(str, size);
    return UTF8String{str};
  }

  // writeToStream
  inline static tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer)
  {
    oStreamer.writeString8(iValue, true);
    return kResultOk;
  }
};

}
}
}
}