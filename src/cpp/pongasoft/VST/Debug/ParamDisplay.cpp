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

#include "ParamDisplay.h"

#include <base/source/fstring.h>
#include <pongasoft/VST/Parameters.h>

#include <sstream>
#include <iomanip>
#include <map>

namespace pongasoft::VST::Debug {


//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
ParamDisplay::Value ParamDisplay::getValue(ParamID iParamID, Key iKey) const
{
  if(fParameters)
    return getValue(fParameters, iParamID, iKey);

  if(fRTState)
    return getValue(fRTState, iParamID, iKey);

  if(fGUIState)
    return getValue(fGUIState, iParamID, iKey);

  return Value{};
}

//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
std::shared_ptr<RawVstParamDef> ParamDisplay::getRawVstParamDef(ParamID iParamID) const
{
  if(fParameters)
    return fParameters->getRawVstParamDef(iParamID);

  if(fRTState)
    return fRTState->fPluginParameters.getRawVstParamDef(iParamID);

  if(fGUIState)
    return fGUIState->fPluginParameters.getRawVstParamDef(iParamID);

  return nullptr;
}

//------------------------------------------------------------------------
// ParamDisplay::getValues
//------------------------------------------------------------------------
ParamDisplay::ValueMap ParamDisplay::getValues(ParamID iParamID, std::vector<Key> const &iKeys) const
{
  ValueMap map{};

  std::vector<Key> const &keys = iKeys.empty() ? fKeys : iKeys;

  for(auto key : keys)
  {
    map[key] = getValue(iParamID, key);
  }

  return map;
}

//------------------------------------------------------------------------
// ParamDisplay::getParamMap
//------------------------------------------------------------------------
ParamDisplay::ParamMap ParamDisplay::getParamMap(const std::vector<ParamID> &iParamIDs) const
{
  std::vector<ParamID> const &paramIDs = iParamIDs.empty() ? fParamIDs : iParamIDs;

  ParamMap map{};

  for(auto paramID : paramIDs)
    map[paramID] = getValues(paramID);

  return map;
}

//------------------------------------------------------------------------
// ParamDisplay::getParamMap
//------------------------------------------------------------------------
ParamDisplay::ParamMap ParamDisplay::getParamMap(NormalizedState const &iNormalizedState) const
{
  ParamMap map{};

  for(int i = 0; i < iNormalizedState.getCount(); i++)
  {
    auto paramID = iNormalizedState.fSaveOrder->fOrder[i];
    auto value = iNormalizedState.fValues[i];
    auto valueMap = getValues(paramID);

    for(auto key : keys())
    {
      switch(key)
      {
        case Key::kNormalizedValue:
          valueMap[key] = getValue(value);
          break;

        case Key::kValue:
        {
          auto paramDef = getRawVstParamDef(paramID);
          if(paramDef)
          {
            String128 s;
            paramDef->toString(value, s);
            valueMap[key] = String(s).text8();
          }
          break;
        }

        default:
          break;
      }
    }

    map[paramID] = valueMap;
  }

  return map;
}

//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
ParamDisplay::Value ParamDisplay::getValue(Parameters const *iParams, ParamID iParamID, Key iKey) const
{
  DCHECK_F(iParams != nullptr);

  auto vst = iParams->getRawVstParamDef(iParamID);

  if(vst)
    return getValue(vst.get(), iKey);

  auto jmb = iParams->getJmbParamDef(iParamID);
  if(jmb)
    return getValue(jmb.get(), iKey);

  return Value{};
}

//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
ParamDisplay::Value ParamDisplay::getValue(RT::RTState const *iState, ParamID iParamID, Key iKey) const
{
  DCHECK_F(iState != nullptr);

  // vst - RawVstParamDef
  auto vst = iState->fVstParameters.find(iParamID);
  if(vst != iState->fVstParameters.cend())
    return getValue(vst->second, iKey);

  // jmbIn - IRTJmbInParameter
  auto jmbIn = iState->fInboundMessagingParameters.find(iParamID);
  if(jmbIn != iState->fInboundMessagingParameters.cend())
    return getValue(jmbIn->second, iKey);

  // IRTJmbOutParameter
  auto jmbOut = iState->fOutboundMessagingParameters.find(iParamID);
  if(jmbOut != iState->fOutboundMessagingParameters.cend())
    return getValue(jmbOut->second, iKey);

  return Value{};
}

//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
ParamDisplay::Value ParamDisplay::getValue(GUI::GUIState const *iState, ParamID iParamID, Key iKey) const
{
  DCHECK_F(iState != nullptr);

  // vst - GUIRawVstParameter
  auto vst = iState->getRawVstParameter(iParamID);
  if(vst)
    return getValue(vst, iKey, &iState->fPluginParameters);

  // jmb - IGUIJmbParameter
  auto jmb = iState->getJmbParameter(iParamID);
  if(jmb)
    return getValue(*jmb, iKey);

  return Value{};
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - RawVstParamDef
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(RawVstParamDef const *iParamDef, Key iKey) const
{
  switch(iKey)
  {
    case Key::kID:
      return std::to_string(iParamDef->fParamID);
    case Key::kType:
      return "vst";
    case Key::kTitle:
      return String(iParamDef->fTitle.c_str()).text8();
    case Key::kOwner:
      return iParamDef->fOwner == IParamDef::Owner::kGUI ? "ui" : "rt";
    case Key::kTransient:
      return iParamDef->fTransient ? "x" : "";
    case Key::kNormalizedDefault:
      return getValue(iParamDef->fDefaultValue);
    case Key::kDefault:
    {
      String128 s;
      iParamDef->toString(iParamDef->fDefaultValue, s);
      return String(s).text8();
    }
    case Key::kSteps:
      return std::to_string(iParamDef->fStepCount);
    case Key::kFlags:
      return std::to_string(iParamDef->fFlags);
    case Key::kShortTitle:
      return String(iParamDef->fShortTitle.c_str()).text8();
    case Key::kPrecision:
      return std::to_string(iParamDef->fPrecision);
    case Key::kUnitID:
      return std::to_string(iParamDef->fUnitID);
    case Key::kUnits:
      return String(iParamDef->fUnits.c_str()).text8();
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - RTRawVstParameter
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(std::unique_ptr<RT::RTRawVstParameter> const &iParam, Key iKey) const
{
  switch(iKey)
  {
    case Key::kNormalizedValue:
      return getValue(iParam->getNormalizedValue());

    case Key::kValue:
    {
      String128 s;
      iParam->getParamDef()->toString(iParam->getNormalizedValue(), s);
      return String(s).text8();
    }

    default:
      return getValue(iParam->getParamDef(), iKey);
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - IRTJmbInParameter
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(std::unique_ptr<RT::IRTJmbInParameter> const &iParam, Key iKey) const
{
  switch(iKey)
  {
    case Key::kValue:
    {
      std::ostringstream s;
      iParam->writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam->getParamDef(), iKey);
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - IRTJmbOutParameter
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(std::unique_ptr<RT::IRTJmbOutParameter> const &iParam, Key iKey) const
{
  switch(iKey)
  {
    case Key::kValue:
    {
      std::ostringstream s;
      iParam->writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam->getParamDef(), iKey);
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - IJmbParamDef
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(IJmbParamDef const *iParamDef, Key iKey) const
{
  switch(iKey)
  {
    case Key::kID:
      return std::to_string(iParamDef->fParamID);
    case Key::kType:
      return "jmb";
    case Key::kTitle:
      return String(iParamDef->fTitle.c_str()).text8();
    case Key::kOwner:
      return iParamDef->fOwner == IParamDef::Owner::kGUI ? "ui" : "rt";
    case Key::kTransient:
      return iParamDef->fTransient ? "x" : "";
    case Key::kDefault:
    {
      std::ostringstream s;
      iParamDef->writeDefaultValue(s);
      return s.str();
    }
    case Key::kShared:
      return iParamDef->fShared ? "x" : "";
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - GUIRawVstParameter
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(std::unique_ptr<GUI::GUIRawVstParameter> const &iParam,
                                   Key iKey,
                                   Parameters const *iParameters) const
{
  switch(iKey)
  {
    case Key::kNormalizedValue:
      return getValue(iParam->getValue());

    case Key::kValue:
    {
      String128 s;
      iParameters->getRawVstParamDef(iParam->getParamID())->toString(iParam->getValue(), s);
      return String(s).text8();
    }

    default:
      return getValue(iParameters->getRawVstParamDef(iParam->getParamID()).get(), iKey);
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue - IGUIJmbParameter
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(GUI::IGUIJmbParameter const &iParam, Key iKey) const
{
  switch(iKey)
  {
    case Key::kValue:
    {
      std::ostringstream s;
      iParam.writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam.getParamDef(), iKey);
  }
}


//------------------------------------------------------------------------
// ParamDisplay::getValue
//------------------------------------------------------------------------
std::string ParamDisplay::getValue(Key iKey) const
{
  switch(iKey)
  {
    case Key::kID:
      return "ID";
    case Key::kTitle:
      return "TITLE";
    case Key::kType:
      return "TYP";
    case Key::kOwner:
      return "OW";
    case Key::kTransient:
      return "TRS";
    case Key::kNormalizedDefault:
      return "DEF.N";
    case Key::kDefault:
      return "DEF.S";
    case Key::kNormalizedValue:
      return "VAL.N";
    case Key::kValue:
      return "VAL.S";
    case Key::kSteps:
      return "STP";
    case Key::kFlags:
      return "FLG";
    case Key::kShortTitle:
      return "SHORT";
    case Key::kPrecision:
      return "PRE";
    case Key::kUnitID:
      return "UID";
    case Key::kUnits:
      return "UNS";
    case Key::kShared:
      return "SHA";
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamDisplay::getValue(ParamValue)
//------------------------------------------------------------------------
ParamDisplay::Value ParamDisplay::getValue(ParamValue iValue) const
{
  std::ostringstream s;

  s.precision(fPrecision);
  s.setf(std::ios::fixed);
  s << iValue;

  return s.str();
}

//------------------------------------------------------------------------
// ParamDisplay::from
//------------------------------------------------------------------------
ParamDisplay ParamDisplay::from(Parameters const &iParams)
{
  return ParamDisplay{iParams}
    .ids(iParams.getAllRegistrationOrder())
    ;
}

//------------------------------------------------------------------------
// ParamDisplay::from
//------------------------------------------------------------------------
ParamDisplay ParamDisplay::from(RT::RTState const *iState, bool iSaveStateOnly)
{
  return ParamDisplay{iState}
    .ids(iSaveStateOnly ? iState->fPluginParameters.getRTSaveStateOrder().fOrder : iState->getAllRegistrationOrder())
    ;
}

//------------------------------------------------------------------------
// ParamDisplay::from
//------------------------------------------------------------------------
ParamDisplay ParamDisplay::from(GUI::GUIState const *iState, bool iSaveStateOnly)
{
  std::vector<ParamID> ids;
  if(iSaveStateOnly)
  {
    ids = iState->fPluginParameters.getGUISaveStateOrder().fOrder;
  }
  else
  {
    // ALL vst parameters are accessible from gui state (no registration required)
    ids = iState->fPluginParameters.getVstRegistrationOrder();
    ids.insert(ids.end(), iState->getAllRegistrationOrder().cbegin(), iState->getAllRegistrationOrder().cend());
  }

  return ParamDisplay{iState}
    .ids(ids)
    ;
}

}