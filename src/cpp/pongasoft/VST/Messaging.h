#pragma once

#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <algorithm>

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

private:
  IMessage *fMessage;
};

}
}