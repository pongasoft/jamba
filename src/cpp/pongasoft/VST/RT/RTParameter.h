#include <pluginterfaces/vst/ivstaudioprocessor.h>

#ifndef __PONGASOFT_VST_RT_PARAMETER_H__
#define __PONGASOFT_VST_RT_PARAMETER_H__

#include <pongasoft/VST/ParamDef.h>
#include <pongasoft/logging/logging.h>

namespace pongasoft {
namespace VST {
namespace RT {

/**
 * Base class which deals with the "raw"/untyped parameter and keep the normalized value (ParamValue in the
 * range [0.0,1.0]). Also keeps the "previous" value which is the value the param had in the previous frame/call to
 * process.
 */
class RTRawVstParameter
{
public:
  // Constructor
  explicit RTRawVstParameter(std::shared_ptr<RawVstParamDef> iParamDef) :
    fParamDef{iParamDef},
    fNormalizedValue{fParamDef->fDefaultValue},
    fPreviousNormalizedValue{fNormalizedValue}
  {}

  // getParamID
  ParamID getParamID() const { return fParamDef->fParamID; }

  // getParamDef
  std::shared_ptr<RawVstParamDef> getParamDef() const { return fParamDef; }

  /**
   * Update the parameter with a new normalized value. This is typically called after the VST parameter managed
   * by the VST sdk changes (for example, moving a knob or loading a previously saved plugin)
   *
   * @return true if the value was actually updated, false if it is the same
   */
  virtual bool updateNormalizedValue(ParamValue iNormalizedValue);

  // getNormalizedValue
  inline ParamValue const &getNormalizedValue() { return fNormalizedValue; }

  // getPreviousNormalizedValue
  inline ParamValue const &getPreviousNormalizedValue() { return fPreviousNormalizedValue; }

  /**
   * Add the current normalized value as an output parameter changes
   */
  tresult addToOutput(ProcessData &oData);

  /**
   * @return true if the parameter has changed within the frame (previous and current are different)
   */
  inline bool hasChanged() const { return fNormalizedValue != fPreviousNormalizedValue; }

  /**
   * Called at the end of the frame so that previous value is set to current value for the next frame
   * @return true if previous value was different than current value
   */
  virtual bool resetPreviousValue();

protected:
  std::shared_ptr<RawVstParamDef> fParamDef;
  ParamValue fNormalizedValue;
  ParamValue fPreviousNormalizedValue;
};

/**
 * The typed version. Maintains the denormalized (aka "typed") version of the value and previous value.
 *
 * @tparam T the underlying type of the param */
template<typename T>
class RTVstParameter : public RTRawVstParameter
{
public:
  using ParamType = T;

  // Constructor
  explicit RTVstParameter(VstParam<T> iParamDef) :
    RTRawVstParameter(iParamDef),
    fValue{denormalize(fNormalizedValue)},
    fPreviousValue{fValue}
  {
  }

  // getParamDef (correct type for this subclass)
  VstParam<T> getVstParamDef() const
  {
    return std::dynamic_pointer_cast<VstParamDef<T>>(getParamDef());
  }

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const { return getVstParamDef()->normalize(iValue); }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const { return getVstParamDef()->denormalize(iNormalizedValue); }

  /**
   * This method is typically called during the processing method when the plugin needs to update the value. In general
   * the change needs to be propagated to the VST sdk (using addToOutput).
   */
  void update(ParamType const &iNewValue);

  // getValue
  inline ParamType const &getValue() const { return fValue; }

  // getPreviousValue
  inline ParamType const &getPreviousValue() const { return fPreviousValue; }

protected:
  // Override the base class to update the denormalized value as well
  bool updateNormalizedValue(ParamValue iNormalizedValue) override;

  // Override the base class to update the denormalized value as well
  bool resetPreviousValue() override;

protected:
  ParamType fValue;
  ParamType fPreviousValue;
};

//------------------------------------------------------------------------
// RTParameter::updateNormalizedValue - update fValue to the new value and return true if it changed
//------------------------------------------------------------------------
template<typename T>
bool RTVstParameter<T>::updateNormalizedValue(ParamValue iNormalizedValue)
{
  if(RTRawVstParameter::updateNormalizedValue(iNormalizedValue))
  {
    fValue = denormalize(iNormalizedValue);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// RTParameter::resetPreviousValue
//------------------------------------------------------------------------
template<typename T>
bool RTVstParameter<T>::resetPreviousValue()
{
  if(RTRawVstParameter::resetPreviousValue())
  {
    fPreviousValue = fValue;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// RTParameter::update
//------------------------------------------------------------------------
template<typename T>
void RTVstParameter<T>::update(const ParamType &iNewValue)
{
  fValue = iNewValue;
  fNormalizedValue = normalize(fValue);
}

//------------------------------------------------------------------------
// RTVstParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of iterators which helps in writing simpler and natural code (the param
 * behaves like T in many ways).
 *
 * @tparam T the underlying type of the param */
template<typename T>
class RTVstParam
{
  using ParamType = T;

public:
  RTVstParam(std::shared_ptr<RTVstParameter<T>> iPtr) :
    fPtr{std::move(iPtr)}
  {}

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const { return fPtr->normalize(iValue); }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const { return fPtr->denormalize(iNormalizedValue); }

  /**
   * This method is typically called during the processing method when the plugin needs to update the value. In general
   * the change needs to be propagated to the VST sdk (using addToOutput). Use this version of the call if you want to
   * control when the update actually happens.
   */
  inline void update(ParamType const &iNewValue) { fPtr->update(iNewValue); }

  /**
   * This method is typically called during the processing method when the plugin needs to update the value.
   * This version will automatically propagate the change to the the VST sdk.
   */
  inline void update(ParamType const &iNewValue, ProcessData &oData)
  {
    update(iNewValue);
    addToOutput(oData);
  }

  /**
   * @return true if the parameter has changed within the frame (previous and current are different)
   */
  inline bool hasChanged() const { return fPtr->hasChanged(); }

  /**
   * Add the current normalized value as an output parameter changes which propagates the change to the vst sdk
   */
  inline tresult addToOutput(ProcessData &oData) { return fPtr->addToOutput(oData); }

  // allow to use the param as the underlying ParamType (ex: "if(param)" in the case ParamType is bool))
  inline operator ParamType const &() const { return fPtr->getValue(); }

  // allow writing param->xxx to access the underlying type directly (if not a primitive)
  inline ParamType const *operator->() const { return &fPtr->getValue(); }

  // getPreviousValue
  inline ParamType const &previous() const { return fPtr->getPreviousValue(); }

private:
  std::shared_ptr<RTVstParameter<T>> fPtr;
};

}
}
}

#endif // __PONGASOFT_VST_RT_PARAMETER_H__