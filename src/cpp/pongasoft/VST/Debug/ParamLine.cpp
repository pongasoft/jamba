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

#include "ParamLine.h"

namespace pongasoft {
namespace VST {
namespace Debug {

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamLine::toString(ParamDisplay::ParamMap const &iParamMap) const
{
  std::ostringstream s;

  int paramCount = 0;

  for(auto paramID : fParamDisplay.ids())
  {
    if(paramCount > 0)
      s << fParamSeparator;

    auto const &param = iParamMap.at(paramID);

    int idx = 0;

    for(auto key: fParamDisplay.keys())
    {
      auto const &val = param.at(key);

      if(!fSkipEmptyEntry || !val.empty())
      {
        if(idx == 1)
          s << fFirstSeparator;
        else if(idx > 1)
          s << fNextSeparator;

        s << val;

        idx++;
      }
    }

    paramCount++;
  }

  return s.str();
}

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamLine::toString() const
{
  return toString(fParamDisplay.getParamMap());
}

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamLine::toString(NormalizedState const &iNormalizedState) const
{
  return toString(fParamDisplay.getParamMap(iNormalizedState));
}

using Key = ParamDisplay::Key;

//------------------------------------------------------------------------
// ParamLine::from
//------------------------------------------------------------------------
ParamLine ParamLine::from(Parameters const &iParams)
{
  return ParamLine{ParamDisplay::from(iParams)
                     .keys({Key::kID, Key::kNormalizedDefault, Key::kDefault})};
}

//------------------------------------------------------------------------
// ParamLine::from
//------------------------------------------------------------------------
ParamLine ParamLine::from(RT::RTState const *iState, bool iSaveStateOnly)
{
  return ParamLine{ParamDisplay::from(iState, iSaveStateOnly)
                     .keys({Key::kID, Key::kNormalizedValue, Key::kValue})};
}

//------------------------------------------------------------------------
// ParamLine::from
//------------------------------------------------------------------------
ParamLine ParamLine::from(GUI::GUIState const *iState, bool iSaveStateOnly)
{
  return ParamLine{ParamDisplay::from(iState, iSaveStateOnly)
                     .keys({Key::kID, Key::kNormalizedValue, Key::kValue})};
}


}
}
}