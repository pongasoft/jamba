/*
 * Copyright (c) 2020 pongasoft
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

#include <pongasoft/Utils/Misc.h>
#include "VectorStream.h"

namespace pongasoft::VST::VstUtils {

//------------------------------------------------------------------------
// BufferStream implementation
//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (VectorStream, IBStream, IBStream::iid)

//------------------------------------------------------------------------
// VectorStream::write
//------------------------------------------------------------------------
tresult VectorStream::write(void *buffer, int32 numBytes, int32 *numBytesWritten)
{
  if(numBytes < 0 || !buffer)
    return kInvalidArgument;

  if(numBytes > 0)
  {
    auto capacity = fBuffer.capacity();

    auto buf = static_cast<int8 *>(buffer);

    if(fPos == fBuffer.size())
    {
      std::copy(buf, buf + numBytes, std::back_inserter(fBuffer));
    }
    else
    {
      auto numBytesToCopy = std::min(static_cast<size_type>(numBytes), fBuffer.size() - fPos);
      std::copy(buf, buf + numBytesToCopy, std::begin(fBuffer) + fPos);
      if(numBytes > numBytesToCopy)
        std::copy(buf + numBytesToCopy, buf + numBytes, std::back_inserter(fBuffer));
    }

    fPos += numBytes;

    // sanity check
    DCHECK_F(fPos >= 0 && fPos <= fBuffer.size());

    if(capacity != fBuffer.capacity())
      DLOG_F(INFO, "Buffer %p capacity %lu -> %lu", this, capacity, fBuffer.capacity());
  }

  if(numBytesWritten)
    *numBytesWritten = numBytes;

  return kResultOk;
}

//------------------------------------------------------------------------
// VectorStream::seek
//------------------------------------------------------------------------
tresult VectorStream::seek(int64 pos, int32 mode, int64 *result)
{
  auto newPos = static_cast<size_type>(pos);
  switch(mode)
  {
    case kIBSeekSet:
      break;

    case kIBSeekCur:
      newPos += fPos;
      break;

    case kIBSeekEnd:
      newPos += fBuffer.size();
      break;

    default:
      DLOG_F(ERROR, "should not be here");
      newPos = 0;
      break;
  }

  fPos = Utils::clamp<size_type>(newPos, 0, fBuffer.size());

  tell(result);

  return kResultOk;
}

//------------------------------------------------------------------------
// VectorStream::tell
//------------------------------------------------------------------------
tresult VectorStream::tell(int64 *pos)
{
  if(pos)
    *pos = static_cast<int64>(fPos);
  return kResultOk;
}

//------------------------------------------------------------------------
// VectorStream::read
//------------------------------------------------------------------------
tresult VectorStream::read(void *buffer, int32 numBytes, int32 *numBytesRead)
{
  if(numBytes < 0 || !buffer)
    return kInvalidArgument;

  auto numBytesToRead = std::min(static_cast<size_type>(numBytes), fBuffer.size() - fPos);

  if(numBytesToRead > 0)
  {
    auto start = std::begin(fBuffer) + fPos;
    std::copy(start, start + numBytesToRead, static_cast<int8 *>(buffer));

    fPos += numBytesToRead;

    DCHECK_F(fPos >= 0 && fPos <= fBuffer.size());
  }

  if(numBytesRead)
    *numBytesRead = static_cast<int32>(numBytesToRead);

  return kResultOk;
}


}