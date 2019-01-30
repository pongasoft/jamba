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
#include <pongasoft/VST/SampleRateBasedClock.h>
#include <gtest/gtest.h>

namespace pongasoft {
namespace VST {

// SampleRateBasedClock - testNextBarSampleCount
TEST(SampleRateBasedClock, testNextBarSampleCount) {
  SampleRateBasedClock clock(48000);

  auto oneBar = clock.getSampleCountFor1Bar(120);

  ASSERT_EQ(oneBar, clock.getSampleCountFor1Bar(120));

  ASSERT_EQ(oneBar, clock.getNextBarSampleCount(52123, 120));
  ASSERT_EQ(7 * oneBar, clock.getNextBarSampleCount(600000, 120));
  ASSERT_EQ(7 * oneBar, clock.getNextBarSampleCount(672000, 120));
  ASSERT_EQ(8 * oneBar, clock.getNextBarSampleCount(672001, 120));

}

}
}