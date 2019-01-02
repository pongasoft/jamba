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
  ASSERT_EQ(0, mapValueDP(-1.0, -1.0, 1.0, 0.0, 7.0));
  ASSERT_EQ(3.5, mapValueDP(0.0, -1.0, 1.0, 0.0, 7.0));
  ASSERT_EQ(7, mapValueDP(1.0, -1.0, 1.0, 0.0, 7.0));

  ASSERT_EQ(100, (mapValueDPXY<int, int>(10, 10, 20, 100, 200)));
  ASSERT_EQ(150, mapValueDPX<int>(15, 10, 20, 100.0, 200.0));
  ASSERT_EQ(200, mapValueDPY<int>(20.0, 10.0, 20.0, 100, 200));

  // testing outside the range
  ASSERT_EQ(100, mapValueDP(5, 10, 20, 100, 200, true));
  ASSERT_EQ(50, mapValueDP(5, 10, 20, 100, 200, false));
  ASSERT_EQ(200, mapValueDP(25, 10, 20, 100, 200, true));
  ASSERT_EQ(250, mapValueDP(25, 10, 20, 100, 200, false));

  ASSERT_EQ(150, mapRangeDP(10, 20, 100, 200).computeY(15));


  ASSERT_EQ(5, mapRangeSPY<int>(1.2, 1.3, 0, 10).computeY(1.25));
  ASSERT_EQ(1.25, mapRangeSPX<int>(0, 10, 1.2, 1.3).computeY(5));
  // note: the extra set of () is required for google test to compile...
  // seems to be an issue with the macro definition
  ASSERT_EQ(25, (mapRangeSPXY<int, int>(0, 10, 20, 30).computeY(5)));
  ASSERT_EQ(25.0, mapRangeSP(0, 10.0, 20.0, 30.0).computeY(5.0));

}

}
}
}