#ifndef __PONGASOFT_VST_PARAM_DEF_H__
#define __PONGASOFT_VST_PARAM_DEF_H__

#include "ParamConverters.h"

#include <base/source/fstreamer.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstunits.h>

#include <memory>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * Base class for a raw parameter definition
 */
class RawParamDef
{
public:
  RawParamDef(ParamID const iParamID,
              TChar const *const iTitle,
              TChar const *const iUnits,
              ParamValue const iDefaultNormalizedValue,
              int32 const iStepCount,
              int32 const iFlags,
              UnitID const iUnitID,
              TChar const *const iShortTitle,
              int32 const iPrecision,
              bool const iUIOnly,
              bool const iTransient) :
    fParamID{iParamID},
    fTitle{iTitle},
    fUnits{iUnits},
    fDefaultNormalizedValue{iDefaultNormalizedValue},
    fStepCount{iStepCount},
    fFlags{iFlags},
    fUnitID{iUnitID},
    fShortTitle{iShortTitle},
    fPrecision{iPrecision},
    fUIOnly{iUIOnly},
    fTransient{iTransient}
  {}

public:
  ParamValue readNormalizedValue(IBStreamer &iStreamer) const
  {
    if(!fTransient)
    {
      double value;
      if(!iStreamer.readDouble(value))
        value = fDefaultNormalizedValue;
      return value;
    }
    else
      return fDefaultNormalizedValue;
  }

  virtual void toString(ParamValue iNormalizedValue, String128 iString) const = 0;

public:
  const ParamID fParamID;
  const TChar *const fTitle;
  const TChar *const fUnits;
  const ParamValue fDefaultNormalizedValue;
  const int32 fStepCount;
  const int32 fFlags;
  const UnitID fUnitID;
  const TChar *const fShortTitle;
  const int32 fPrecision;
  const bool fUIOnly; // not used in RT (saved in the UI stream)
  const bool fTransient; // not saved in the stream
};

/**
 * Typed parameter definition.
 * @tparam ParamConverter the converter (see ParamConverters.h for an explanation of what is expected)
 */
template<typename ParamConverter>
class ParamDef : public RawParamDef
{
public:
  using ParamType = typename ParamConverter::ParamType;

  ParamDef(ParamID const iParamID,
           TChar const *const iTitle,
           TChar const *const iUnits,
           ParamValue const iDefaultNormalizedValue,
           int32 const iStepCount,
           int32 const iFlags,
           UnitID const iUnitID,
           TChar const *const iShortTitle,
           int32 const iPrecision,
           bool const iUIOnly,
           bool const iTransient) :
    RawParamDef(iParamID,
                iTitle,
                iUnits,
                iDefaultNormalizedValue,
                iStepCount,
                iFlags,
                iUnitID,
                iShortTitle,
                iPrecision,
                iUIOnly,
                iTransient)
  {
  }

  // getDefaultValue
  ParamType getDefaultValue() const { return denormalize(fDefaultNormalizedValue); }

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const { return ParamConverter::normalize(iValue); }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const { return ParamConverter::denormalize(iNormalizedValue); }

  /**
   * Using ParamConverter::toString
   */
  void toString(ParamValue iNormalizedValue, String128 iString) const override
  {
    ParamConverter::toString(ParamConverter::denormalize(iNormalizedValue), iString, fPrecision);
  }
};

//------------------------------------------------------------------------
// ParamDefSPtr - define shortcut notation
//------------------------------------------------------------------------
template<typename ParamConverter>
using ParamDefSPtr = std::shared_ptr<ParamDef<ParamConverter>>;

}
}

#endif // __PONGASOFT_VST_PARAM_DEF_H__