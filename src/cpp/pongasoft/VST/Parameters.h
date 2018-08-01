#ifndef __PONGASOFT_VST_PARAMETERS_H__
#define __PONGASOFT_VST_PARAMETERS_H__

#include "ParamDef.h"
#include "NormalizedState.h"

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
   * Interface to implement to receive parameter changes
   */
  class IChangeListener
  {
  public:
    virtual void onParameterChange(ParamID iParamID) = 0;
  };

public:
  /**
   * Implements the builder pattern for ease of build.
   * @tparam ParamConverter the converter (see ParamConverters.h for an explanation of what is expected)
   */
  template<typename ParamConverter>
  struct VstParamDefBuilder
  {
    // builder methods
    VstParamDefBuilder &units(const TChar *iUnits) { fUnits = iUnits; return *this; }
    VstParamDefBuilder &defaultValue(typename ParamConverter::ParamType const &iDefaultValue) { fDefaultNormalizedValue = ParamConverter::normalize(iDefaultValue); return *this;}
    VstParamDefBuilder &stepCount(int32 iStepCount) { fStepCount = iStepCount; return *this; }
    VstParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    VstParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    VstParamDefBuilder &shortTitle(const TChar *iShortTitle) { fShortTitle = iShortTitle; return *this; }
    VstParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    VstParamDefBuilder &uiOnly(bool iUIOnly = true) { fUIOnly = iUIOnly; return *this; }
    VstParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }

    // parameter factory method
    VstParam<ParamConverter> add() const;

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
    VstParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParameters{iParameters}, fParamID{iParamID}, fTitle{iTitle} {}

  private:
    Parameters *fParameters;
  };

  /**
   * Implements the builder pattern for ease of build.
   * @tparam ParamSerializer the serializer (see ParamSerializers.h for an explanation of what is expected) */
  template<typename ParamSerializer>
  struct SerParamDefBuilder
  {
    // builder methods
    SerParamDefBuilder &defaultValue(typename ParamSerializer::ParamType const &iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    SerParamDefBuilder &uiOnly(bool iUIOnly = true) { fUIOnly = iUIOnly; return *this; }
    SerParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }

    // parameter factory method
    SerParam<ParamSerializer> add() const;

    // fields
    ParamID fParamID;
    const TChar *fTitle;
    typename ParamSerializer::ParamType fDefaultValue{};
    bool fUIOnly = false;
    bool fTransient = false;

    friend class Parameters;

  protected:
    SerParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParameters{iParameters}, fParamID{iParamID}, fTitle{iTitle} {}

  private:
    Parameters *fParameters;
  };


public:
  // Constructor
  explicit Parameters() = default;

  // Ensure the class is polymorphinc
  virtual ~Parameters() = default;

  /**
   * Used from derived classes to build a parameter backed by a VST parameter
   * TODO add example + don't forget that order is important (define the order in Maschine for example)
   */
  template<typename ParamConverter>
  VstParamDefBuilder<ParamConverter> vst(ParamID iParamID, const TChar *iTitle);

  /**
   * Used from derived classes to build a parameter of any type not convertible to a ParamValue
   */
  template<typename ParamSerializer>
  SerParamDefBuilder<ParamSerializer> ser(ParamID iParamID, const TChar *iTitle);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @tparam Args can be any combination of ParamID, RawParamDef or ParamDef<ParamConverter>
   */
  template<typename... Args>
  tresult setRTSaveStateOrder(int16 iVersion, Args... args);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   */
  tresult setRTSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder);

  /**
   * @return the order used when saving the GUI state (getState/setState in the controller)
   */
  NormalizedState::SaveOrder const &getGUISaveStateOrder() const { return fGUISaveStateOrder; }

  /**
   * Used to change the default order (registration order) used when saving the GUI state (getState/setState in
   * the controller)
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @tparam Args can be any combination of ParamID, RawVstParamDef, VstParamDef<ParamConverter>,
   */
  template<typename... Args>
  tresult setGUISaveStateOrder(int16 iVersion, Args... args);

  /**
   * Used to change the default order (registration order) used when saving the GUI state (getState/setState in
   * the controller)
   */
  tresult setGUISaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder);

  /**
   * @return the order used when saving the RT state (getState/setState in the processor, setComponentState in
   *         the controller)
   */
  NormalizedState::SaveOrder const &getRTSaveStateOrder() const { return fRTSaveStateOrder; }

  /**
   * This method is called from the GUI controller to register all the parameters to the ParameterContainer class
   * which is the class managing the parameters in the vst sdk
   */
  void registerVstParameters(Vst::ParameterContainer &iParameterContainer) const;

  /**
   * This method is called to read the RTState from the stream */
  virtual std::unique_ptr<NormalizedState> readRTState(IBStreamer &iStreamer) const;

  /**
   * This method is called to write the NormalizedState to the stream */
  virtual tresult writeRTState(NormalizedState const *iNormalizedState, IBStreamer &oStreamer) const;

  /**
   * @return a new normalized state for RT (can be overridden to return a subclass!)
   */
  virtual std::unique_ptr<NormalizedState> newRTState() const;

  /**
   * @return normalized value read from the stream for the given parameter
   */
  virtual ParamValue readNormalizedValue(ParamID iParamID, IBStreamer &iStreamer) const;

  // getParamDef - nullptr when not found
  std::shared_ptr<RawVstParamDef> getRawVstParamDef(ParamID iParamID) const;

  // getSerParamDef - nullptr when not found
  std::shared_ptr<IParamDef> getSerParamDef(ParamID iParamID) const;

