/*
 * Copyright (c) 2018 pongasoft
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

namespace Debug { class ParamDisplay; }

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

  /**
   * A callback that will be invoked for changes. Note that unlike the IChangeListener, there is no
   * parameter since it is expected to be used like this:
   *
   * fMyParam = registerVstParam(myParamId, [this]() { .... ; });
   */
  using ChangeCallback = std::function<void()>;

  /**
   * A callback which will be invoked for changes with the param as an argument.
   */
  template<typename Param>
  using ChangeCallback1 = std::function<void(Param &)>;

  /**
   * A callback which will be invoked for changes with the view and param as arguments.
   */
  template<typename TView, typename Param>
  using ChangeCallback2 = std::function<void(TView *, Param &)>;

public:
  /**
   * Implements the builder pattern for ease of build. */
  struct RawVstParamDefBuilder
  {
    // builder methods
    RawVstParamDefBuilder &units(const TChar *iUnits) { fUnits = iUnits; return *this; }
    RawVstParamDefBuilder &defaultValue(ParamValue iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    RawVstParamDefBuilder &stepCount(int32 iStepCount) { fStepCount = iStepCount; return *this;}
    RawVstParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    RawVstParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    RawVstParamDefBuilder &shortTitle(const TChar *iShortTitle) { fShortTitle = iShortTitle; return *this; }
    RawVstParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    RawVstParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    RawVstParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    RawVstParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }

    // parameter factory method
    RawVstParam add() const;

    // fields
    ParamID fParamID;
    const TChar *fTitle;
    const TChar *fUnits = nullptr;
    ParamValue fDefaultValue{};
    int32 fStepCount{0};
    int32 fFlags = ParameterInfo::kCanAutomate;
    UnitID fUnitID = kRootUnitId;
    const TChar *fShortTitle = nullptr;
    int32 fPrecision = 4;
    IParamDef::Owner fOwner = IParamDef::Owner::kRT;
    bool fTransient = false;

    friend class Parameters;

  protected:
    RawVstParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParamID{iParamID}, fTitle{iTitle}, fParameters{iParameters} {}

  private:
    Parameters *fParameters;
  };

  /**
   * Implements the builder pattern for ease of build.
   * @tparam T the underlying type of the param */
  template<typename T>
  struct VstParamDefBuilder
  {
    // builder methods
    VstParamDefBuilder &units(const TChar *iUnits) { fUnits = iUnits; return *this; }
    VstParamDefBuilder &defaultValue(T const &iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    VstParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    VstParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    VstParamDefBuilder &shortTitle(const TChar *iShortTitle) { fShortTitle = iShortTitle; return *this; }
    VstParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    VstParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    VstParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    VstParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }
    VstParamDefBuilder &converter(std::shared_ptr<IParamConverter<T>> iConverter) { fConverter = std::move(iConverter); return *this; }
    template<typename ParamConverter, typename... Args>
    VstParamDefBuilder &converter(Args... iArgs) { fConverter = std::make_shared<ParamConverter>(iArgs...); return *this; }

    // parameter factory method
    VstParam<T> add() const;

    // fields
    ParamID fParamID;
    const TChar *fTitle;
    const TChar *fUnits = nullptr;
    T fDefaultValue{};
    int32 fFlags = ParameterInfo::kCanAutomate;
    UnitID fUnitID = kRootUnitId;
    const TChar *fShortTitle = nullptr;
    int32 fPrecision = 4;
    IParamDef::Owner fOwner = IParamDef::Owner::kRT;
    bool fTransient = false;
    std::shared_ptr<IParamConverter<T>> fConverter{};

    friend class Parameters;

  protected:
    VstParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParamID{iParamID}, fTitle{iTitle}, fParameters{iParameters} {}

  private:
    Parameters *fParameters;
  };

  /**
   * Implements the builder pattern for ease of build.
   * @tparam T the underlying type of the param */
  template<typename T>
  struct JmbParamDefBuilder
  {
    // builder methods
    JmbParamDefBuilder &defaultValue(T const &iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    JmbParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }
    JmbParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    JmbParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    JmbParamDefBuilder &shared(bool iShared = true) { fShared = iShared; return *this; }
    JmbParamDefBuilder &serializer(std::shared_ptr<IParamSerializer<T>> iSerializer) { fSerializer = std::move(iSerializer); return *this; }
    template<typename ParamSerializer, typename... Args>
    JmbParamDefBuilder &serializer(Args... iArgs) { fSerializer = std::make_shared<ParamSerializer>(iArgs...); return *this; }

    // parameter factory method
    JmbParam<T> add() const;

    // fields
    ParamID fParamID;
    const TChar *fTitle;
    T fDefaultValue{};
    IParamDef::Owner fOwner = IParamDef::Owner::kGUI;
    bool fTransient = false;
    bool fShared = false;
    std::shared_ptr<IParamSerializer<T>> fSerializer{};

    friend class Parameters;

  protected:
    JmbParamDefBuilder(Parameters *iParameters, ParamID iParamID, const TChar* iTitle) :
      fParamID{iParamID}, fTitle{iTitle}, fParameters{iParameters} {}

  private:
    Parameters *fParameters;
  };

