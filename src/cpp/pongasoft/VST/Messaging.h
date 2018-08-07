/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <algorithm>
#include <memory>
#include <public.sdk/source/vst/vstpresetfile.h>
#include <string>
#include <sstream>
#include "ParamDef.h"

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

static const auto ATTR_MSG_ID = "ATTR_MSG_ID";

/**
 * Simple wrapper class with better api
 */
class Message
{
public:
  explicit Message(IMessage *message) : fMessage(message) {}

  inline int getMessageID() const
  {
    return static_cast<int>(getInt(ATTR_MSG_ID, -1));
  }

  inline void setMessageID(int messageID)
  {
    fMessage->getAttributes()->setInt(ATTR_MSG_ID, messageID);
  }

  inline int64 getInt(IAttributeList::AttrID id, int64 defaultValue) const
  {
    int64 value;
    if(fMessage->getAttributes()->getInt(id, value) != kResultOk)
      value = defaultValue;
    return value;
  }

  inline void setInt(IAttributeList::AttrID id, int64 value)
  {
    fMessage->getAttributes()->setInt(id, value);
  }

  inline double getFloat(IAttributeList::AttrID id, double defaultValue) const
  {
    double value;
    if(fMessage->getAttributes()->getFloat(id, value) != kResultOk)
      value = defaultValue;
    return value;
  }

  inline void setFloat(IAttributeList::AttrID id, double value)
  {
    fMessage->getAttributes()->setFloat(id, value);
  }

  /**
   * Sets a binary message.
   * @param iSize the number of elements in iData array (NOT the size in bytes!)
   */
  template<typename T>
  inline void setBinary(IAttributeList::AttrID id, const T *iData, uint32 iSize)
  {
    fMessage->getAttributes()->setBinary(id, iData, iSize * sizeof(T));
  }

  /**
   * Gets a binary message.
   * @param iData the array into which the binary data will be copied
   * @param iSize the number of elements in iData array (NOT the size in bytes!)
   * @return -1 if cannot ready binary otherwise number of elements read (always <= iSize)
   */
  template<typename T>
  inline int32 getBinary(IAttributeList::AttrID id, T *iData, uint32 iSize) const
  {
    const void *data;
    uint32 size;

    if(fMessage->getAttributes()->getBinary(id, data, size) != kResultOk)
      return -1;

    uint32 oSize = size / sizeof(T);
    oSize = std::min(iSize, oSize);

    memcpy(iData, data, oSize * sizeof(T));

    return oSize;
  }

  /**
   * Serializes the parameter value as an entry in the message (technically 2 entries: one for the byte array
   * of serialized data and one for the size of this array)
   *
   * @return kResultOk if successful */
  template<typename T>
  tresult setSerParam(SerParam<T> const &iParamDef, T const &iValue);

  /**
   * Extract the serialized parameter from the stream (reverse of setSerParam)
   *
   * @return kResultOk if successful */
  template<typename T>
  tresult getSerParam(SerParam<T> const &iParamDef, T &oValue) const;

  /**
   * Compute the attribute name for an ser param given its id (must be a string :(
   */
  inline static std::string computeParamAttrID(ParamID iParamID)
  {
    std::ostringstream s;
    s << "__p__" << iParamID;
    return s.str();
  }

  /**
   * Compute the attribute name for the size of the ser parama given its id (must be a string :(
   */
  inline static std::string computeSizeParamAttrID(ParamID iParamID)
  {
    std::ostringstream s;
    s << "__p__" << iParamID << "__size";
    return s.str();
  }

private:
  IMessage *fMessage;
};

/**
 * Internal class to override the BufferStream class and give access to the buffer
 */
class BufferStream : public Steinberg::Vst::BufferStream
{
public:
  BufferStream() : Steinberg::Vst::BufferStream() {}
  explicit BufferStream(Buffer &&iFrom) : Steinberg::Vst::BufferStream()
  {
    mBuffer.take(iFrom);
  }
  Buffer const &getBuffer() const { return mBuffer; }
};

//------------------------------------------------------------------------
// Message::setSerParam
//------------------------------------------------------------------------
template<typename T>
tresult Message::setSerParam(const std::shared_ptr<SerParamDef<T>> &iParamDef, const T &iValue)
{
  BufferStream stream{};
  
  IBStreamer streamer{&stream};

  tresult res = iParamDef->writeToStream(iValue, streamer);
  if(res == kResultOk)
  {
    auto const &buffer = stream.getBuffer();
    
    setInt(computeSizeParamAttrID(iParamDef->fParamID).c_str(), buffer.getFillSize());
    setBinary(computeParamAttrID(iParamDef->fParamID).c_str(), buffer.int8Ptr(), buffer.getFillSize());
    return kResultOk;
  }
  return res;
}

//------------------------------------------------------------------------
// Message::getSerParam
//------------------------------------------------------------------------
template<typename T>
tresult Message::getSerParam(const SerParam<T> &iParamDef, T &oValue) const
{

  int64 size = getInt(computeSizeParamAttrID(iParamDef->fParamID).c_str(), -1);
  if(size > 0)
  {
    auto bufferSize = static_cast<uint32>(size);
    Buffer buffer(bufferSize);
    getBinary(computeParamAttrID(iParamDef->fParamID).c_str(), buffer.int8Ptr(), bufferSize);

    BufferStream stream{std::move(buffer)};

    IBStreamer streamer{&stream};

    return iParamDef->readFromStream(streamer, oValue);
  }

  return kResultFalse;


}

}
}