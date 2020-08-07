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
#include "ReadOnlyMemoryStream.h"

namespace pongasoft::VST::VstUtils {

IMPLEMENT_FUNKNOWN_METHODS(ReadOnlyMemoryStream, IBStream, IBStream::iid)

//------------------------------------------------------------------------
// ReadOnlyMemoryStream::ReadOnlyMemoryStream
//------------------------------------------------------------------------
ReadOnlyMemoryStream::ReadOnlyMemoryStream(char const *iMemory, TSize iMemorySize) :
  IBStream{}, fMemory{iMemory}, fMemorySize{iMemorySize}, fPos{}
{
  FUNKNOWN_CTOR
  DCHECK_F(iMemorySize >= 0);
}

//------------------------------------------------------------------------
// ReadOnlyMemoryStream::read
//------------------------------------------------------------------------
tresult ReadOnlyMemoryStream::read(void *buffer, int32 numBytes, int32 *numBytesRead)
{
  if(!fMemory || fMemorySize <= 0)
  {
    numBytes = 0;
  }
  else
  {
    // Does read exceed size ?
    if(fPos + numBytes > fMemorySize)
    {
      int32 maxBytes = int32(fMemorySize - fPos);

      // Has length become zero or negative ?
      if(maxBytes <= 0)
      {
        fPos = fMemorySize;
        numBytes = 0;
      }
      else
        numBytes = maxBytes;
    }

    if(numBytes)
    {
      memcpy(buffer, &fMemory[fPos], numBytes);
      fPos += numBytes;
    }
  }

  if(numBytesRead)
    *numBytesRead = numBytes;

  return kResultTrue;
}

//------------------------------------------------------------------------
// ReadOnlyMemoryStream::write
//------------------------------------------------------------------------
tresult ReadOnlyMemoryStream::write(void *buffer, int32 numBytes, int32 *numBytesWritten)
{
  // Read only => no write allowed
  return kResultFalse;
}

//------------------------------------------------------------------------
// ReadOnlyMemoryStream::seek
//------------------------------------------------------------------------
tresult ReadOnlyMemoryStream::seek(int64 pos, int32 mode, int64 *result)
{
  switch(mode)
  {
    case kIBSeekSet:
      fPos = pos;
      break;
    case kIBSeekCur:
      fPos = fPos + pos;
      break;
    case kIBSeekEnd:
      fPos = fMemorySize + pos;
      break;
  }

  fPos = Utils::clamp<TSize>(fPos, 0, fMemorySize);

  if(result)
    *result = fPos;

  return kResultTrue;
}

//------------------------------------------------------------------------
// ReadOnlyMemoryStream::tell
//------------------------------------------------------------------------
tresult ReadOnlyMemoryStream::tell(int64 *pos)
{
  if(!pos)
    return kInvalidArgument;

  *pos = fPos;
  return kResultTrue;
}


}