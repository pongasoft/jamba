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