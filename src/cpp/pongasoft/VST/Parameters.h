/*
 * Copyright (c) 2018-2020 pongasoft
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
#pragma once

#include "ParamDef.h"
#include "NormalizedState.h"

#include <map>
#include <vector>
#include <set>
#include <functional>

#include <public.sdk/source/vst/vstparameters.h>

namespace pongasoft::VST {

namespace Debug { class ParamDisplay; }

/**
 * This is the class which maintains all the registered parameters
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
    RawVstParamDefBuilder &units(VstString16 iUnits) { fUnits = std::move(iUnits); return *this; }
    RawVstParamDefBuilder &defaultValue(ParamValue iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    RawVstParamDefBuilder &stepCount(int32 iStepCount) { fStepCount = iStepCount; return *this;}
    RawVstParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    RawVstParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    RawVstParamDefBuilder &shortTitle(VstString16 iShortTitle) { fShortTitle = std::move(iShortTitle); return *this; }
    RawVstParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    RawVstParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    RawVstParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    RawVstParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }
    RawVstParamDefBuilder &deprecatedSince(int16 iVersion) { fDeprecatedSince = iVersion; return *this; }

    // parameter factory method
    RawVstParam add() const;

    // fields
    ParamID fParamID;
    VstString16 fTitle;
    VstString16 fUnits{};
    ParamValue fDefaultValue{};
    int32 fStepCount{0};
    int32 fFlags{ParameterInfo::kCanAutomate};
    UnitID fUnitID{kRootUnitId};
    VstString16 fShortTitle{};
    int32 fPrecision{4};
    IParamDef::Owner fOwner{IParamDef::Owner::kRT};
    bool fTransient{false};
    int16 fDeprecatedSince{IParamDef::kVersionNotDeprecated};

    friend class Parameters;

  protected:
    RawVstParamDefBuilder(Parameters *iParameters, ParamID iParamID, VstString16 iTitle) :
      fParamID{iParamID}, fTitle{std::move(iTitle)}, fParameters{iParameters} {}

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
    VstParamDefBuilder &units(VstString16 iUnits) { fUnits = std::move(iUnits); return *this; }
    VstParamDefBuilder &defaultValue(T const &iDefaultValue) { fDefaultValue = iDefaultValue; return *this;}
    VstParamDefBuilder &flags(int32 iFlags) { fFlags = iFlags; return *this; }
    VstParamDefBuilder &unitID(int32 iUnitID) { fUnitID = iUnitID; return *this; }
    VstParamDefBuilder &shortTitle(VstString16 iShortTitle) { fShortTitle = std::move(iShortTitle); return *this; }
    VstParamDefBuilder &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
    VstParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    VstParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    VstParamDefBuilder &transient(bool iTransient = true) { fTransient = iTransient; return *this; }
    VstParamDefBuilder &deprecatedSince(int16 iVersion) { fDeprecatedSince = iVersion; return *this; }
    VstParamDefBuilder &converter(std::shared_ptr<IParamConverter<T>> iConverter) { fConverter = std::move(iConverter); return *this; }
    template<typename ParamConverter, typename... Args>
    VstParamDefBuilder &converter(Args&& ...iArgs) { fConverter = std::make_shared<ParamConverter>(std::forward<Args>(iArgs)...); return *this; }

    // parameter factory method
    VstParam<T> add() const;

    // fields
    ParamID fParamID;
    VstString16 fTitle;
    VstString16 fUnits{};
    T fDefaultValue{};
    int32 fFlags{ParameterInfo::kCanAutomate};
    UnitID fUnitID{kRootUnitId};
    VstString16 fShortTitle{};
    int32 fPrecision{4};
    IParamDef::Owner fOwner{IParamDef::Owner::kRT};
    bool fTransient{false};
    int16 fDeprecatedSince{IParamDef::kVersionNotDeprecated};
    std::shared_ptr<IParamConverter<T>> fConverter{};

    friend class Parameters;

  protected:
    VstParamDefBuilder(Parameters *iParameters, ParamID iParamID, VstString16 iTitle) :
      fParamID{iParamID}, fTitle{std::move(iTitle)}, fParameters{iParameters} {}

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
    JmbParamDefBuilder &deprecatedSince(int16 iVersion) { fDeprecatedSince = iVersion; return *this; }
    JmbParamDefBuilder &rtOwned() { fOwner = IParamDef::Owner::kRT; return *this; }
    JmbParamDefBuilder &guiOwned() { fOwner = IParamDef::Owner::kGUI; return *this; }
    JmbParamDefBuilder &shared(bool iShared = true) { fShared = iShared; return *this; }
    JmbParamDefBuilder &serializer(std::shared_ptr<IParamSerializer<T>> iSerializer) { fSerializer = std::move(iSerializer); return *this; }
    template<typename ParamSerializer, typename... Args>
    JmbParamDefBuilder &serializer(Args&& ...iArgs) { fSerializer = std::make_shared<ParamSerializer>(std::forward<Args>(iArgs)...); return *this; }

    // parameter factory method
    JmbParam<T> add() const;

    // fields
    ParamID fParamID;
    VstString16 fTitle;
    T fDefaultValue{};
    IParamDef::Owner fOwner{IParamDef::Owner::kGUI};
    bool fTransient{false};
    int16 fDeprecatedSince{IParamDef::kVersionNotDeprecated};
    bool fShared{false};
    std::shared_ptr<IParamSerializer<T>> fSerializer{};

    friend class Parameters;

  protected:
    JmbParamDefBuilder(Parameters *iParameters, ParamID iParamID, VstString16 iTitle) :
      fParamID{iParamID}, fTitle{std::move(iTitle)}, fParameters{iParameters} {}

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
  RawVstParamDefBuilder raw(ParamID iParamID, VstString16 iTitle);

  /**
   * Used from derived classes to build a parameter backed by a VST parameter
   */
  template<typename ParamConverter, typename... Args>
  VstParamDefBuilder<typename ParamConverter::ParamType> vst(ParamID iParamID, VstString16 iTitle, Args&& ...iConverterArgs);

  /**
   * This flavor allows the `ParamConverter` to provide an actual type for the primary constructor
   */
  template<typename ParamConverter>
  VstParamDefBuilder<typename ParamConverter::ParamType> vst(int32 iParamID,
                                                             VstString16 iTitle,
                                                             typename ParamConverter::ConstructorType initValue);

  /**
   * Used from derived classes to build a parameter backed by a VST parameter. Use this version
   * if you want to provide a different converter.
   */
  template<typename T>
  VstParamDefBuilder<T> vstFromType(ParamID iParamID, VstString16 iTitle);

  /**
   * Used from derived classes to build a non vst parameter (not convertible to a ParamValue)
   */
  template<typename ParamSerializer, typename... Args>
  JmbParamDefBuilder<typename ParamSerializer::ParamType> jmb(ParamID iParamID, VstString16 iTitle, Args&& ...iSerializerArgs);

  /**
   * This flavor allows the `ParamSerializer` to provide an actual type for the primary constructor
   */
  template<typename ParamSerializer>
  JmbParamDefBuilder<typename ParamSerializer::ParamType> jmb(int32 iParamID,
                                                              VstString16 iTitle,
                                                              typename ParamSerializer::ConstructorType initValue);

  /**
   * Used from derived classes to build a non vst parameter (not convertible to a ParamValue). Use this version
   * if you want to provide a different serializer.
   */
  template<typename T>
  JmbParamDefBuilder<T> jmbFromType(ParamID iParamID, VstString16 iTitle);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @param args can be any combination of `ParamID`, RawVstParamDef, VstParamDef
   */
  template<typename... Args>
  tresult setRTSaveStateOrder(int16 iVersion, Args&& ...args);

  /**
   * Used to change the default order (registration order) used when saving the RT state (getState/setState in the
   * processor, setComponentState in the controller)
   */
  tresult setRTSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder);

  /**
   * This method should be called to save the order of a deprecated version so that it can be handled during upgrade. A
   * deprecated version should be used when the state changes in a non compatible fashion.
   *
   * For example, adding new parameters at the end of the state is **not** an incompatible change and does not require
   * to deprecate the version.
   *
   * On the other end, adding new parameters in the middle and removing parameters are incompatible changes and
   * require to deprecate the version.
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @param args can be any combination of `ParamID`, RawVstParamDef, VstParamDef
   */
  template<typename... Args>
  tresult setRTDeprecatedSaveStateOrder(int16 iVersion, Args&& ...args);

  /**
   * @return the order used when saving the GUI state (getState/setState in the controller)
   */
  NormalizedState::SaveOrder const &getGUISaveStateOrder() const { return fGUISaveStateOrder; }

  /**
   * Used to change the default order (registration order) used when saving the GUI state (getState/setState in
   * the controller)
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @param args can be any combination of `ParamID`, RawVstParamDef, VstParamDef, JmbParamDef
   */
  template<typename... Args>
  tresult setGUISaveStateOrder(int16 iVersion, Args&& ...args);

  /**
   * @return the order used when saving the GUI state (getState/setState in the controller) for the given (deprecated)
   *         version or `nullptr` if there is no such version
   */
  NormalizedState::SaveOrder const *getGUIDeprecatedSaveStateOrder(int16 iVersion) const;

  /**
   * This method should be called to save the order of a deprecated version so that it can be handled during upgrade. A
   * deprecated version should be used when the state changes in a non compatible fashion.
   *
   * For example, adding new parameters at the end of the state is **not** an incompatible change and does not require
   * to deprecate the version.
   *
   * On the other end, adding new parameters in the middle and removing parameters are incompatible changes and
   * require to deprecate the version.
   *
   * @param iVersion should be a >= 0 number. If negative it will be ignored
   * @param args can be any combination of `ParamID`, RawVstParamDef, VstParamDef, JmbParamDef
   */
  template<typename... Args>
  tresult setGUIDeprecatedSaveStateOrder(int16 iVersion, Args&& ...args);

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
   * @return a new normalized state for RT (can be overridden to return a subclass!)
   */
  virtual std::unique_ptr<NormalizedState> newRTState(NormalizedState::SaveOrder const *iSaveOrder) const;

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

  /**
   * When Jamba detects that a previously saved RT state matches a deprecated version (as registered with
   * `setRTDeprecatedSaveStateOrder`), it will call this method to let the plugin handle the upgrade if necessary.
   *
   * Here is an example of a typical implementation of this method:
   *
   * ```
   * switch(iDeprecatedState.getVersion())
   * {
   *    case kRT_V1: // assuming this is the previous version
   *    {
   *      // __deprecated_fNumSlices is a deprecated param (removed from deprecated state) so we read
   *      // its value using a helper in the deprecated state
   *      auto oldNumSlices = __deprecated_fNumSlices->readFromState(iDeprecatedState);
   *
   *      // fNumSlices is a new parameter (added to the new state) which handles number of slices completely
   *      // differently so we save the new value in the new state using a helper
   *      fNumSlices->writeToState(NumSlice{oldNumSlices}, oNewState);
   *
   *      return kResultTrue;
   *    }
   *
   *    default:
   *      DLOG_F(ERROR, "Unexpected deprecated version %d", iDeprecatedState.fSaveOrder->fVersion);
   *      return kResultFalse;
   * }
   * ```
   *
   * \note Since `oNewState` has already been populated with the values from `iDeprecatedState` that had the same
   *       parameter ID, this method should be overridden only when further tweaks need to happen (like in the
   *       above example when a parameter value which was an enumeration, need to be converted to a float)
   *
   * @param iDeprecatedState the populated deprecated state from which to read the deprecated values
   * @param oNewState the new state already populated with the common values from `iDeprecatedState` for
   *                  further tweaks
   * @return `kResultTrue` if handled, `kResultFalse` if unhandled
   */
  virtual tresult handleRTStateUpgrade(NormalizedState const &iDeprecatedState, NormalizedState &oNewState) const
  {
    return kResultTrue;
  }

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

  /**
   * This method is called to read a deprecated (prior version) RTState from the stream */
  virtual tresult readDeprecatedRTState(uint16 iVersion, IBStreamer &iStreamer, NormalizedState *oNormalizedState) const;

