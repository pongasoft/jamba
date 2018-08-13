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
  inline static tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue)
  {
    char8 str[size];
    iStreamer.readString8(str, size);
    oValue = UTF8String{str};
    return kResultOk;
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