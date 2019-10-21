/*
 * Copyright (c) 2018-2019 pongasoft
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
#include "GUIRawVstParameter.h"
#include "GUIVstParameter.h"

namespace pongasoft::VST::GUI::Params {

//------------------------------------------------------------------------
// GUIRawVstParameter::Editor::Editor
//------------------------------------------------------------------------
GUIRawVstParameter::Editor::Editor(ParamID iParamID, VstParametersSPtr iVstParameters) :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)}
{
// DLOG_F(INFO, "GUIRawVstParameter::Editor(%d)", fParamID);
  fVstParameters->beginEdit(fParamID);
  fIsEditing = true;
  fInitialParamValue = fVstParameters->getParamNormalized(fParamID);
}

//------------------------------------------------------------------------
// GUIRawVstParameter::Editor::setValue
//------------------------------------------------------------------------
tresult GUIRawVstParameter::Editor::setValue(ParamValue const &iValue)
{
  tresult res = kResultFalse;
  if(fIsEditing)
  {
    res = fVstParameters->setParamNormalized(fParamID, iValue);
    if(res == kResultOk)
      fVstParameters->performEdit(fParamID, fVstParameters->getParamNormalized(fParamID));
  }
  return res;
}

//------------------------------------------------------------------------
// GUIRawVstParameter::Editor::updateValue
//------------------------------------------------------------------------
bool GUIRawVstParameter::Editor::updateValue(ParamValue const &iValue)
{
  auto previousValue = fVstParameters->getParamNormalized(fParamID);
  if(previousValue != iValue)
  {
    if(setValue(iValue) == kResultOk)
      return true;
  }
  return false;
}

//------------------------------------------------------------------------
// GUIRawVstParameter::Editor::commit
//------------------------------------------------------------------------
tresult GUIRawVstParameter::Editor::commit()
{
  if(fIsEditing)
  {
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIRawVstParameter::Editor::rollback
//------------------------------------------------------------------------
tresult GUIRawVstParameter::Editor::rollback()
{
  if(fIsEditing)
  {
    setValue(fInitialParamValue);
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

//------------------------------------------------------------------------
// GUIRawVstParameter::GUIRawVstParameter
//------------------------------------------------------------------------
GUIRawVstParameter::GUIRawVstParameter(ParamID iParamID,
                                       VstParametersSPtr iVstParameters,
                                       std::shared_ptr<RawVstParamDef> iParamDef)  :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)},
  fParamDef{std::move(iParamDef)}
{
  // DLOG_F(INFO, "GUIRawVstParameter::GUIRawVstParameter(%d)", fParamID);
  DCHECK_F(fVstParameters->exists(fParamID), "Missing parameter [%d]", iParamID);
}

/**
 * Simple wrapper to allow to treat any vst parameter as a discrete one: uses the vst sdk definition of
 * what a discrete property is
 *
 * @see convertDiscreteValueToNormalizedValue
 * @see convertNormalizedValueToDiscreteValue
 */
class DiscreteParamConverter : public IParamConverter<int32>
{
public:
  using ParamType = int32;

  explicit DiscreteParamConverter(int32 iStepCount) : fStepCount(iStepCount)
  {
    DCHECK_F(iStepCount > 0);
  }

  int32 getStepCount() const override { return fStepCount; }

  ParamValue normalize(ParamType const &iValue) const override
  {
    return convertDiscreteValueToNormalizedValue(fStepCount, iValue);
  }

  ParamType denormalize(ParamValue iNormalizedValue) const override
  {
    return convertNormalizedValueToDiscreteValue(fStepCount, iNormalizedValue);
  }

protected:
  int32 fStepCount;
};

//------------------------------------------------------------------------
// GUIRawVstParameter::asDiscreteParameter
//------------------------------------------------------------------------
std::shared_ptr<GUIDiscreteParameter> GUIRawVstParameter::asDiscreteParameter(int32 iStepCount)
{
  int32 stepCount = getStepCount();

  if(stepCount > 0)
  {
    if(iStepCount != -1 && iStepCount != stepCount)
    {
      DLOG_F(WARNING, "Parameter step count %d, differs from requested %d. Using parameter step count.", stepCount, iStepCount);
    }
  }
  else
  {
    stepCount = iStepCount;
  }

  if(stepCount > 0)
    return std::make_shared<GUIVstParameter<int32>>(std::dynamic_pointer_cast<GUIRawVstParameter>(shared_from_this()),
                                                    std::make_shared<DiscreteParamConverter>(stepCount));
  else
    return nullptr;
}


}