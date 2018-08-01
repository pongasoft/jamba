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
class IParamDef
{
public:
  IParamDef(ParamID const iParamID,
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
 * Base class for a raw vst parameter definition
 */
class RawVstParamDef : public IParamDef
{
public:
  RawVstParamDef(ParamID const iParamID,
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
    IParamDef(iParamID, iTitle, iUIOnly, iTransient),
    fUnits{iUnits},
    fDefaultValue{iDefaultNormalizedValue},
    fStepCount{iStepCount},
    fFlags{iFlags},
    fUnitID{iUnitID},
    fShortTitle{iShortTitle},
    fPrecision{iPrecision}
  {}

public:
  // readFromStream
  ParamValue readFromStream(IBStreamer &iStreamer) const
  {
    if(!fTransient)
    {
      return RawParamSerializer::readFromStream(iStreamer, fDefaultValue);
    }
    else
      return fDefaultValue;
  }

  virtual void toString(ParamValue iNormalizedValue, String128 iString) const = 0;

public:
  const TChar *const fUnits;
  const ParamValue fDefaultValue;
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
class VstParamDef : public RawVstParamDef
{
public:
  using ParamType = typename ParamConverter::ParamType;

  VstParamDef(ParamID const iParamID,
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
    RawVstParamDef(iParamID,
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

/**
 * Base class for serializable params (non templated)
 */
class ISerParamDef : public IParamDef
{
public:
  ISerParamDef(const ParamID iParamID, const TChar *const iTitle, const bool iUIOnly, const bool iTransient)
    : IParamDef(iParamID, iTitle, iUIOnly, iTransient)
  {}
};

/**
 * Base class for all non vst parameters (need to provide serialization/deserialization)
 *
 * @tparam ParamSerializer the serializer (see ParamSerializers.h for an explanation of what is expected) */
template<typename ParamSerializer>
class SerParamDef : public ISerParamDef
{
public:
  using SerializableParamType = typename ParamSerializer::ParamType;

  SerParamDef(ParamID const iParamID,
              TChar const *const iTitle,
              bool const iUIOnly,
              bool const iTransient,
              SerializableParamType const &iDefaultValue) :
    ISerParamDef(iParamID, iTitle, iUIOnly, iTransient),
    fDefaultValue{iDefaultValue}
  {}

  // readFromStream
  SerializableParamType readFromStream(IBStreamer &iStreamer) const;

  // writeToStream
  tresult writeToStream(SerializableParamType const &iValue, IBStreamer &oStreamer) const;

public:
  const SerializableParamType fDefaultValue;
};

//------------------------------------------------------------------------
// SerParamDef::readFromStream
//------------------------------------------------------------------------
template<typename ParamSerializer>
typename ParamSerializer::ParamType SerParamDef<ParamSerializer>::readFromStream(IBStreamer &iStreamer) const
{
  if(!fTransient)
  {
    return ParamSerializer::readFromStream(iStreamer, fDefaultValue);
  }
  else
    return fDefaultValue;
}

//------------------------------------------------------------------------
// SerParamDef::writeToStream
//------------------------------------------------------------------------
template<typename ParamSerializer>
tresult SerParamDef<ParamSerializer>::writeToStream(const typename ParamSerializer::ParamType &iValue, IBStreamer &oStreamer) const
{
  return ParamSerializer::writeToStream(iValue, oStreamer);
}

//------------------------------------------------------------------------
// VstParam - define shortcut notation
//------------------------------------------------------------------------
template<typename ParamConverter>
using VstParam = std::shared_ptr<VstParamDef<ParamConverter>>;

//------------------------------------------------------------------------
// SerParam - define shortcut notation
//------------------------------------------------------------------------
template<typename ParamSerializer>
using SerParam = std::shared_ptr<SerParamDef<ParamSerializer>>;

}
}

#endif // __PONGASOFT_VST_PARAM_DEF_H__