protected:
  // internally called by the builder
  template<typename ParamConverter>
  VstParam<ParamConverter> add(VstParamDefBuilder<ParamConverter> const &iBuilder);

  // internally called by the builder
  template<typename ParamSerializer>
  SerParam<ParamSerializer> add(SerParamDefBuilder<ParamSerializer> const &iBuilder);

  // addVstParamDef
  void addVstParamDef(std::shared_ptr<RawVstParamDef> iParamDef);

  // addSerParamDef
  void addSerParamDef(std::shared_ptr<ISerParamDef> iParamDef);

private:
  // contains all the registered (raw type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<RawVstParamDef>> fVstParams{};

  // contains all the registered (serializable type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<ISerParamDef>> fSerParams{};

  // order in which the parameters will be registered in the plugin
  std::vector<ParamID> fPluginOrder{};

  // TODO: Handle multiple versions with upgrade
  NormalizedState::SaveOrder fRTSaveStateOrder{};
  NormalizedState::SaveOrder fGUISaveStateOrder{};

private:
  // leaf of templated calls to build a list of ParamIDs from ParamID or ParamDefs
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs) { return kResultOk; }

  // case when ParamID
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID id, Args... args);

  // case when ISerParamDef
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<ISerParamDef> &iParamDef, Args... args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

  // case when ISerParamDef
  template<typename ParamSerializer, typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<SerParamDef<ParamSerializer>> &iParamDef, Args... args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

  // case when VstParamDef
  template<typename ParamConverver, typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<VstParamDef<ParamConverver>> &iParamDef, Args... args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

  // case when RawVstParamDef
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<RawVstParamDef> &iParamDef, Args... args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

};

//------------------------------------------------------------------------
// Parameters::SerParamDefBuilder::add
//------------------------------------------------------------------------
template<typename ParamSerializer>
SerParam<ParamSerializer> Parameters::SerParamDefBuilder<ParamSerializer>::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::VstParamDefBuilder::add
//------------------------------------------------------------------------
template<typename ParamConverter>
VstParam<ParamConverter> Parameters::VstParamDefBuilder<ParamConverter>::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::add (called by the builder)
//------------------------------------------------------------------------
template<typename ParamConverter>
VstParam<ParamConverter> Parameters::add(VstParamDefBuilder<ParamConverter> const &iBuilder)
{
  auto param = std::make_shared<VstParamDef<ParamConverter>>(iBuilder.fParamID,
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

  addVstParamDef(param);

  return param;
}

//------------------------------------------------------------------------
// Parameters::add (called by the builder)
//------------------------------------------------------------------------
template<typename ParamSerializer>
SerParam<ParamSerializer> Parameters::add(Parameters::SerParamDefBuilder<ParamSerializer> const &iBuilder)
{
  auto param = std::make_shared<SerParamDef<ParamSerializer>>(iBuilder.fParamID,
                                                              iBuilder.fTitle,
                                                              iBuilder.fUIOnly,
                                                              iBuilder.fTransient,
                                                              iBuilder.fDefaultValue);

  addSerParamDef(param);

  return param;
}


//------------------------------------------------------------------------
// Parameters::build
//------------------------------------------------------------------------
template<typename ParamConverter>
Parameters::VstParamDefBuilder<ParamConverter> Parameters::vst(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::VstParamDefBuilder<ParamConverter>(this, iParamID, iTitle);
}

//------------------------------------------------------------------------
// Parameters::ser
//------------------------------------------------------------------------
template<typename ParamSerializer>
Parameters::SerParamDefBuilder<ParamSerializer> Parameters::ser(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::SerParamDefBuilder<ParamSerializer>(this, iParamID, iTitle);
}

//------------------------------------------------------------------------
// Parameters::buildParamIDs
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID iParamID, Args... args)
{
  tresult res = kResultOk;

  auto iter = fVstParams.find(iParamID);
  if(fVstParams.find(iParamID) != fVstParams.cend() ||
     fSerParams.find(iParamID) != fSerParams.cend())
  {
    iParamIDs.emplace_back(iParamID);
  }
  else
  {
    DLOG_F(ERROR, "No such parameter [%d]", iParamID);
    res = kResultFalse;
  }
  res |= buildParamIDs(iParamIDs, args...);
  return res;
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setRTSaveStateOrder(int16 iVersion, Args... args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, args...);
  return setRTSaveStateOrder({iVersion, ids});
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setGUISaveStateOrder(int16 iVersion, Args... args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, args...);
  return setGUISaveStateOrder({iVersion, ids});
}

//------------------------------------------------------------------------
// Parameters::build - specialization for BooleanParamConverter
//------------------------------------------------------------------------
template<>
Parameters::VstParamDefBuilder<BooleanParamConverter> Parameters::vst(ParamID iParamID, const TChar *iTitle);

// TODO should handle DiscreteValueParamConverter (because it is templated, it doesn't seem that I can do like BooleanParamConverter)
// check https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature

}
}

#endif // __PONGASOFT_VST_PARAMETERS_H__