/*
 * Copyright (c) 2018-2019 pongasoft
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

#include "ParamConverters.h"
#include "ParamSerializers.h"
#include "Messaging.h"

#include <base/source/fstreamer.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstunits.h>

#include <string>
#include <memory>

namespace pongasoft::VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

// forward declaration required for API
namespace GUI::Params {
class IGUIJmbParameter;
}

/**
 * Base class for all ParamDef
 */
class IParamDef : public std::enable_shared_from_this<IParamDef>
{
public:
  enum class Owner
  {
    kRT,
    kGUI
  };
public:
  IParamDef(ParamID const iParamID,
            VstString16 iTitle,
            Owner const iOwner,
            bool const iTransient) :
    fParamID{iParamID},
    fTitle{std::move(iTitle)},
    fOwner{iOwner},
    fTransient{iTransient}
  {}

  virtual ~IParamDef() = default;

public:
  const ParamID fParamID;
  const VstString16 fTitle;
  const Owner fOwner; // who owns the parameter (and which stream will it be saved if non transient)
  const bool fTransient; // not saved in the stream
};


/**
 * Base class for a raw vst parameter definition
 */
class RawVstParamDef : public IParamDef
{
public:
  RawVstParamDef(ParamID const iParamID,
                 VstString16 iTitle,
                 VstString16 iUnits,
                 ParamValue const iDefaultNormalizedValue,
                 int32 const iStepCount,
                 int32 const iFlags,
                 UnitID const iUnitID,
                 VstString16 iShortTitle,
                 int32 const iPrecision,
                 Owner const iOwner,
                 bool const iTransient) :
    IParamDef(iParamID, std::move(iTitle), iOwner, iTransient),
    fUnits{std::move(iUnits)},
    fDefaultValue{Utils::clampE(iDefaultNormalizedValue, 0.0, 1.0)},
    fStepCount{iStepCount},
    fFlags{iFlags},
    fUnitID{iUnitID},
    fShortTitle{std::move(iShortTitle)},
    fPrecision{iPrecision}
  {}

public:
  // readFromStream
  ParamValue readFromStream(IBStreamer &iStreamer) const
  {
    ParamValue res = fDefaultValue;

    ParamValue value;
    if(IBStreamHelper::readDouble(iStreamer, value) == kResultOk)
      res = value;

    return res;
  }

  // toString
  virtual void toString(ParamValue iNormalizedValue, String128 iString) const
  {
    RawParamConverter::staticToString(iNormalizedValue, iString, fPrecision);
  }

  /**
   * Return the value as a utf-8 string
   *
   * @param iPrecision if < 0 uses `fPrecision` otherwise use the one provided
   */
  virtual std::string toUTF8String(ParamValue iNormalizedValue, int32 iPrecision) const
  {
    String128 s;
    s[0] = 0;
    RawParamConverter::staticToString(iNormalizedValue, s, iPrecision >= 0 ? iPrecision : fPrecision);
    return VstUtils::toUT8String(s);
  }

public:
  const VstString16 fUnits;
  const ParamValue fDefaultValue;
  const int32 fStepCount;
  const int32 fFlags;
  const UnitID fUnitID;
  const VstString16 fShortTitle;
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
              VstString16 iTitle,
              VstString16 iUnits,
              ParamType const iDefaultValue,
              int32 const iFlags,
              UnitID const iUnitID,
              VstString16 iShortTitle,
              int32 const iPrecision,
              Owner const iOwner,
              bool const iTransient,
              std::shared_ptr<IParamConverter<ParamType>> iConverter) :
    RawVstParamDef(iParamID,
                   std::move(iTitle),
                   std::move(iUnits),
                   iConverter ? iConverter->normalize(iDefaultValue) : 0,
                   iConverter ? iConverter->getStepCount() : 0,
                   iFlags,
                   iUnitID,
                   std::move(iShortTitle),
                   iPrecision,
                   iOwner,
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
   * Using fConverter::toString
   */
  void toString(ParamValue iNormalizedValue, String128 iString) const override
  {
    if(fConverter)
      fConverter->toString(fConverter->denormalize(iNormalizedValue), iString, fPrecision);
    else
      RawVstParamDef::toString(iNormalizedValue, iString);
  }

  /**
   * Return the value as a utf-8 string
   *
   * @param iPrecision if < 0 uses `fPrecision` otherwise use the one provided
   */
  std::string toUTF8String(ParamValue iNormalizedValue, int32 iPrecision) const override
  {
    if(fConverter)
    {
      String128 s;
      s[0] = 0;
      fConverter->toString(fConverter->denormalize(iNormalizedValue), s, iPrecision >= 0 ? iPrecision : fPrecision);
      return VstUtils::toUT8String(s);
    }
    else
      return RawVstParamDef::toUTF8String(iNormalizedValue, iPrecision);
  }

public:
  const ParamType fDefaultValue;
  const std::shared_ptr<IParamConverter<ParamType>> fConverter;
};

/**
 * Base class for jamba parameters (non templated)
 */
class IJmbParamDef : public IParamDef
{
public:
  IJmbParamDef(const ParamID iParamID,
               VstString16 iTitle,
               Owner const iOwner,
               bool const iTransient,
               bool const iShared)
    : IParamDef(iParamID, std::move(iTitle), iOwner, iTransient),
      fShared{iShared}
  {}

  ~IJmbParamDef() override = default;

  // writeDefaultValue
  virtual void writeDefaultValue(std::ostream &oStreamer) const = 0;

  /**
   * Create a new `IGUIJmbParameter` of the proper subtype.
   */
  virtual std::shared_ptr<GUI::Params::IGUIJmbParameter> newGUIParam() = 0;

