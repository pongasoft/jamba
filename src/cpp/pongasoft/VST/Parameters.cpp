/*
 * Copyright (c) 2018-2023 pongasoft
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
#include <base/source/fstring.h>
#include "Parameters.h"

namespace pongasoft::VST {

/**
 * Internal class which extends the Vst::Parameter to override toString and delegate to the param def
 */
class VstParameterImpl : public Vst::Parameter
{
public:
  explicit VstParameterImpl(std::shared_ptr<RawVstParamDef> const &iParamDef) :
    Vst::Parameter(iParamDef->fTitle.c_str(),
                   iParamDef->fParamID,
                   iParamDef->fUnits.empty() ? nullptr : iParamDef->fUnits.c_str(),
                   iParamDef->fDefaultValue,
                   iParamDef->fStepCount,
                   iParamDef->fFlags,
                   iParamDef->fUnitID,
                   iParamDef->fShortTitle.empty() ? nullptr : iParamDef->fShortTitle.c_str()),
    fParamDef{iParamDef}
  {
    setPrecision(fParamDef->fPrecision);
  }

  void toString(ParamValue iNormalizedValue, String128 iString) const override
  {
    fParamDef->toString(iNormalizedValue, iString);
  }

private:
  std::shared_ptr<RawVstParamDef> fParamDef;
};

//------------------------------------------------------------------------
// Parameters::registerVstParameters
//------------------------------------------------------------------------
void Parameters::registerVstParameters(Vst::ParameterContainer &iParameterContainer) const
{
  for(auto paramID : fVstRegistrationOrder)
  {
    // YP Note: ParameterContainer is expecting a pointer and then assumes ownership
    iParameterContainer.addParameter(new VstParameterImpl(fVstParams.at(paramID)));
  }
}

//------------------------------------------------------------------------
// Parameters::newRTState
//------------------------------------------------------------------------
std::unique_ptr<NormalizedState> Parameters::newRTState() const
{
  return std::make_unique<NormalizedState>(&fRTSaveStateOrder);
}

//------------------------------------------------------------------------
// Parameters::newRTState
//------------------------------------------------------------------------
std::unique_ptr<NormalizedState> Parameters::newRTState(NormalizedState::SaveOrder const *iSaveOrder) const
{
  return std::make_unique<NormalizedState>(iSaveOrder);
}

//------------------------------------------------------------------------
// __readStateVersion
//------------------------------------------------------------------------
uint16 __readStateVersion(IBStreamer &iStreamer)
{
  uint16 stateVersion;
  if(!iStreamer.readInt16u(stateVersion))
    stateVersion = 0;
  return stateVersion;
}

//------------------------------------------------------------------------
// Parameters::readRTState
//------------------------------------------------------------------------
tresult Parameters::readRTState(IBStreamer &iStreamer, NormalizedState *oNormalizedState) const
{
  // ignoring version if negative
  if(fRTSaveStateOrder.fVersion >= 0)
  {
    uint16 stateVersion = __readStateVersion(iStreamer);

    // handling deprecated versions
    if(stateVersion != fRTSaveStateOrder.fVersion)
      return readDeprecatedRTState(stateVersion, iStreamer, oNormalizedState);
  }

  return oNormalizedState->readFromStream(this, iStreamer);
}

//------------------------------------------------------------------------
// Parameters::readDeprecatedRTState
//------------------------------------------------------------------------
tresult Parameters::readDeprecatedRTState(uint16 iVersion, IBStreamer &iStreamer, NormalizedState *oNormalizedState) const
{
  auto iter = fRTDeprecatedSaveStateOrders.find(iVersion);

  if(iter != fRTDeprecatedSaveStateOrders.end())
  {
    auto const &deprecatedSaveOrder = iter->second;

    // Implementation note: this code dynamically allocates memory but it is ok because
    // 1) it happens only on load of the plugin
    // 2) it happens in the UI thread
    auto deprecatedNormalizedState = newRTState(&deprecatedSaveOrder);

    // now we can read the deprecated state from the deprecated stream
    auto res = deprecatedNormalizedState->readFromStream(this, iStreamer);

    if(res == kResultOk)
    {
      // we copy all values that we can from deprecated state to new state
      deprecatedNormalizedState->copyValuesTo(*oNormalizedState);

      // we let the plugin potentially handle further changes necessary
      res = handleRTStateUpgrade(*deprecatedNormalizedState, *oNormalizedState);
    }

    return res;
  }
  else
  {
    DLOG_F(WARNING, "unhandled RT state version %d", iVersion);
    return oNormalizedState->readFromStream(this, iStreamer);
  }
}

