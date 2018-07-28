#ifndef __PONGASOFT_VST_PARAMETERS_H__
#define __PONGASOFT_VST_PARAMETERS_H__

#include "ParamDef.h"

#include <map>
#include <vector>
#include <set>

#include <public.sdk/source/vst/vstparameters.h>

namespace pongasoft {
namespace VST {

/**
 * This is the class which maintains all the registered parameters
 * TODO provide example on how to use this class
 */
class Parameters
{
public:
  /**
   * Implements the builder pattern for ease of build.
   * @tparam ParamConverter the converter (see ParamConverters.h for an explanation of what is expected)
   */
  template<typename ParamConverter>
  struct ParamDefBuilder
  {
    // builder methods
    ParamDefBuilder &units(const TChar *iUnits) { fUnits = iUnits; return *this; }
    ParamDefBuilder &defaultValue(typename ParamConverter::ParamType const &iDefaultValue) { fDefaultNormalizedValue = ParamConverter::normalize(iDefaultValue); return *this;}
    ParamDefBuilder &stepCount(int32 iStepCount) { fStepCount = iStepCount; return *this; }
    ParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    ParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    ParamDefBuilder &shortTitle(const TChar *iShortTitle) { fShortTitle = iShortTitle; return *this; }
    ParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    ParamDefBuilder &uiOnly(bool iUIOnly = true) { fUIOnly = iUIOnly; return *this; }
    ParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }

    // parameter factory method
    ParamDefSPtr<ParamConverter> add() const;

    // fields
    ParamID fParamID;
    const TChar *fTitle;
    const TChar *fUnits = nullptr;
    ParamValue fDefaultNormalizedValue = 0;
    int32 fStepCount = 0;
    int32 fFlags = ParameterInfo::kCanAutomate;
    UnitID fUnitID = kRootUnitId;
    const TChar *fShortTitle = nullptr;
    int32 fPrecision = 4;
    bool fUIOnly = false;
    bool fTransient = false;

    friend class Parameters;

  protected:
    ParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParameters{iParameters}, fParamID{iParamID}, fTitle{iTitle} {}

  private:
    Parameters *fParameters;
  };

public:
  /**
   * Maintains the order used to save/restore the RT and GUI state
   */
  struct SaveStateOrder
  {
    uint16 fVersion{0};
    std::vector<ParamID> fOrder{};
    int getParamCount() const { return fOrder.size(); }
  };

public:
  // Constructor
  explicit Parameters() = default;

  // Ensure the class is polymorphinc
  virtual ~Parameters() = default;

  /**
   * Used from derived classes to build a parameter.
   * TODO add example + don't forget that order is important (define the order in Maschine for example)
   */
  template<typename ParamConverter>
  ParamDefBuilder<ParamConverter> build(ParamID iParamID, const TChar* iTitle);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   *
   * @tparam Args can be any combination of ParamID, RawParamDef or ParamDef<ParamConverter>
   */
  template<typename... Args>
  void setRTSaveStateOrder(uint16 iVersion, Args... args);

  /**
   * @return the order used when saving the GUI state (getState/setState in the controller)
   */
  SaveStateOrder getGUISaveStateOrder() const { return fGUISaveStateOrder; }

  /**
   * Used to change the default order (registration order) used when saving the GUI state (getState/setState in
   * the controller)
   *
   * @tparam Args can be any combination of ParamID, RawParamDef or ParamDef<ParamConverter>
   */
  template<typename... Args>
  void setGUISaveStateOrder(uint16 iVersion, Args... args);

  /**
   * @return the order used when saving the RT state (getState/setState in the processor, setComponentState in
   *         the controller)
   */
  SaveStateOrder getRTSaveStateOrder() const { return fRTSaveStateOrder; }

  /**
   * This method is called from the GUI controller to register all the parameters to the ParameterContainer class
   * which is the class managing the parameters in the vst sdk
   */
  void registerVstParameters(Vst::ParameterContainer &iParameterContainer) const;

  // getRawParamDef - nullptr when not found
  std::shared_ptr<RawParamDef> getRawParamDef(ParamID iParamID) const;

protected:
  // internally called by the builder
  template<typename ParamConverter>
  ParamDefSPtr<ParamConverter> add(ParamDefBuilder<ParamConverter> const &iBuilder);

  void addRawParamDef(std::shared_ptr<RawParamDef> iParamDef);

private:
  // contains all the registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<RawParamDef>> fParameters{};

  // order in which the parameters will be registered in the plugin
  std::vector<ParamID> fPluginOrder{};

  // TODO: Handle multiple versions with upgrade
  SaveStateOrder fRTSaveStateOrder{};
  SaveStateOrder fGUISaveStateOrder{};

  // leaf of templated calls to build a list of ParamIDs from ParamID or ParamDefs
  void buildParamIDs(std::vector<ParamID> &iParamIDs) {}

  // case when ParamID
  template<typename... Args>
  void buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID id, Args... args);

