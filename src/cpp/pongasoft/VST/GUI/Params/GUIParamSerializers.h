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
class UTF8StringParamSerializer : public IParamSerializer<UTF8String>
{
public:
  // readFromStream
  inline tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override
  {
    char8 str[size];
    iStreamer.readString8(str, size);
    str[size - 1] = 0; // making sure it is null terminated....
    oValue = UTF8String{str};
    return kResultOk;
  }

  // writeToStream
  inline tresult writeToStream(const ParamType &iValue, IBStreamer &oStreamer) const override
  {
    oStreamer.writeString8(iValue, true);
    return kResultOk;
  }

  // writeToStream
  void writeToStream(ParamType const &iValue, std::ostream &oStream) const override
  {
    oStream << iValue;
  }
};

}
}
}
}