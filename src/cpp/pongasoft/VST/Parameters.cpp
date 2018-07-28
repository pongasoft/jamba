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
  explicit VstParameterImpl(std::shared_ptr<RawParamDef> const &iParamDef) :
    fParamDef{iParamDef},
    Vst::Parameter(iParamDef->fTitle,
                   iParamDef->fParamID,
                   iParamDef->fUnits,
                   iParamDef->fDefaultNormalizedValue,
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
  std::shared_ptr<RawParamDef> fParamDef;
};

//------------------------------------------------------------------------
// Parameters::registerVstParameters
//------------------------------------------------------------------------
void Parameters::registerVstParameters(Vst::ParameterContainer &iParameterContainer) const
{
  for(auto paramID : fPluginOrder)
  {
    // YP Note: ParameterContainer is expecting a pointer and then assumes ownership
    iParameterContainer.addParameter(new VstParameterImpl(fParameters.at(paramID)));
  }
}

//------------------------------------------------------------------------
// Parameters::getRawParamDef
//------------------------------------------------------------------------
std::shared_ptr<RawParamDef> Parameters::getRawParamDef(ParamID iParamID) const
{
  auto iter = fParameters.find(iParamID);
  if(iter == fParameters.cend())
    return nullptr;

  return iter->second;
}

//------------------------------------------------------------------------
// Parameters::addRawParamDef
//------------------------------------------------------------------------
void Parameters::addRawParamDef(std::shared_ptr<RawParamDef> iParamDef)
{
  if(fParameters.find(iParamDef->fParamID) != fParameters.cend())
  {
    ABORT_F("Parameter [%d] already registered", iParamDef->fParamID);
  }

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "Parameters::addRawParamDef{%d, \"%s\", \"%s\", %f, %d, %d, %d, \"%s\", %d%s%s}",
         iParamDef->fParamID,
         String(iParamDef->fTitle).text8(),
         String(iParamDef->fUnits).text8(),
         iParamDef->fDefaultNormalizedValue,
         iParamDef->fStepCount,
         iParamDef->fFlags,
         iParamDef->fUnitID,
         String(iParamDef->fShortTitle).text8(),
         iParamDef->fPrecision,
         iParamDef->fUIOnly ? ", uiOnly" : "",
         iParamDef->fTransient ? ", transient" : "");
#endif

  fParameters[iParamDef->fParamID] = iParamDef;

  fPluginOrder.emplace_back(iParamDef->fParamID);

  if(!iParamDef->fTransient)
  {
    if(iParamDef->fUIOnly)
      fGUISaveStateOrder.fOrder.emplace_back(iParamDef->fParamID);
    else
      fRTSaveStateOrder.fOrder.emplace_back(iParamDef->fParamID);
  }
}

//------------------------------------------------------------------------
// Parameters::build<BooleanParamConverter> => make sure stepCount is 1
//------------------------------------------------------------------------
template<>
Parameters::ParamDefBuilder<BooleanParamConverter> Parameters::build(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::ParamDefBuilder<BooleanParamConverter>(this, iParamID, iTitle).stepCount(1);
}



}
}