  // case when RawParamDef
  template<typename... Args>
  void buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<RawParamDef> &iParamDef, Args... args)
  {
    buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

  // case when ParamDef
  template<typename ParamConverver, typename... Args>
  void buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<ParamDef<ParamConverver>> &iParamDef, Args... args)
  {
    buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

};

//------------------------------------------------------------------------
// Parameters::ParamDefBuilder::add
//------------------------------------------------------------------------
template<typename ParamConverter>
ParamDefSPtr<ParamConverter> Parameters::ParamDefBuilder<ParamConverter>::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::add (called by the builder)
//------------------------------------------------------------------------
template<typename ParamConverter>
ParamDefSPtr<ParamConverter> Parameters::add(ParamDefBuilder<ParamConverter> const &iBuilder)
{
  auto param = std::make_shared<ParamDef<ParamConverter>>(iBuilder.fParamID,
                                                          iBuilder.fTitle,
                                                          iBuilder.fUnits,
                                                          iBuilder.fDefaultNormalizedValue,
                                                          iBuilder.fStepCount,
                                                          iBuilder.fFlags,
                                                          iBuilder.fUnitID,
                                                          iBuilder.fShortTitle,
                                                          iBuilder.fPrecision,
                                                          iBuilder.fUIOnly,
                                                          iBuilder.fTransient);

  addRawParamDef(param);

  return param;
}

//------------------------------------------------------------------------
// Parameters::build
//------------------------------------------------------------------------
template<typename ParamConverter>
Parameters::ParamDefBuilder<ParamConverter> Parameters::build(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::ParamDefBuilder<ParamConverter>(this, iParamID, iTitle);
}

//------------------------------------------------------------------------
// Parameters::buildParamIDs
//------------------------------------------------------------------------
template<typename... Args>
void Parameters::buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID iParamID, Args... args)
{
  auto iter = fParameters.find(iParamID);
  if(iter != fParameters.cend())
  {
    iParamIDs.emplace_back(iParamID);
  }
  else
  {
    ABORT_F("No such parameter %d", iParamID);
  }
  buildParamIDs(iParamIDs, args...);
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
void Parameters::setRTSaveStateOrder(uint16 iVersion, Args... args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, args...);

  bool ok = true;

  for(auto id : ids)
  {
    if(fParameters.at(id)->fTransient)
    {
      ok = false;
      DLOG_F(ERROR,
             "Param [%d] cannot be used for RTSaveStateOrder as it is defined transient",
             id);
    }

    if(fParameters.at(id)->fUIOnly)
    {
      ok = false;
      DLOG_F(ERROR,
             "Param [%d] cannot be used for RTSaveStateOrder as it is defined UIOnly",
             id);

    }
  }

  for(auto p : fParameters)
  {
    auto param = p.second;
    if(!param->fUIOnly && !param->fTransient)
    {
      if(std::find(ids.cbegin(), ids.cend(), p.first) == ids.cend())
      {
        DLOG_F(WARNING, "Param [%d] is not marked transient. Either mark the parameter transient or add it to RTSaveStateOrder", p.first);
      }
    }
  }

  DCHECK_F(ok, "Issue with setRTSaveStateOrder... failing in development mode");

  fRTSaveStateOrder = {iVersion, ids};
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
void Parameters::setGUISaveStateOrder(uint16 iVersion, Args... args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, args...);

  bool ok = true;

  for(auto id : ids)
  {
    if(fParameters.at(id)->fTransient)
    {
      ok = false;
      DLOG_F(ERROR,
             "Param [%d] cannot be used for GUISaveStateOrder as it is defined transient",
             id);
    }

    if(!fParameters.at(id)->fUIOnly)
    {
      ok = false;
      DLOG_F(ERROR,
             "Param [%d] cannot be used for GUISaveStateOrder as it is not defined UIOnly",
             id);

    }
  }

  for(auto p : fParameters)
  {
    auto param = p.second;
    if(param->fUIOnly && !param->fTransient)
    {
      if(std::find(ids.cbegin(), ids.cend(), p.first) == ids.cend())
      {
        DLOG_F(WARNING, "Param [%d] is not marked transient. Either mark the parameter transient or add it to GUISaveStateOrder", p.first);
      }
    }
  }

  DCHECK_F(ok, "Issue with setGUISaveStateOrder... failing in development mode");

  fGUISaveStateOrder = {iVersion, ids};
}

//------------------------------------------------------------------------
// Parameters::build - specialization for BooleanParamConverter
//------------------------------------------------------------------------
template<>
Parameters::ParamDefBuilder<BooleanParamConverter> Parameters::build(ParamID iParamID, const TChar *iTitle);

// TODO should handle DiscreteValueParamConverter (because it is templated, it doesn't seem that I can do like BooleanParamConverter)
// check https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature

}
}

#endif // __PONGASOFT_VST_PARAMETERS_H__