public:
  // Constructor
  explicit Parameters() = default;

  // Ensure the class is polymorphinc
  virtual ~Parameters() = default;


  /**
   * Used from derived classes to build a parameter backed by a raw VST parameter
   */
  RawVstParamDefBuilder raw(ParamID iParamID, const TChar *iTitle);

  /**
   * Used from derived classes to build a parameter backed by a VST parameter
   * TODO add example + don't forget that order is important (define the order in Maschine for example)
   */
  template<typename ParamConverter, typename... Args>
  VstParamDefBuilder<typename ParamConverter::ParamType> vst(ParamID iParamID, const TChar *iTitle, Args... iConverterArgs);

  /**
   * Used from derived classes to build a parameter backed by a VST parameter. Use this version
   * if you want to provide a different converter.
   */
  template<typename T>
  VstParamDefBuilder<T> vstFromType(ParamID iParamID, const TChar *iTitle);

  /**
   * Used from derived classes to build a non vst parameter (not convertible to a ParamValue)
   */
  template<typename ParamSerializer, typename... Args>
  JmbParamDefBuilder<typename ParamSerializer::ParamType> jmb(ParamID iParamID, const TChar *iTitle, Args... iSerializerArgs);

  /**
   * Used from derived classes to build a non vst parameter (not convertible to a ParamValue). Use this version
   * if you want to provide a different serializer.
   */
  template<typename T>
  JmbParamDefBuilder<T> jmbFromType(ParamID iParamID, const TChar *iTitle);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @tparam Args can be any combination of ParamID, RawParamDef or VstParamDef<T>
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
   * @tparam Args can be any combination of ParamID, RawVstParamDef, VstParamDef<T>, JmbParamDef<T>
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
  virtual tresult readRTState(IBStreamer &iStreamer, NormalizedState *oNormalizedState) const;

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

  // getJmbParamDef - nullptr when not found
  std::shared_ptr<IJmbParamDef> getJmbParamDef(ParamID iParamID) const;

  // getVstRegistrationOrder
  std::vector<ParamID> const &getVstRegistrationOrder() const { return fVstRegistrationOrder; }

  // getAllRegistrationOrder
  std::vector<ParamID> const &getAllRegistrationOrder() const { return fAllRegistrationOrder; }

  // gives access for debug
  friend class Debug::ParamDisplay;

protected:
  // internally called by the builder
  RawVstParam add(RawVstParamDefBuilder const &iBuilder);

  // internally called by the builder
  template<typename T>
  VstParam<T> add(VstParamDefBuilder<T> const &iBuilder);

  // internally called by the builder
  template<typename T>
  JmbParam<T> add(JmbParamDefBuilder<T> const &iBuilder);

  // addVstParamDef
  tresult addVstParamDef(std::shared_ptr<RawVstParamDef> iParamDef);

  // addJmbParamDef
  tresult addJmbParamDef(std::shared_ptr<IJmbParamDef> iParamDef);

private:
  // contains all the registered (raw type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<RawVstParamDef>> fVstParams{};

  // contains all the registered (serializable type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<IJmbParamDef>> fJmbParams{};

  // order in which the parameters will be registered in the vst world
  std::vector<ParamID> fVstRegistrationOrder{};

  // order in which the parameters were registered
  std::vector<ParamID> fAllRegistrationOrder{};

  // TODO: Handle multiple versions with upgrade
  NormalizedState::SaveOrder fRTSaveStateOrder{};
  NormalizedState::SaveOrder fGUISaveStateOrder{};

