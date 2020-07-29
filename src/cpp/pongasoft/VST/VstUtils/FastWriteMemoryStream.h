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

#ifndef JAMBA_FASTWRITEMEMORYSTREAM_H
#define JAMBA_FASTWRITEMEMORYSTREAM_H

#include <pluginterfaces/base/ibstream.h>

namespace pongasoft::VST::VstUtils {

using namespace Steinberg;

/**
 * This class is a copy of `MemoryStream` for the purpose of fixing the growing issue encountered
 * on Windows 10 when writing a lot of data to it. `MemoryStream` essentially adds 4k whenever new memory
 * is needed and although `realloc` is being used, it is extremelly slow on PC/Windows 10. This class
 * implements a similar strategy found in `std::vector`: the memory is doubled every time which leads
 * to much less `realloc` and much faster performance.
 *
 * This class does not implement the "non-owning" code since `MemoryStream` can be used for this (no memory
 * allocation).
 *
 * This class fixes the issue with `MemoryStream::seek` which blindly sets the cursor potentially outside
 * the valid boundaries.
 */
class FastWriteMemoryStream : public IBStream
{
public:
  //------------------------------------------------------------------------
  FastWriteMemoryStream();

  virtual ~FastWriteMemoryStream();

  //---IBStream---------------------------------------
  virtual tresult PLUGIN_API read(void *buffer, int32 numBytes, int32 *numBytesRead) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API write(void *buffer, int32 numBytes, int32 *numBytesWritten) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API seek(int64 pos, int32 mode, int64 *result) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API tell(int64 *pos) SMTG_OVERRIDE;

  TSize getSize() const { return size; };    ///< returns the current memory size
  void setSize(TSize size);  ///< set the memory size, a realloc will occur if memory already used
  void reset();
  inline void clear() { setSize(0); }
  inline char const* getData() const { return memory; }
  inline int64 pos() const { return cursor; }

  //------------------------------------------------------------------------
DECLARE_FUNKNOWN_METHODS
protected:
  char *memory;        // memory block
  TSize memorySize;      // size of the memory block
  TSize size;          // size of the stream
  int64 cursor;        // stream pointer
  bool allocationError;       // stream invalid
};

}


#endif //JAMBA_FASTWRITEMEMORYSTREAM_H
