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
#pragma once

#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <algorithm>
#include <memory>
#include <public.sdk/source/vst/vstpresetfile.h>
#include <string>
#include <sstream>

#include "ParamSerializers.h"

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

static const auto ATTR_MSG_ID = "ATTR_MSG_ID";

using MessageID = int;

/**
 * Simple wrapper class with better api
 */
class Message
{
public:
  explicit Message(IMessage *message) : fMessage(message) {}

  inline MessageID getMessageID() const
  {
    return static_cast<MessageID>(getInt(ATTR_MSG_ID, -1));
  }

  inline void setMessageID(MessageID messageID)
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
  inline tresult setBinary(IAttributeList::AttrID id, const T *iData, uint32 iSize)
  {
    return fMessage->getAttributes()->setBinary(id, iData, iSize * sizeof(T));
  }

  /**
   * Gets a binary message.
   * @param iData the array into which the binary data will be copied
   * @param iSize the number of elements in iData array (NOT the size in bytes!)
   * @return -1 if cannot ready binary otherwise number of elements read (always <= iSize)
   */
  template<typename T>
  inline int32 getBinary(IAttributeList::AttrID id, T *iData, uint32 iSize) const;

  /**
   * Serializes the parameter value as an entry in the message
   *
   * @return kResultOk if successful */
  template<typename T>
  tresult setSerializableValue(IAttributeList::AttrID id, IParamSerializer<T> const &iSerializer, T const &iValue);

  /**
   * Deserializes the parameter value from an entry in the message
   *
   * @return kResultOk if successful */
  template<typename T>
  tresult getSerializableValue(IAttributeList::AttrID id, IParamSerializer<T> const &iSerializer, T &oValue) const;

private:
  IMessage *fMessage;
};

//------------------------------------------------------------------------
// Message::getBinary
//------------------------------------------------------------------------
template<typename T>
int32 Message::getBinary(IAttributeList::AttrID id, T *iData, uint32 iSize) const
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
// Message::setSerializableValue
//------------------------------------------------------------------------
template<typename T>
tresult Message::setSerializableValue(IAttributeList::AttrID id, const IParamSerializer<T> &iSerializer, const T &iValue)
{
  BufferStream stream{};

  IBStreamer streamer{&stream};

  tresult res = iSerializer.writeToStream(iValue, streamer);
  if(res == kResultOk)
  {
    auto const &buffer = stream.getBuffer();
    return setBinary(id, buffer.int8Ptr(), buffer.getFillSize());
  }
  return res;
}

//------------------------------------------------------------------------
// Message::getSerializableValue
//------------------------------------------------------------------------
template<typename T>
tresult Message::getSerializableValue(IAttributeList::AttrID id, const IParamSerializer<T> &iSerializer, T &oValue) const
{
  const void *data;
  uint32 size;

  tresult res = fMessage->getAttributes()->getBinary(id, data, size);

  if(res != kResultOk)
    return res;

  // TODO this line unnecessarily copies the data but there is no other API
  Buffer buffer(data, size);

  BufferStream stream{std::move(buffer)};
  stream.seek(IBStream::kIBSeekSet, 0, nullptr); // make sure it is at the beginning of the stream

  IBStreamer streamer{&stream};

  return iSerializer.readFromStream(streamer, oValue);
}

}
}