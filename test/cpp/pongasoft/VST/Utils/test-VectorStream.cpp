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
#include <pongasoft/VST/VstUtils/VectorStream.h>
#include <base/source/fstreamer.h>
#include <array>

namespace pongasoft::VST::VstUtils::TestVectorStream {

// TestVectorStream - test_write
TEST(TestVectorStream, test_write)
{
  VectorStream vs{};

  ASSERT_EQ(0, vs.size());
  ASSERT_EQ(0, vs.pos());

  auto seek = [&vs](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, vs.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  seek(3, VectorStream::kIBSeekSet, 0);
  seek(3, VectorStream::kIBSeekCur, 0);
  seek(-3, VectorStream::kIBSeekEnd, 0);

  std::array<int8, 5> str = {'a', 'b', 'c', 'd', 'e'};

  auto write = [&vs, &str](int64 start, int64 end, std::vector<int8> iExpected) {
    int32 numWritten = 0;
    ASSERT_EQ(kResultOk, vs.write(str.data() + start, end - start, &numWritten));
    ASSERT_EQ(end - start, numWritten);
    ASSERT_EQ(iExpected.size(), vs.size());
    ASSERT_EQ(iExpected, std::vector<int8>(vs.data(), vs.data() + vs.size()));
  };

  write(0, 3, {'a', 'b', 'c'});
  ASSERT_EQ(3, vs.pos());

  write(3, 5, {'a', 'b', 'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());

  seek(2, VectorStream::kIBSeekSet, 2);
  write(0, 1, {'a', 'b', 'a', 'd', 'e'});
  ASSERT_EQ(3, vs.pos());

  write(0, 4, {'a', 'b', 'a', 'a', 'b', 'c', 'd'});
  ASSERT_EQ(7, vs.pos());

}

// TestVectorStream - test_read
TEST(TestVectorStream, test_read)
{
  VectorStream vs{};

  ASSERT_EQ(0, vs.size());
  ASSERT_EQ(0, vs.pos());

  auto seek = [&vs](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, vs.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  std::array<int8, 5> str = {'a', 'b', 'c', 'd', 'e'};

  auto write = [&vs, &str](int64 start, int64 end, std::vector<int8> iExpected) {
    int32 numWritten = 0;
    ASSERT_EQ(kResultOk, vs.write(str.data() + start, end - start, &numWritten));
    ASSERT_EQ(end - start, numWritten);
    ASSERT_EQ(iExpected.size(), vs.size());
    ASSERT_EQ(iExpected, std::vector<int8>(vs.data(), vs.data() + vs.size()));
  };


  auto read = [&vs](int32 numBytes, std::vector<int8> iExpected) {
    int8 buf[numBytes];
    int32 numBytesRead = 0;
    ASSERT_EQ(kResultOk, vs.read(buf, numBytes, &numBytesRead));
    ASSERT_EQ(iExpected.size(), numBytesRead);
    ASSERT_EQ(iExpected, std::vector<int8>(&buf[0], &buf[numBytesRead]));
  };

  write(0, 3, {'a', 'b', 'c'});
  ASSERT_EQ(3, vs.pos());

  write(3, 5, {'a', 'b', 'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());
  read(3, {});

  seek(2, VectorStream::kIBSeekSet, 2);
  ASSERT_EQ(2, vs.pos());
  read(0, {});
  ASSERT_EQ(2, vs.pos());
  read(1, {'c'});
  ASSERT_EQ(3, vs.pos());
  seek(-3, VectorStream::kIBSeekCur, 0);
  read(2, {'a', 'b'});
  ASSERT_EQ(2, vs.pos());
  read(10, {'c', 'd', 'e'});
  ASSERT_EQ(5, vs.pos());
  read(10, {});
  ASSERT_EQ(5, vs.pos());
  seek(-3, VectorStream::kIBSeekEnd, 2);
  ASSERT_EQ(2, vs.pos());
  read(1, {'c'});
}

}