private:
  // contains all the registered (raw type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<RawVstParamDef>> fVstParams{};

  // contains all the registered (serializable type) parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<IJmbParamDef>> fJmbParams{};

  // order in which the parameters will be registered in the vst world
  std::vector<ParamID> fVstRegistrationOrder{};

  // order in which the parameters were registered
  std::vector<ParamID> fAllRegistrationOrder{};

  // The "latest" order for both RT and GUI
  NormalizedState::SaveOrder fRTSaveStateOrder{};
  NormalizedState::SaveOrder fGUISaveStateOrder{};

  // Keep track of deprecated state orders used for upgrade
  std::map<int16, NormalizedState::SaveOrder> fRTDeprecatedSaveStateOrders{};
  std::map<int16, NormalizedState::SaveOrder> fGUIDeprecatedSaveStateOrders{};

private:
  // leaf of templated calls to build a list of ParamIDs from ParamID or ParamDefs
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs) { return kResultOk; }

  // case when ParamID
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID id, Args&& ...args);

  // case when IJmbParamDef
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<IJmbParamDef> &iParamDef, Args&& ...args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, std::forward<Args>(args)...);
  }

  // case when IJmbParamDef
  template<typename T, typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<JmbParamDef<T>> &iParamDef, Args&& ...args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, std::forward<Args>(args)...);
  }

  // case when VstParamDef
  template<typename ParamConverver, typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<VstParamDef<ParamConverver>> &iParamDef, Args&& ...args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, std::forward<Args>(args)...);
  }

  // case when RawVstParamDef
  template<typename... Args>
  tresult buildParamIDs(std::vector<ParamID> &iParamIDs, std::shared_ptr<RawVstParamDef> &iParamDef, Args&& ...args)
  {
    return buildParamIDs(iParamIDs, iParamDef->fParamID, std::forward<Args>(args)...);
  }

  /**
   * Called internally with the order for a deprecated version */
  tresult setRTDeprecatedSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder);

  /**
   * Called internally with the order for a deprecated version */
  tresult setGUIDeprecatedSaveStateOrder(NormalizedState::SaveOrder const &iSaveOrder);
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
                                                iBuilder.fDeprecatedSince,
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
                                                iBuilder.fDeprecatedSince,
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
Parameters::VstParamDefBuilder<T> Parameters::vstFromType(ParamID iParamID, VstString16 iTitle)
{
  return Parameters::VstParamDefBuilder<T>(this, iParamID, std::move(iTitle));
}