//------------------------------------------------------------------------
// Parameters::writeRTState
//------------------------------------------------------------------------
tresult Parameters::writeRTState(NormalizedState const *iNormalizedState, IBStreamer &oStreamer) const
{
  DCHECK_F(iNormalizedState->fSaveOrder == &fRTSaveStateOrder);
  return iNormalizedState->writeToStream(this, oStreamer);
}

//------------------------------------------------------------------------
// Parameters::readNormalizedValue
//------------------------------------------------------------------------
ParamValue Parameters::readNormalizedValue(ParamID iParamID, IBStreamer &iStreamer) const
{
  auto iter = fVstParams.find(iParamID);
  if(iter == fVstParams.cend())
  {
    DLOG_F(WARNING, "Could not find parameter [%d]", iParamID);
    return 0;
  }

  return iter->second->readFromStream(iStreamer);
}

//------------------------------------------------------------------------
// Parameters::getParamDef
//------------------------------------------------------------------------
std::shared_ptr<RawVstParamDef> Parameters::getRawVstParamDef(ParamID iParamID) const
{
  auto iter = fVstParams.find(iParamID);
  if(iter == fVstParams.cend())
    return nullptr;

  return iter->second;
}

//------------------------------------------------------------------------
// Parameters::getJmbParamDef
//------------------------------------------------------------------------
std::shared_ptr<IJmbParamDef> Parameters::getJmbParamDef(ParamID iParamID) const
{
  auto iter = fJmbParams.find(iParamID);

  if(iter == fJmbParams.cend())
    return nullptr;

  return iter->second;
}

//------------------------------------------------------------------------
// Parameters::addVstParamDef
//------------------------------------------------------------------------
tresult Parameters::addVstParamDef(std::shared_ptr<RawVstParamDef> iParamDef)
{
  ParamID paramID = iParamDef->fParamID;

  if(fVstParams.find(paramID) != fVstParams.cend())
  {
    DLOG_F(ERROR, "Parameter [%d] already registered", paramID);
    return kInvalidArgument;
  }

  if(fJmbParams.find(paramID) != fJmbParams.cend())
  {
    DLOG_F(ERROR, "Parameter [%d] already registered", paramID);
    return kInvalidArgument;
  }

  if(!iParamDef->isDeprecated())
  {
    fVstRegistrationOrder.emplace_back(paramID);
    fAllRegistrationOrder.emplace_back(paramID);

    if(!iParamDef->fTransient)
    {
      if(iParamDef->fOwner == IParamDef::Owner::kGUI)
        fGUISaveStateOrder.fOrder.emplace_back(paramID);
      else
        fRTSaveStateOrder.fOrder.emplace_back(paramID);
    }
  }

  fVstParams[paramID] = std::move(iParamDef);

  return kResultOk;
}

//------------------------------------------------------------------------
// Parameters::addJmbParamDef
//------------------------------------------------------------------------
tresult Parameters::addJmbParamDef(std::shared_ptr<IJmbParamDef> iParamDef)
{
  ParamID paramID = iParamDef->fParamID;
  
  if(fVstParams.find(paramID) != fVstParams.cend())
  {
    DLOG_F(ERROR, "Parameter [%d] already registered", paramID);
    return kInvalidArgument;
  }

  if(fJmbParams.find(paramID) != fJmbParams.cend())
  {
    DLOG_F(ERROR, "Parameter [%d] already registered", paramID);
    return kInvalidArgument;
  }

  if(!iParamDef->isDeprecated())
  {
    fAllRegistrationOrder.emplace_back(paramID);

    if(!iParamDef->fTransient && iParamDef->fOwner == IParamDef::Owner::kGUI)
      fGUISaveStateOrder.fOrder.emplace_back(paramID);
  }

  fJmbParams[paramID] = std::move(iParamDef);

  return kResultOk;
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
tresult Parameters::setRTSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder)
{
  tresult res = kResultOk;

  auto ids = iSaveOrder.fOrder;
  std::vector<ParamID> newIds{};

  for(auto id : ids)
  {
    tresult paramOk = kResultOk;

    auto iter = fVstParams.find(id);
    if(iter == fVstParams.cend())
    {
      paramOk = kResultFalse;
      DLOG_F(ERROR,
             "Param [%d] was not registered as a vst parameter (did you register as a jmb parameter?)",
             id);
    }
    else
    {
      auto param = iter->second;

      if(param->fTransient)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for RTSaveStateOrder as it is defined transient",
               id);
      }

      if(param->isDeprecated())
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for RTSaveStateOrder as it is deprecated (should be used in RTDeprecatedSaveStateOrder instead)",
               id);
      }

      if(param->fOwner == IParamDef::Owner::kGUI)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for RTSaveStateOrder as it is owned by the GUI",
               id);

      }
    }

    if(paramOk == kResultOk)
    {
      newIds.emplace_back(id);
    }

    res |= paramOk;
  }

  for(auto const &p : fVstParams)
  {
    auto param = p.second;
    if(param->fOwner == IParamDef::Owner::kRT && !param->fTransient && !param->isDeprecated())
    {
      if(std::find(newIds.cbegin(), newIds.cend(), p.first) == newIds.cend())
      {
        DLOG_F(WARNING, "Param [%d] is not marked transient or deprecated. Either mark the parameter transient or deprecated or add it to RTSaveStateOrder", p.first);
      }
    }
  }

  fRTSaveStateOrder = {iSaveOrder.fVersion, newIds};

  return res;
}

