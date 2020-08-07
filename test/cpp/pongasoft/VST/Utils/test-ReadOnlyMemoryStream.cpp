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
#include <pongasoft/VST/VstUtils/ReadOnlyMemoryStream.h>
#include <base/source/fstreamer.h>
#include <array>

namespace pongasoft::VST::VstUtils::TestReadOnlyMemoryStream {

// TestReadOnlyMemoryStream - test_write
TEST(TestReadOnlyMemoryStream, test_write)
{
  constexpr int64 size = 20;
  std::array<char, size> memory{};

  ReadOnlyMemoryStream ms{memory.data(), memory.size()};

  ASSERT_EQ(0, ms.pos());
  ASSERT_EQ(size, ms.getMemorySize());
  ASSERT_EQ(size, ms.getSize());

  auto seek = [&ms](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, ms.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  seek(3, ReadOnlyMemoryStream::kIBSeekSet, 3);
  seek(3, ReadOnlyMemoryStream::kIBSeekCur, 6);
  seek(-3, ReadOnlyMemoryStream::kIBSeekEnd, 17);

  std::array<char, size> buf{};

  // write is simply not allowed
  ASSERT_EQ(kResultFalse, ms.write(buf.data(), buf.size(), nullptr));
}

// TestReadOnlyMemoryStream - test_read
TEST(TestReadOnlyMemoryStream, test_read)
{
  constexpr int64 size = 6;
  std::array<char, size> memory{'a', 'b', 'c', 'd', 'e', 'f'};

  ReadOnlyMemoryStream ms{memory.data(), memory.size()};

  ASSERT_EQ(0, ms.pos());
  ASSERT_EQ(size, ms.getMemorySize());
  ASSERT_EQ(size, ms.getSize());

  auto seek = [&ms](int64 iPos, int32 iMode, int64 iExpectedResult) {
    int64 res;
    ASSERT_EQ(kResultOk, ms.seek(iPos, iMode, &res));
    ASSERT_EQ(iExpectedResult, res);
  };

  std::array<int8, 5> str = {'a', 'b', 'c', 'd', 'e'};

  auto read = [&ms](int32 numBytes, std::vector<int8> iExpected) {
    int8 buf[numBytes];
    int32 numBytesRead = 0;
    ASSERT_EQ(kResultOk, ms.read(buf, numBytes, &numBytesRead));
    ASSERT_EQ(iExpected.size(), numBytesRead);
    ASSERT_EQ(iExpected, std::vector<int8>(&buf[0], &buf[numBytesRead]));
  };

  read(3, {'a', 'b', 'c'});
  ASSERT_EQ(3, ms.pos());
  ASSERT_EQ(size, ms.getMemorySize());
  ASSERT_EQ(size - 3, ms.getSize());

  seek(2, ReadOnlyMemoryStream::kIBSeekSet, 2);
  ASSERT_EQ(2, ms.pos());
  read(0, {});
  ASSERT_EQ(2, ms.pos());
  read(1, {'c'});
  ASSERT_EQ(3, ms.pos());
  seek(-3, ReadOnlyMemoryStream::kIBSeekCur, 0);
  read(2, {'a', 'b'});
  ASSERT_EQ(2, ms.pos());
  read(10, {'c', 'd', 'e', 'f'});
  ASSERT_EQ(6, ms.pos());
  read(10, {});
  ASSERT_EQ(6, ms.pos());
  seek(-3, ReadOnlyMemoryStream::kIBSeekEnd, 3);
  ASSERT_EQ(3, ms.pos());
  read(1, {'d'});
}

}