//------------------------------------------------------------------------
// Parameters::vst
//------------------------------------------------------------------------
template<typename ParamConverter, typename... Args>
Parameters::VstParamDefBuilder<typename ParamConverter::ParamType> Parameters::vst(ParamID iParamID,
                                                                                   VstString16 iTitle,
                                                                                   Args&& ...iConverterArgs)
{
  auto builder = vstFromType<typename ParamConverter::ParamType>(iParamID, std::move(iTitle));
  builder.template converter<ParamConverter>(std::forward<Args>(iConverterArgs)...);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::vst
//------------------------------------------------------------------------
template<typename ParamConverter>
Parameters::VstParamDefBuilder<typename ParamConverter::ParamType> Parameters::vst(int32 iParamID,
                                                                                   VstString16 iTitle,
                                                                                   typename ParamConverter::ConstructorType initValue) {
  auto builder = vstFromType<typename ParamConverter::ParamType>(iParamID, std::move(iTitle));
  builder.template converter<ParamConverter>(initValue);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::jmbFromType
//------------------------------------------------------------------------
template<typename T>
Parameters::JmbParamDefBuilder<T> Parameters::jmbFromType(ParamID iParamID, VstString16 iTitle)
{
  return Parameters::JmbParamDefBuilder<T>(this, iParamID, std::move(iTitle));
}

//------------------------------------------------------------------------
// Parameters::jmb
//------------------------------------------------------------------------
template<typename ParamSerializer, typename... Args>
Parameters::JmbParamDefBuilder<typename ParamSerializer::ParamType> Parameters::jmb(ParamID iParamID,
                                                                                    VstString16 iTitle,
                                                                                    Args&& ...iSerializerArgs)
{
  auto builder = jmbFromType<typename ParamSerializer::ParamType>(iParamID, std::move(iTitle));
  builder.template serializer<ParamSerializer>(std::forward<Args>(iSerializerArgs)...);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::jmb
//------------------------------------------------------------------------
template<typename ParamSerializer>
Parameters::JmbParamDefBuilder<typename ParamSerializer::ParamType> Parameters::jmb(int32 iParamID,
                                                                                    VstString16 iTitle,
                                                                                    typename ParamSerializer::ConstructorType initValue) {
  auto builder = jmbFromType<typename ParamSerializer::ParamType>(iParamID, std::move(iTitle));
  builder.template serializer<ParamSerializer>(initValue);
  return builder;
}

//------------------------------------------------------------------------
// Parameters::buildParamIDs
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::buildParamIDs(std::vector<ParamID> &iParamIDs, ParamID iParamID, Args&& ...args)
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
  res |= buildParamIDs(iParamIDs, std::forward<Args>(args)...);
  return res;
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setRTSaveStateOrder(int16 iVersion, Args&& ...args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, std::forward<Args>(args)...);
  return setRTSaveStateOrder({iVersion, ids});
}

//------------------------------------------------------------------------
// Parameters::setRTDeprecatedSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setRTDeprecatedSaveStateOrder(int16 iVersion, Args&& ...args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, std::forward<Args>(args)...);
  return setRTDeprecatedSaveStateOrder({iVersion, ids});
}

//------------------------------------------------------------------------
// Parameters::setRTSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setGUISaveStateOrder(int16 iVersion, Args&& ...args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, std::forward<Args>(args)...);
  return setGUISaveStateOrder({iVersion, ids});
}

//------------------------------------------------------------------------
// Parameters::setGUIDeprecatedSaveStateOrder
//------------------------------------------------------------------------
template<typename... Args>
tresult Parameters::setGUIDeprecatedSaveStateOrder(int16 iVersion, Args&& ...args)
{
  std::vector<ParamID> ids{};
  buildParamIDs(ids, std::forward<Args>(args)...);
  return setGUIDeprecatedSaveStateOrder({iVersion, ids});
}

}