//------------------------------------------------------------------------
// Parameters::setRTDeprecatedSaveStateOrder
//------------------------------------------------------------------------
tresult Parameters::setRTDeprecatedSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder)
{
  tresult res = kResultOk;

  auto ids = iSaveOrder.fOrder;
  std::vector<ParamID> newIds{};

  bool deprecatedParam = false;

  for(auto id : ids)
  {
    tresult paramOk = kResultOk;

    auto iter = fVstParams.find(id);
    if(iter == fVstParams.cend())
    {
      paramOk = kResultFalse;
      DLOG_F(ERROR,
             "Param [%d] was not registered as a vst parameter (did you register as a jmb parameter?)",
             id);
    }
    else
    {
      auto param = iter->second;

      if(param->fOwner == IParamDef::Owner::kGUI)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for RTDeprecatedSaveStateOrder as it is owned by the GUI",
               id);
      }

      if(param->isDeprecated())
      {
        deprecatedParam = true;
        if(param->fDeprecatedSince != iSaveOrder.fVersion)
        {
          paramOk = kResultFalse;
          DLOG_F(ERROR,
                 "Param [%d] deprecated version mismatch (marked deprecated since [%d] but used in [%d] RTDeprecatedSaveStateOrder)",
                 id, param->fDeprecatedSince, iSaveOrder.fVersion);
        }
      }
    }

    if(paramOk == kResultOk)
    {
      newIds.emplace_back(id);
    }

    res |= paramOk;
  }

  if(!deprecatedParam)
  {
    DLOG_F(WARNING, "RTDeprecatedSaveStateOrder for version %d does not contain any deprecated parameter.",  iSaveOrder.fVersion);
  }

  fRTDeprecatedSaveStateOrders[iSaveOrder.fVersion] = {iSaveOrder.fVersion, newIds};

  return res;
}

//------------------------------------------------------------------------
// Parameters::setGUISaveStateOrder
//------------------------------------------------------------------------
tresult Parameters::setGUISaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder)
{
  tresult res = kResultOk;

  auto ids = iSaveOrder.fOrder;
  std::vector<ParamID> newIds{};

  std::map<ParamID, std::shared_ptr<IParamDef>> allParams{};
  for(auto &&p : fVstParams)
  {
    allParams[p.first] = p.second;
  }

  for(auto &&p : fJmbParams)
  {
    allParams[p.first] = p.second;
  }

  for(auto id : ids)
  {
    tresult paramOk = kResultOk;

    auto iter = allParams.find(id);
    if(iter == allParams.cend())
    {
      paramOk = kResultFalse;
      DLOG_F(ERROR,
             "Param [%d] was not registered as a vst or jmb parameter",
             id);
    }
    else
    {
      auto param = iter->second;
      if(param->fTransient)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for GUISaveStateOrder as it is defined transient",
               id);
      }

      if(param->fOwner == IParamDef::Owner::kRT)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for GUISaveStateOrder as it is owned by RT",
               id);

      }
    }

    if(paramOk == kResultOk)
    {
      newIds.emplace_back(id);
    }

    res |= paramOk;

  }

  for(auto &&p : allParams)
  {
    auto param = p.second;
    if(param->fOwner == IParamDef::Owner::kGUI && !param->fTransient)
    {
      if(std::find(newIds.cbegin(), newIds.cend(), p.first) == newIds.cend())
      {
        DLOG_F(WARNING, "Param [%d] is not marked transient. Either mark the parameter transient or add it to GUISaveStateOrder", p.first);
      }
    }
  }

  fGUISaveStateOrder = {iSaveOrder.fVersion, newIds};

  return res;
}

