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
#include "RTParameter.h"

#include <pluginterfaces/vst/ivstaudioprocessor.h>

namespace pongasoft {
namespace VST {
namespace RT {

//------------------------------------------------------------------------
// RTRawVstParameter::addToOutput
//------------------------------------------------------------------------
tresult RTRawVstParameter::addToOutput(ProcessData &oData)
{
  IParameterChanges* outParamChanges = oData.outputParameterChanges;
  if(outParamChanges != nullptr)
  {
    int32 index = 0;
    auto paramQueue = outParamChanges->addParameterData(getParamID(), index);
    if(paramQueue != nullptr)
    {
      int32 index2 = 0;
      return paramQueue->addPoint(0, getNormalizedValue(), index2);
    }
  }

  return kResultFalse;
}

//------------------------------------------------------------------------
// RTParameter::updateNormalizedValue
//------------------------------------------------------------------------
bool RTRawVstParameter::updateNormalizedValue(ParamValue iNormalizedValue)
{
  iNormalizedValue = Utils::clamp(iNormalizedValue, 0.0, 1.0);

  if(fNormalizedValue != iNormalizedValue)
  {
    fNormalizedValue = iNormalizedValue;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// RTParameter::resetPreviousValue
//------------------------------------------------------------------------
bool RTRawVstParameter::resetPreviousValue()
{
  if(hasChanged())
  {
    fPreviousNormalizedValue = fNormalizedValue;
    return true;
  }
  return false;
}

}
}
}