/*
 * Copyright (c) 2018-2019 pongasoft
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
#include <pongasoft/VST/AudioUtils.h>
#include <gtest/gtest.h>
#include <algorithm>

namespace pongasoft {
namespace VST {

// AudioUtils - bitManipulation
TEST(AudioUtils, bitManipulation) {
  ASSERT_EQ(0b00000000'00000000'00000000'00000001, bitSet(0b00000000'00000000'00000000'00000000, 0));
  ASSERT_EQ(0b00000000'00000000'00000000'00000011, bitSet(0b00000000'00000000'00000000'00000001, 1));
  ASSERT_EQ(0b00000000'00000000'00000000'00000011, bitSet(0b00000000'00000000'00000000'00000011, 1));
  ASSERT_EQ(0b00000000'00010000'10000000'00000001, bitSet(0b00000000'00000000'10000000'00000001, 20));
  ASSERT_EQ(0b00000000'00000000'10000000'00000001, bitClear(0b00000000'00010000'10000000'00000001, 20));
  ASSERT_EQ(0b00000000'00010000'10000000'00000001, bitClear(0b00000000'00010000'10000000'00000001, 21));
  ASSERT_FALSE(bitTest(0b00000000'00010000'10000000'00000001, 19));
  ASSERT_TRUE(bitTest(0b00000000'00010000'10000000'00000001, 20));
}

// AudioUtils - silentThreshold
TEST(AudioUtils, silentThreshold) {
  ASSERT_EQ(Sample32SilentThreshold, getSampleSilentThreshold<Sample32>());
  ASSERT_EQ(Sample64SilentThreshold, getSampleSilentThreshold<Sample64>());
}


}
}