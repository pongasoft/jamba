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

#ifndef JAMBA_VECTORSTREAM_H
#define JAMBA_VECTORSTREAM_H

#include <pluginterfaces/base/ibstream.h>
#include <vector>
#include <pongasoft/logging/logging.h>
#include <algorithm>

namespace pongasoft::VST::VstUtils {

using namespace Steinberg;

class VectorStream : public IBStream
{
public:
  tresult read(void *buffer, int32 numBytes, int32 *numBytesRead) override;

  tresult write(void *buffer, int32 numBytes, int32 *numBytesWritten) override;

  tresult seek(int64 pos, int32 mode, int64 *result) override;

  inline tresult reset() { fPos = 0; return kResultOk; }

  tresult tell(int64 *pos) override;

  inline int8 const *readData() const { return fBuffer.data() + fPos; }
  inline int64 readSize() const { return static_cast<int64>(fBuffer.size() - fPos); }

  inline std::vector<int8> const &buffer() { return fBuffer; }
  inline int8 const *data() const { return fBuffer.data(); }
  inline int64 size() const { return static_cast<int64>(fBuffer.size()); }
  inline int64 pos() const { return static_cast<int64>(fPos); }

public:
  //---from FUnknown------------------
  DECLARE_FUNKNOWN_METHODS

protected:
  using size_type = std::vector<int8>::size_type;
  std::vector<int8> fBuffer{};
  size_type fPos{};
};

}


#endif //JAMBA_VECTORSTREAM_H
