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
class RTRawParameter
{
public:
  // Constructor
  explicit RTRawParameter(std::shared_ptr<RawParamDef> iParamDef) :
    fRawParamDef{iParamDef},
    fNormalizedValue{fRawParamDef->fDefaultNormalizedValue},
    fPreviousNormalizedValue{fNormalizedValue}
  {}

  // getParamID
  ParamID getParamID() const { return fRawParamDef->fParamID; }

  // getRawParamDef
  std::shared_ptr<RawParamDef> getRawParamDef() const { return fRawParamDef; }

  /**
   * Update the parameter with a new normalized value. This is typically called after the VST parameter managed
   * by the VST sdk changes (for example, moving a knob or loading a previously saved plugin)
   *
   * @return true if the value was actually update, false if it is the same
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
  std::shared_ptr<RawParamDef> fRawParamDef;
  ParamValue fNormalizedValue;
  ParamValue fPreviousNormalizedValue;
};

/**
 * The typed version. Maintains the denormalized (aka "typed") version of the value and previous value.
 *
 * @tparam ParamConverter the converter (see ParamConverters.h for an explanation of what is expected)
 */
template<typename ParamConverter>
class RTParameter : public RTRawParameter
{
public:
  using ParamType = typename ParamConverter::ParamType;

  // Constructor
  explicit RTParameter(ParamDefSPtr<ParamConverter> iParamDef) :
    RTRawParameter(iParamDef),
    fValue{denormalize(fNormalizedValue)},
    fPreviousValue{fValue}
  {
  }

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const { return ParamConverter::normalize(iValue); }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const { return ParamConverter::denormalize(iNormalizedValue); }

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
template<typename ParamConverter>
bool RTParameter<ParamConverter>::updateNormalizedValue(ParamValue iNormalizedValue)
{
  if(RTRawParameter::updateNormalizedValue(iNormalizedValue))
  {
    fValue = denormalize(iNormalizedValue);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// RTParameter::resetPreviousValue
//------------------------------------------------------------------------
template<typename ParamConverter>
bool RTParameter<ParamConverter>::resetPreviousValue()
{
  if(RTRawParameter::resetPreviousValue())
  {
    fPreviousValue = fValue;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// RTParameter::update
//------------------------------------------------------------------------
template<typename ParamConverter>
void RTParameter<ParamConverter>::update(const ParamType &iNewValue)
{
  fValue = iNewValue;
  fNormalizedValue = normalize(fValue);
}

//------------------------------------------------------------------------
// RTParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of iterators which helps in writing simpler and natural code (the param
 * behaves like ParamConverter::ParamType in many ways).
 *
 * @tparam ParamConverter the converter (see ParamConverters.h for an explanation of what is expected)
 */
template<typename ParamConverter>
class RTParam
{
  using ParamType = typename ParamConverter::ParamType;

public:
  RTParam(std::shared_ptr<RTParameter<ParamConverter>> iPtr) :
    fPtr{std::move(iPtr)}
  {}

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const { return ParamConverter::normalize(iValue); }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const { return ParamConverter::denormalize(iNormalizedValue); }

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
  std::shared_ptr<RTParameter<ParamConverter>> fPtr;
};

}
}
}

#endif // __PONGASOFT_VST_RT_PARAMETER_H__