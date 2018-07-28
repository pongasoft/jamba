#include "RTParameter.h"

#include <pluginterfaces/vst/ivstaudioprocessor.h>

namespace pongasoft {
namespace VST {
namespace RT {

//------------------------------------------------------------------------
// RTParameter::update - update fValue to the new value and return true if it changed
//------------------------------------------------------------------------
tresult RTRawParameter::addToOutput(ProcessData &oData)
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
bool RTRawParameter::updateNormalizedValue(ParamValue iNormalizedValue)
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
bool RTRawParameter::resetPreviousValue()
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