//------------------------------------------------------------------------
// Parameters::setGUIDeprecatedSaveStateOrder
//------------------------------------------------------------------------
tresult Parameters::setGUIDeprecatedSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder)
{
  tresult res = kResultOk;

  auto ids = iSaveOrder.fOrder;
  std::vector<ParamID> newIds{};

  bool deprecatedParam = false;

  std::map<ParamID, std::shared_ptr<IParamDef>> allParams{};
  for(auto &&p : fVstParams)
  {
    allParams[p.first] = p.second;
  }

  for(auto &&p : fJmbParams)
  {
    allParams[p.first] = p.second;
  }

  for(auto id : ids)
  {
    tresult paramOk = kResultOk;

    auto iter = allParams.find(id);
    if(iter == allParams.cend())
    {
      paramOk = kResultFalse;
      DLOG_F(ERROR,
             "Param [%d] was not registered as a vst or jmb parameter",
             id);
    }
    else
    {
      auto param = iter->second;

      if(param->fOwner == IParamDef::Owner::kRT)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for GUISaveStateOrder as it is owned by RT",
               id);

      }

      if(param->isDeprecated())
      {
        deprecatedParam = true;
        if(param->fDeprecatedSince != iSaveOrder.fVersion)
        {
          paramOk = kResultFalse;
          DLOG_F(ERROR,
                 "Param [%d] deprecated version mismatch (marked deprecated since [%d] but used in [%d] GUIDeprecatedSaveStateOrder)",
                 id, param->fDeprecatedSince, iSaveOrder.fVersion);
        }
      }
    }

    if(paramOk == kResultOk)
    {
      newIds.emplace_back(id);
    }

    res |= paramOk;

  }

  if(!deprecatedParam)
  {
    DLOG_F(WARNING, "RTDeprecatedSaveStateOrder for version %d does not contain any deprecated parameter.",  iSaveOrder.fVersion);
  }

  fGUIDeprecatedSaveStateOrders[iSaveOrder.fVersion] = {iSaveOrder.fVersion, newIds};

  return res;
}

//------------------------------------------------------------------------
// Parameters::getGUISaveStateOrder
//------------------------------------------------------------------------
NormalizedState::SaveOrder const *Parameters::getGUIDeprecatedSaveStateOrder(int16 iVersion) const
{
  auto iter = fGUIDeprecatedSaveStateOrders.find(iVersion);

  if(iter != fGUIDeprecatedSaveStateOrders.end())
    return &iter->second;
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Parameters::RawVstParamDefBuilder::add
//------------------------------------------------------------------------
RawVstParam Parameters::RawVstParamDefBuilder::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::add
//------------------------------------------------------------------------
RawVstParam Parameters::add(RawVstParamDefBuilder const &iBuilder)
{
  auto param = std::make_shared<RawVstParamDef>(iBuilder.fParamID,
                                                iBuilder.fTitle,
                                                iBuilder.fUnits,
                                                iBuilder.fDefaultValue,
                                                iBuilder.fStepCount,
                                                iBuilder.fFlags,
                                                iBuilder.fUnitID,
                                                iBuilder.fShortTitle,
                                                iBuilder.fPrecision,
                                                iBuilder.fOwner,
                                                iBuilder.fTransient,
                                                iBuilder.fDeprecatedSince);

  if(addVstParamDef(param) == kResultOk)
    return param;
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Parameters::raw
//------------------------------------------------------------------------
Parameters::RawVstParamDefBuilder Parameters::raw(ParamID iParamID, VstString16 iTitle)
{
  return Parameters::RawVstParamDefBuilder(this, iParamID, std::move(iTitle));
}

//------------------------------------------------------------------------
// Parameters::addParamID
//------------------------------------------------------------------------
tresult Parameters::addParamID(std::vector<ParamID> &oParamIDs, ParamID iParamID)
{
  tresult res = kResultOk;

  if(fVstParams.find(iParamID) != fVstParams.cend() ||
     fJmbParams.find(iParamID) != fJmbParams.cend())
  {
    oParamIDs.emplace_back(iParamID);
  }
  else
  {
    DLOG_F(ERROR, "No such parameter [%d]", iParamID);
    res = kResultFalse;
  }
  return res;
}

}