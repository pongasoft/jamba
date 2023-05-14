/*
 * Copyright (c) 2019 pongasoft
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

#include <gtest/gtest.h>
#include <pongasoft/VST/VstUtils/FastWriteMemoryStream.h>
#include <base/source/fstreamer.h>
#include <array>

namespace pongasoft::VST::VstUtils::TestFastWriteMemoryStream {

// TestFastWriteMemoryStream - test_write
TEST(TestFastWriteMemoryStream, test_write)
{
  FastWriteMemoryStream vs{};

  ASSERT_EQ(0, vs.getSize());
  ASSERT_EQ(0, vs.pos());

  auto seek = [&vs](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, vs.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  seek(3, FastWriteMemoryStream::kIBSeekSet, 0);
  seek(3, FastWriteMemoryStream::kIBSeekCur, 0);
  seek(-3, FastWriteMemoryStream::kIBSeekEnd, 0);

  std::array<int8, 5> str = {'a', 'b', 'c', 'd', 'e'};

  auto write = [&vs, &str](int32 start, int32 end, std::vector<int8> iExpected) {
    int32 numWritten = 0;
    ASSERT_EQ(kResultOk, vs.write(str.data() + start, end - start, &numWritten));
    ASSERT_EQ(end - start, numWritten);
    ASSERT_EQ(iExpected.size(), vs.getSize());
    ASSERT_EQ(iExpected, std::vector<int8>(vs.getData(), vs.getData() + vs.getSize()));
  };

  write(0, 3, {'a', 'b', 'c'});
  ASSERT_EQ(3, vs.pos());

  write(3, 5, {'a', 'b', 'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());

  seek(2, FastWriteMemoryStream::kIBSeekSet, 2);
  write(0, 1, {'a', 'b', 'a', 'd', 'e'});
  ASSERT_EQ(3, vs.pos());

  write(0, 4, {'a', 'b', 'a', 'a', 'b', 'c', 'd'});
  ASSERT_EQ(7, vs.pos());

}

// TestFastWriteMemoryStream - test_read
TEST(TestFastWriteMemoryStream, test_read)
{
  FastWriteMemoryStream vs{};

  ASSERT_EQ(0, vs.getSize());
  ASSERT_EQ(0, vs.pos());

  auto seek = [&vs](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, vs.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  std::array<int8, 5> str = {'a', 'b', 'c', 'd', 'e'};

  auto write = [&vs, &str](int32 start, int32 end, std::vector<int8> iExpected) {
    int32 numWritten = 0;
    ASSERT_EQ(kResultOk, vs.write(str.data() + start, end - start, &numWritten));
    ASSERT_EQ(end - start, numWritten);
    ASSERT_EQ(iExpected.size(), vs.getSize());
    ASSERT_EQ(iExpected, std::vector<int8>(vs.getData(), vs.getData() + vs.getSize()));
  };


  auto read = [&vs](int32 numBytes, std::vector<int8> iExpected) {
    std::vector<int8> buf{};
    buf.resize(numBytes + 1);
    int32 numBytesRead = 0;
    ASSERT_EQ(kResultOk, vs.read(buf.data(), numBytes, &numBytesRead));
    ASSERT_EQ(iExpected.size(), numBytesRead);
    ASSERT_EQ(iExpected, std::vector<int8>(buf.begin(), buf.begin() + numBytesRead));
  };

  write(0, 3, {'a', 'b', 'c'});
  ASSERT_EQ(3, vs.pos());

  write(3, 5, {'a', 'b', 'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());
  read(3, {});

  seek(2, FastWriteMemoryStream::kIBSeekSet, 2);
  ASSERT_EQ(2, vs.pos());
  read(0, {});
  ASSERT_EQ(2, vs.pos());
  read(1, {'c'});
  ASSERT_EQ(3, vs.pos());
  seek(-3, FastWriteMemoryStream::kIBSeekCur, 0);
  read(2, {'a', 'b'});
  ASSERT_EQ(2, vs.pos());
  read(10, {'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());
  read(10, {});
  ASSERT_EQ(5, vs.pos());
  seek(-3, FastWriteMemoryStream::kIBSeekEnd, 2);
  ASSERT_EQ(2, vs.pos());
  read(1, {'c'});
}

}