#ifndef __PONGASOFT_VST_PARAM_DEF_H__
#define __PONGASOFT_VST_PARAM_DEF_H__

#include "ParamConverters.h"
#include "ParamSerializers.h"

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
 * Base class for all ParamDef
 */
class SerializableParamDef
{
public:
  SerializableParamDef(ParamID const iParamID,
                       TChar const *const iTitle,
                       bool const iUIOnly,
                       bool const iTransient) :
    fParamID{iParamID},
    fTitle{iTitle},
    fUIOnly{iUIOnly},
    fTransient{iTransient}
  {}

public:
  const ParamID fParamID;
  const TChar *const fTitle;
  const bool fUIOnly; // not used in RT (saved in the UI stream)
  const bool fTransient; // not saved in the stream
};

/**
 * Base class for all parameter defs (provide serialization/deserialization)
 *
 * @tparam ParamSerializer the serializer (see ParamSerializers.h for an explanation of what is expected) */
template<typename ParamSerializer>
class AnyParamDef : public SerializableParamDef
{
public:
  using SerializableParamType = typename ParamSerializer::ParamType;

  AnyParamDef(ParamID const iParamID,
              TChar const *const iTitle,
              bool const iUIOnly,
              bool const iTransient,
              SerializableParamType const &iDefaultValue) :
    SerializableParamDef(iParamID, iTitle, iUIOnly, iTransient),
    fDefaultValue{iDefaultValue}
  {}

  // readFromStream
  SerializableParamType readFromStream(IBStreamer &iStreamer) const;

  // writeToStream
  tresult writeToStream(SerializableParamType const &iValue, IBStreamer &oStreamer) const;

public:
  const SerializableParamType fDefaultValue;
};

/**
 * Base class for a raw vst parameter definition
 */
class RawParamDef : public AnyParamDef<RawParamSerializer>
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
    AnyParamDef(iParamID, iTitle, iUIOnly, iTransient, iDefaultNormalizedValue),
    fUnits{iUnits},
    fStepCount{iStepCount},
    fFlags{iFlags},
    fUnitID{iUnitID},
    fShortTitle{iShortTitle},
    fPrecision{iPrecision}
  {}

public:
  virtual void toString(ParamValue iNormalizedValue, String128 iString) const = 0;

public:
  const TChar *const fUnits;
  const int32 fStepCount;
  const int32 fFlags;
  const UnitID fUnitID;
  const TChar *const fShortTitle;
  const int32 fPrecision;
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
  ParamType getDefaultValue() const { return denormalize(fDefaultValue); }

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
// AnyParamDef::readFromStream
//------------------------------------------------------------------------
template<typename ParamSerializer>
typename ParamSerializer::ParamType AnyParamDef<ParamSerializer>::readFromStream(IBStreamer &iStreamer) const
{
  if(!fTransient)
  {
    return ParamSerializer::readFromStream(iStreamer, fDefaultValue);
  }
  else
    return fDefaultValue;
}

//------------------------------------------------------------------------
// AnyParamDef::writeToStream
//------------------------------------------------------------------------
template<typename ParamSerializer>
tresult AnyParamDef<ParamSerializer>::writeToStream(const typename ParamSerializer::ParamType &iValue, IBStreamer &oStreamer) const
{
  return ParamSerializer::writeToStream(iValue, oStreamer);
}

//------------------------------------------------------------------------
// ParamDefSPtr - define shortcut notation
//------------------------------------------------------------------------
template<typename ParamConverter>
using ParamDefSPtr = std::shared_ptr<ParamDef<ParamConverter>>;

//------------------------------------------------------------------------
// AnyParamDefSPtr - define shortcut notation
//------------------------------------------------------------------------
template<typename ParamSerializer>
using AnyParamDefSPtr = std::shared_ptr<AnyParamDef<ParamSerializer>>;

}
}

#endif // __PONGASOFT_VST_PARAM_DEF_H__