private:
  // leaf of templated calls to build a list of ParamIDs from ParamID or ParamDefs
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs) { return kResultOk; }

  // case when ParamID
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID id, Args... args);

  // case when IJmbParamDef
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<IJmbParamDef> &iParamDef, Args... args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, args...);
  }

  // case when IJmbParamDef
  template<typename T, typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<JmbParamDef<T>> &iParamDef, Args... args)
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
// Parameters::JmbParamDefBuilder::add
//------------------------------------------------------------------------
template<typename T>
JmbParam<T> Parameters::JmbParamDefBuilder<T>::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::VstParamDefBuilder::add
//------------------------------------------------------------------------
template<typename T>
VstParam<T> Parameters::VstParamDefBuilder<T>::add() const
{
  return fParameters->add(*this);
}

//------------------------------------------------------------------------
// Parameters::add (called by the builder)
//------------------------------------------------------------------------
template<typename T>
VstParam<T> Parameters::add(VstParamDefBuilder<T> const &iBuilder)
{
  auto param = std::make_shared<VstParamDef<T>>(iBuilder.fParamID,
                                                iBuilder.fTitle,
                                                iBuilder.fUnits,
                                                iBuilder.fDefaultValue,
                                                iBuilder.fFlags,
                                                iBuilder.fUnitID,
                                                iBuilder.fShortTitle,
                                                iBuilder.fPrecision,
                                                iBuilder.fOwner,
                                                iBuilder.fTransient,
                                                iBuilder.fConverter);

  if(!iBuilder.fTransient && !iBuilder.fConverter)
  {
    DLOG_F(WARNING, "No converter defined for non transient parameter [%d]", iBuilder.fParamID);
  }

  if(addVstParamDef(param) == kResultOk)
    return param;
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Parameters::add (called by the builder)
//------------------------------------------------------------------------
template<typename T>
JmbParam<T> Parameters::add(Parameters::JmbParamDefBuilder<T> const &iBuilder)
{
  auto param = std::make_shared<JmbParamDef<T>>(iBuilder.fParamID,
                                                iBuilder.fTitle,
                                                iBuilder.fOwner,
                                                iBuilder.fTransient,
                                                iBuilder.fShared,
                                                iBuilder.fDefaultValue,
                                                iBuilder.fSerializer);

  if(!iBuilder.fTransient && !iBuilder.fSerializer)
  {
    DLOG_F(WARNING, "No serializer defined for parameter [%d] (won't be able to be saved)", iBuilder.fParamID);
  }

  if(iBuilder.fShared && !iBuilder.fSerializer)
  {
    DLOG_F(WARNING, "No serializer defined for parameter [%d] (won't be able to be shared with peer)", iBuilder.fParamID);
  }

  if(addJmbParamDef(param) == kResultOk)
    return param;
  else
    return nullptr;
}


//------------------------------------------------------------------------
// Parameters::vstFromType
//------------------------------------------------------------------------
template<typename T>
Parameters::VstParamDefBuilder<T> Parameters::vstFromType(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::VstParamDefBuilder<T>(this, iParamID, iTitle);
}

//------------------------------------------------------------------------
// Parameters::vst
//------------------------------------------------------------------------
template<typename ParamConverter, typename... Args>
Parameters::VstParamDefBuilder<typename ParamConverter::ParamType> Parameters::vst(ParamID iParamID,
                                                                                   const TChar *iTitle,
                                                                                   Args... iConverterArgs)
{
  auto builder = vstFromType<typename ParamConverter::ParamType>(iParamID, iTitle);
  builder.template converter<ParamConverter>(iConverterArgs...);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::jmbFromType
//------------------------------------------------------------------------
template<typename T>
Parameters::JmbParamDefBuilder<T> Parameters::jmbFromType(ParamID iParamID, const TChar *iTitle)
{
  return Parameters::JmbParamDefBuilder<T>(this, iParamID, iTitle);
}

//------------------------------------------------------------------------
// Parameters::jmb
//------------------------------------------------------------------------
template<typename ParamSerializer, typename... Args>
Parameters::JmbParamDefBuilder<typename ParamSerializer::ParamType> Parameters::jmb(ParamID iParamID,
                                                                                    const TChar *iTitle,
                                                                                    Args... iSerializerArgs)
{
  auto builder = jmbFromType<typename ParamSerializer::ParamType>(iParamID, iTitle);
  builder.template serializer<ParamSerializer>(iSerializerArgs...);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::buildParamIDs
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID iParamID, Args... args)
{
  tresult res = kResultOk;

  if(fVstParams.find(iParamID) != fVstParams.cend() ||
     fJmbParams.find(iParamID) != fJmbParams.cend())
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

}
}

#endif // __PONGASOFT_VST_PARAMETERS_H__