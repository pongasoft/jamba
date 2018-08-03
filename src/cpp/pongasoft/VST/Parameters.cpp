#include <base/source/fstring.h>
#include "Parameters.h"

namespace pongasoft {
namespace VST {

/**
 * Internal class which extends the Vst::Parameter to override toString and delegate to the param def
 */
class VstParameterImpl : public Vst::Parameter
{
public:
  explicit VstParameterImpl(std::shared_ptr<RawVstParamDef> const &iParamDef) :
    fParamDef{iParamDef},
    Vst::Parameter(iParamDef->fTitle,
                   iParamDef->fParamID,
                   iParamDef->fUnits,
                   iParamDef->fDefaultValue,
                   iParamDef->fStepCount,
                   iParamDef->fFlags,
                   iParamDef->fUnitID,
                   iParamDef->fShortTitle)
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
  for(auto paramID : fPluginOrder)
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
std::unique_ptr<NormalizedState> Parameters::readRTState(IBStreamer &iStreamer) const
{
  // ignoring version if negative
  if(fRTSaveStateOrder.fVersion >= 0)
  {
    uint16 stateVersion = __readStateVersion(iStreamer);

    // TODO handle multiple versions
    if(stateVersion != fRTSaveStateOrder.fVersion)
    {
      DLOG_F(WARNING, "unexpected RT state version %d", stateVersion);
    }
  }

  // YP Implementation note: It is OK to allocate memory here because this method is called by the GUI!!!
  auto normalizedState = newRTState();

  if(normalizedState->readFromStream(this, iStreamer) == kResultOk)
    return normalizedState;

  return nullptr;
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
// Parameters::getSerParamDef
//------------------------------------------------------------------------
std::shared_ptr<IParamDef> Parameters::getSerParamDef(ParamID iParamID) const
{
  auto iter = fSerParams.find(iParamID);
  if(iter == fSerParams.cend())
    return nullptr;

  return iter->second;
}

//------------------------------------------------------------------------
// Parameters::addVstParamDef
//------------------------------------------------------------------------
void Parameters::addVstParamDef(std::shared_ptr<RawVstParamDef> iParamDef)
{
  ParamID paramID = iParamDef->fParamID;

  if(fVstParams.find(paramID) != fVstParams.cend())
  {
    ABORT_F("Parameter [%d] already registered", paramID);
  }

  if(fSerParams.find(paramID) != fSerParams.cend())
  {
    ABORT_F("Parameter [%d] already registered", paramID);
  }

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "Parameters::addVstParamDef{%d, \"%s\", \"%s\", %f, %d, %d, %d, \"%s\", %d%s%s}",
         paramID,
         String(iParamDef->fTitle).text8(),
         String(iParamDef->fUnits).text8(),
         iParamDef->fDefaultValue,
         iParamDef->fStepCount,
         iParamDef->fFlags,
         iParamDef->fUnitID,
         String(iParamDef->fShortTitle).text8(),
         iParamDef->fPrecision,
         iParamDef->fUIOnly ? ", uiOnly" : "",
         iParamDef->fTransient ? ", transient" : "");
#endif

  fVstParams[paramID] = iParamDef;

  fPluginOrder.emplace_back(paramID);

  if(!iParamDef->fTransient)
  {
    if(iParamDef->fUIOnly)
      fGUISaveStateOrder.fOrder.emplace_back(paramID);
    else
      fRTSaveStateOrder.fOrder.emplace_back(paramID);
  }
}

//------------------------------------------------------------------------
// Parameters::addSerParamDef
//------------------------------------------------------------------------
void Parameters::addSerParamDef(std::shared_ptr<ISerParamDef> iParamDef)
{
  ParamID paramID = iParamDef->fParamID;
  
  if(fVstParams.find(paramID) != fVstParams.cend())
  {
    ABORT_F("Parameter [%d] already registered", paramID);
  }

  if(fSerParams.find(paramID) != fSerParams.cend())
  {
    ABORT_F("Parameter [%d] already registered", paramID);
  }

  DCHECK_F(iParamDef->fUIOnly, "Serializable parameter [%d] must be marked UIOnly (not supported in RT for now)", paramID);

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "Parameters::addSerParamDef{%d, \"%s\", %s%s}",
         paramID,
         String(iParamDef->fTitle).text8(),
         iParamDef->fUIOnly ? ", uiOnly" : "",
         iParamDef->fTransient ? ", transient" : "");
#endif

  fSerParams[paramID] = iParamDef;
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
             "Param [%d] was not registered as a vst parameter (did you register as a ser parameter?)",
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

      if(param->fUIOnly)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for RTSaveStateOrder as it is defined UIOnly",
               id);

      }
    }

    if(paramOk == kResultOk)
    {
      newIds.emplace_back(id);
    }

    res |= paramOk;
  }

  for(auto p : fVstParams)
  {
    auto param = p.second;
    if(!param->fUIOnly && !param->fTransient)
    {
      if(std::find(newIds.cbegin(), newIds.cend(), p.first) == newIds.cend())
      {
        DLOG_F(WARNING, "Param [%d] is not marked transient. Either mark the parameter transient or add it to RTSaveStateOrder", p.first);
      }
    }
  }

  fRTSaveStateOrder = {iSaveOrder.fVersion, newIds};

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

  for(auto &&p : fSerParams)
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
             "Param [%d] was not registered as a vst or ser parameter",
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

      if(!param->fUIOnly)
      {
        paramOk = kResultFalse;
        DLOG_F(ERROR,
               "Param [%d] cannot be used for GUISaveStateOrder as it is not defined UIOnly",
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
    if(param->fUIOnly && !param->fTransient)
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
// Parameters::build<bool> => make sure stepCount is 1
//------------------------------------------------------------------------
template<>
Parameters::VstParamDefBuilder<bool> Parameters::vstFromType(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::VstParamDefBuilder<bool>(this, iParamID, iTitle).stepCount(1);
}



}
}