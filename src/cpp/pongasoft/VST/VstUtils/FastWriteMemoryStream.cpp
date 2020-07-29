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
#include "FastWriteMemoryStream.h"
#include <memory>
#include <algorithm>

namespace pongasoft::VST::VstUtils {

IMPLEMENT_FUNKNOWN_METHODS (FastWriteMemoryStream, IBStream, IBStream::iid)

//------------------------------------------------------------------------
// FastWriteMemoryStream::FastWriteMemoryStream
//------------------------------------------------------------------------
FastWriteMemoryStream::FastWriteMemoryStream()
  : memory(nullptr), memorySize(0), size(0), cursor(0), allocationError(false)
{
  FUNKNOWN_CTOR
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::~FastWriteMemoryStream
//------------------------------------------------------------------------
FastWriteMemoryStream::~FastWriteMemoryStream()
{
  if(memory)
    ::free(memory);

  FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::read
//------------------------------------------------------------------------
tresult PLUGIN_API FastWriteMemoryStream::read(void *data, int32 numBytes, int32 *numBytesRead)
{
  if(memory == nullptr)
  {
    if(allocationError)
      return kOutOfMemory;
    numBytes = 0;
  }
  else
  {
    // Does read exceed size ?
    if(cursor + numBytes > size)
    {
      int32 maxBytes = int32(size - cursor);

      // Has length become zero or negative ?
      if(maxBytes <= 0)
      {
        cursor = size;
        numBytes = 0;
      }
      else
        numBytes = maxBytes;
    }

    if(numBytes)
    {
      memcpy(data, &memory[cursor], numBytes);
      cursor += numBytes;
    }
  }

  if(numBytesRead)
    *numBytesRead = numBytes;

  return kResultTrue;
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::write
//------------------------------------------------------------------------
tresult PLUGIN_API FastWriteMemoryStream::write(void *buffer, int32 numBytes, int32 *numBytesWritten)
{
  if(allocationError)
    return kOutOfMemory;
  if(buffer == nullptr)
    return kInvalidArgument;

  // Does write exceed size ?
  TSize requiredSize = cursor + numBytes;
  if(requiredSize > size)
  {
    if(requiredSize > memorySize)
      setSize(requiredSize);
    else
      size = requiredSize;
  }

  // Copy data
  if(memory && cursor >= 0 && numBytes > 0)
  {
    memcpy(&memory[cursor], buffer, numBytes);
    // Update cursor
    cursor += numBytes;
  }
  else
    numBytes = 0;

  if(numBytesWritten)
    *numBytesWritten = numBytes;

  return kResultTrue;
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::seek
//------------------------------------------------------------------------
tresult PLUGIN_API FastWriteMemoryStream::seek(int64 pos, int32 mode, int64 *result)
{
  switch(mode)
  {
    case kIBSeekSet:
      cursor = pos;
      break;
    case kIBSeekCur:
      cursor = cursor + pos;
      break;
    case kIBSeekEnd:
      cursor = size + pos;
      break;
  }

  cursor = Utils::clamp<int64>(cursor, 0, size);

  if(result)
    *result = cursor;

  return kResultTrue;
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::tell
//------------------------------------------------------------------------
tresult PLUGIN_API FastWriteMemoryStream::tell(int64 *pos)
{
  if(!pos)
    return kInvalidArgument;

  *pos = cursor;
  return kResultTrue;
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::setSize
//------------------------------------------------------------------------
void FastWriteMemoryStream::setSize(TSize s)
{
  if(s <= 0)
  {
    if(memory)
      free(memory);

    memory = nullptr;
    memorySize = 0;
    size = 0;
    cursor = 0;
    return;
  }

  if(memorySize >= s)
    return;

  auto newMemorySize = std::max(memorySize, static_cast<TSize>(256));
  newMemorySize = std::max(newMemorySize * 2, s);

//  DLOG_F(INFO, "FastWriteMemoryStream::setSize %lld -> %lld", memorySize, newMemorySize);

  char *newMemory;

  if(memory)
  {
    newMemory = (char *) realloc(memory, (size_t) newMemorySize);
    if(newMemory == nullptr && newMemorySize > 0)
    {
      newMemory = (char *) malloc((size_t) newMemorySize);
      if(newMemory)
      {
        memcpy(newMemory, memory, (size_t) std::min(newMemorySize, memorySize));
        free(memory);
      }
    }
  }
  else
    newMemory = (char *) malloc((size_t) newMemorySize);

  if(newMemory == nullptr)
  {
    if(newMemorySize > 0)
      allocationError = true;

    memory = nullptr;
    memorySize = 0;
    size = 0;
    cursor = 0;
  }
  else
  {
    memory = newMemory;
    memorySize = newMemorySize;
    size = s;
  }
}

//------------------------------------------------------------------------
// FastWriteMemoryStream::reset
//------------------------------------------------------------------------
void FastWriteMemoryStream::reset()
{
  size = 0;
  cursor = 0;
}

}