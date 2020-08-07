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

#ifndef JAMBA_READONLYMEMORYSTREAM_H
#define JAMBA_READONLYMEMORYSTREAM_H

#include <pluginterfaces/base/ibstream.h>

namespace pongasoft::VST::VstUtils {

using namespace Steinberg;

class ReadOnlyMemoryStream : public IBStream
{
public:
  //! Constructor
  ReadOnlyMemoryStream(char const *iMemory, TSize iMemorySize);

  virtual ~ReadOnlyMemoryStream() = default;

  //---IBStream---------------------------------------
  virtual tresult PLUGIN_API read(void *buffer, int32 numBytes, int32 *numBytesRead) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API write(void *buffer, int32 numBytes, int32 *numBytesWritten) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API seek(int64 pos, int32 mode, int64 *result) SMTG_OVERRIDE;
  virtual tresult PLUGIN_API tell(int64 *pos) SMTG_OVERRIDE;

  TSize getSize() const { return fMemorySize - fPos; };    ///< returns the current memory size (how many chars can be read until the end)
  inline char const* getData() const { return fMemory; }
  TSize getMemorySize() const { return fMemorySize; };    ///< returns the total size of the memory
  inline int64 pos() const { return fPos; }

  //------------------------------------------------------------------------
DECLARE_FUNKNOWN_METHODS

protected:
  char const *fMemory;        // memory block
  TSize fMemorySize;    // size of the memory block
  int64 fPos;           // stream pointer
};

}

#endif //JAMBA_READONLYMEMORYSTREAM_H