  /**
   * @return `true` if the parameter can be serialized (so provides a means to be serialized)
   */
  virtual bool isSerializable() const = 0;

public:
  bool const fShared;
};

/**
 * Base class for all non vst parameters (need to provide serialization/deserialization)
 *
 * @tparam T the underlying type of the param */
template<typename T>
class JmbParamDef : public IJmbParamDef, IParamSerializer<T>
{
public:
  using ParamType = T;

  JmbParamDef(ParamID const iParamID,
              VstString16 iTitle,
              Owner const iOwner,
              bool const iTransient,
              bool const iShared,
              ParamType const &iDefaultValue,
              std::shared_ptr<IParamSerializer<ParamType>> iSerializer) :
    IJmbParamDef(iParamID, std::move(iTitle), iOwner, iTransient, iShared),
    fDefaultValue{iDefaultValue},
    fSerializer{std::move(iSerializer)}
  {}

  // readFromStream
  tresult readFromStream(IBStreamer &iStreamer, ParamType &oValue) const override;
  ParamType readFromStream(IBStreamer &iStreamer) const;

  // writeToStream
  tresult writeToStream(ParamType const &iValue, IBStreamer &oStreamer) const override;

  // writeToStream
  void writeToStream(ParamType const &iValue, std::ostream &oStreamer) const override;

  // writeDefaultValue
  void writeDefaultValue(std::ostream &oStreamer) const override;

  // readFromMessage
  tresult readFromMessage(Message const &iMessage, ParamType &oValue) const;

  // writeToMessage
  tresult writeToMessage(ParamType const &iValue, Message &oMessage) const;

  /**
   * Return the value as a utf-8 string
   *
   * @param iPrecision if < 0 uses `fPrecision` otherwise use the one provided
   */
  std::string toUTF8String(ParamType const &iValue, int32 iPrecision) const
  {
    if(fSerializer)
      return fSerializer->toString(iValue, iPrecision);
    else
      return "";
  }

  /**
   * @return the discrete converter associated with this param def or `nullptr` if there isn't one.
   */
  std::shared_ptr<IDiscreteConverter<T>> getDiscreteConverter() const
  {
    // Implementation note: at this moment, only checks if the serializer also implements the API.
    // But possible to add an additional separate field to set it explicitly if there is a need
    return std::dynamic_pointer_cast<IDiscreteConverter<T>>(fSerializer);
  }

  // computeMessageAttrID
  std::string computeMessageAttrID() const
  {
    return "__param__" + std::to_string(fParamID);
  }

  /**
   * Create a new `IGUIJmbParameter` of the proper subtype.
   *
   * Implementation note: because `IGUIJmbParameter` depends on `ParamDef` the implementation of
   * this templated method is defined in `GUIState.h`.
   */
  std::shared_ptr<GUI::Params::IGUIJmbParameter> newGUIParam() override;

  /**
   * @return `true` if a serializer was provided
   */
  bool isSerializable() const override { return fSerializer != nullptr; }

public:
  const ParamType fDefaultValue;
  const std::shared_ptr<IParamSerializer<ParamType>> fSerializer;
};

//------------------------------------------------------------------------
// JmbParamDef::readFromStream
//------------------------------------------------------------------------
template<typename T>
tresult JmbParamDef<T>::readFromStream(IBStreamer &iStreamer, T &oValue) const
{
  if(fSerializer)
  {
    return fSerializer->readFromStream(iStreamer, oValue);
  }
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// JmbParamDef::readFromStream
//------------------------------------------------------------------------
template<typename T>
T JmbParamDef<T>::readFromStream(IBStreamer &iStreamer) const
{
  T value;
  if(readFromStream(iStreamer, value) != kResultOk)
    value = fDefaultValue;
  return value;
}


//------------------------------------------------------------------------
// JmbParamDef::writeToStream
//------------------------------------------------------------------------
template<typename T>
tresult JmbParamDef<T>::writeToStream(const T &iValue, IBStreamer &oStreamer) const
{
  if(fSerializer)
    return fSerializer->writeToStream(iValue, oStreamer);
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// JmbParamDef::writeToStream
//------------------------------------------------------------------------
template<typename T>
void JmbParamDef<T>::writeToStream(const ParamType &iValue, std::ostream &oStreamer) const
{
  if(fSerializer)
    fSerializer->writeToStream(iValue, oStreamer);
}

//------------------------------------------------------------------------
// JmbParamDef::writeDefaultValue
//------------------------------------------------------------------------
template<typename T>
void JmbParamDef<T>::writeDefaultValue(std::ostream &oStreamer) const
{
  writeToStream(fDefaultValue, oStreamer);
}

//------------------------------------------------------------------------
// JmbParamDef::readFromMessage
//------------------------------------------------------------------------
template<typename T>
tresult JmbParamDef<T>::readFromMessage(Message const &iMessage, ParamType &oValue) const
{
  if(fSerializer)
    return iMessage.getSerializableValue(computeMessageAttrID().c_str(), *this, oValue);
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// JmbParamDef::writeToMessage
//------------------------------------------------------------------------
template<typename T>
tresult JmbParamDef<T>::writeToMessage(const ParamType &iValue, Message &oMessage) const
{
  if(fSerializer)
    return oMessage.setSerializableValue(computeMessageAttrID().c_str(), *this, iValue);
  else
    return kResultFalse;
}

//------------------------------------------------------------------------
// VstParam - define shortcut notation
//------------------------------------------------------------------------
template<typename T>
using VstParam = std::shared_ptr<VstParamDef<T>>;
using RawVstParam = std::shared_ptr<RawVstParamDef>;

//------------------------------------------------------------------------
// JmbParam - define shortcut notation
//------------------------------------------------------------------------
template<typename T>
using JmbParam = std::shared_ptr<JmbParamDef<T>>;

}
