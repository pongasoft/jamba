/*
 * Copyright (c) 2018 pongasoft
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

#include <pongasoft/Utils/Lerp.h>
#include <gtest/gtest.h>
#include <cmath>

namespace pongasoft {
namespace Utils {
namespace Test {

// Lerp - mapValue
TEST(Lerp, mapValue)
{
  ASSERT_EQ(0, DPLerp::mapValue(-1.0, -1.0, 1.0, 0.0, 7.0));
  ASSERT_EQ(3.5, DPLerp::mapValue(0.0, -1.0, 1.0, 0.0, 7.0));
  ASSERT_EQ(7, DPLerp::mapValue(1.0, -1.0, 1.0, 0.0, 7.0));

  ASSERT_EQ(100, DPLerp::mapValue(10, 10, 20, 100, 200));
  ASSERT_EQ(150, DPLerp::mapValue(15, 10, 20, 100, 200));
  ASSERT_EQ(200, DPLerp::mapValue(20, 10, 20, 100, 200));

  // testing outside the range
  ASSERT_EQ(100, DPLerp::mapValue(5, 10, 20, 100, 200, true));
  ASSERT_EQ(50, DPLerp::mapValue(5, 10, 20, 100, 200, false));
  ASSERT_EQ(200, DPLerp::mapValue(25, 10, 20, 100, 200, true));
  ASSERT_EQ(250, DPLerp::mapValue(25, 10, 20, 100, 200, false));

  ASSERT_EQ(150, DPLerp::mapRange(10, 20, 100, 200).computeY(15));

}

}
}
}