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
 * @tparam T the underlying type of the param */
template<typename T>
class VstParamDef : public RawVstParamDef
{
public:
  using ParamType = T;

  VstParamDef(ParamID const iParamID,
              TChar const *const iTitle,
              TChar const *const iUnits,
              ParamType const iDefaultValue,
              int32 const iStepCount,
              int32 const iFlags,
              UnitID const iUnitID,
              TChar const *const iShortTitle,
              int32 const iPrecision,
              bool const iUIOnly,
              bool const iTransient,
              std::shared_ptr<IParamConverter<ParamType>> iConverter) :
    RawVstParamDef(iParamID,
                   iTitle,
                   iUnits,
                   iConverter ? iConverter->normalize(iDefaultValue) : 0,
                   iStepCount,
                   iFlags,
                   iUnitID,
                   iShortTitle,
                   iPrecision,
                   iUIOnly,
                   iTransient),
    fDefaultValue{iDefaultValue},
    fConverter{std::move(iConverter)}
  {
  }

  // getDefaultValue
  ParamType getDefaultValue() const { return fDefaultValue; }

  // shortcut to normalize
  inline ParamValue normalize(ParamType const &iValue) const
  {
    if(fConverter)
      return fConverter->normalize(iValue);
    return 0;
  }

  // shortcut to denormalize
  inline ParamType denormalize(ParamValue iNormalizedValue) const
  {
    if(fConverter)
      return fConverter->denormalize(iNormalizedValue);
    return fDefaultValue;
  }

  /**
   * Using ParamConverter::toString
   */
  void toString(ParamValue iNormalizedValue, String128 iString) const override
  {
    if(fConverter)
      fConverter->toString(fConverter->denormalize(iNormalizedValue), iString, fPrecision);
  }

public:
  const ParamType fDefaultValue;
  const std::shared_ptr<IParamConverter<ParamType>> fConverter;
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

  virtual ~ISerParamDef() = default;
};

/**
 * Base class for all non vst parameters (need to provide serialization/deserialization)
 *
 * @tparam T the underlying type of the param */
template<typename T>
class SerParamDef : public ISerParamDef
{
public:
  using ParamType = T;

  SerParamDef(ParamID const iParamID,
              TChar const *const iTitle,
              bool const iUIOnly,
              bool const iTransient,
              ParamType const &iDefaultValue,
              std::shared_ptr<IParamSerializer<ParamType>> iSerializer) :
    ISerParamDef(iParamID, iTitle, iUIOnly, iTransient),
    fDefaultValue{iDefaultValue},
    fSerializer{std::move(iSerializer)}
  {}

  // readFromStream
  ParamType readFromStream(IBStreamer &iStreamer) const;

  // writeToStream
  tresult writeToStream(ParamType const &iValue, IBStreamer &oStreamer) const;

public:
  const ParamType fDefaultValue;
  const std::shared_ptr<IParamSerializer<ParamType>> fSerializer;
};

//------------------------------------------------------------------------
// SerParamDef::readFromStream
//------------------------------------------------------------------------
template<typename T>
T SerParamDef<T>::readFromStream(IBStreamer &iStreamer) const
{
  if(!fTransient && fSerializer)
  {
    return fSerializer->readFromStream(iStreamer, fDefaultValue);
  }
  else
    return fDefaultValue;
}

//------------------------------------------------------------------------
// SerParamDef::writeToStream
//------------------------------------------------------------------------
template<typename T>
tresult SerParamDef<T>::writeToStream(const T &iValue, IBStreamer &oStreamer) const
{
  if(fSerializer)
    return fSerializer->writeToStream(iValue, oStreamer);
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// VstParam - define shortcut notation
//------------------------------------------------------------------------
template<typename T>
using VstParam = std::shared_ptr<VstParamDef<T>>;

//------------------------------------------------------------------------
// SerParam - define shortcut notation
//------------------------------------------------------------------------
template<typename T>
using SerParam = std::shared_ptr<SerParamDef<T>>;

}
}

#endif // __PONGASOFT_VST_PARAM_DEF_H__