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

namespace pongasoft {
namespace Utils {
namespace Test {

// Lerp - mapValue
TEST(Lerp, mapValue)
{
  ASSERT_EQ(0, Lerp<double>::mapValue(-1, -1, 1, 0, 7));
  ASSERT_EQ(3.5, Lerp<double>::mapValue(0.0, -1, 1, 0, 7));
  ASSERT_EQ(7, Lerp<double>::mapValue(1, -1, 1, 0, 7));

  ASSERT_EQ(100, Lerp<double>::mapValue(10, 10, 20, 100, 200));
  ASSERT_EQ(150, Lerp<double>::mapValue(15, 10, 20, 100, 200));
  ASSERT_EQ(200, Lerp<double>::mapValue(20, 10, 20, 100, 200));

  // testing outside the range
  ASSERT_EQ(100, Lerp<double>::mapValue(5, 10, 20, 100, 200));
  ASSERT_EQ(50, Lerp<double>::mapValueX(5, 10, 20, 100, 200));
  ASSERT_EQ(200, Lerp<double>::mapValue(25, 10, 20, 100, 200));
  ASSERT_EQ(250, Lerp<double>::mapValueX(25, 10, 20, 100, 200));


  ASSERT_EQ(150, Lerp<double>::mapRange(10, 20, 100, 200).computeY(15));

}

}
}
}