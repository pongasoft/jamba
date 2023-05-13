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

#include <pongasoft/Utils/StringUtils.h>
#include <cmath>
#include <gtest/gtest.h>

namespace pongasoft {
namespace Utils {
namespace Test {

// StringUtils - splitString
TEST(StringUtils, splitString)
{
  using VString = std::vector<std::string>;
  VString expected;

  expected = VString{"abc", "def", "g"};
  ASSERT_EQ(expected, splitString("abc:def:g", ':'));

  expected = VString{"abc", "def", "g"};
  ASSERT_EQ(expected, splitString(":abc::def::g:", ':', true));

  expected = VString{};
  ASSERT_EQ(expected, splitString(":", ':', true));

  expected = VString{"", ""};
  ASSERT_EQ(expected, splitString(":", ':', false));

  expected = VString{"", "abc", "", "def", "", "g", ""};
  ASSERT_EQ(expected, splitString(":abc::def::g:", ':', false));

  expected = VString{"abc:def:g"};
  ASSERT_EQ(expected, splitString("abc:def:g", ','));
}

// StringUtils - stringToFloat
TEST(StringUtils, stringToFloat)
{
  double d = -12;

  ASSERT_TRUE(stringToFloat("5.6", d));
  ASSERT_EQ(5.6, d);
  ASSERT_EQ(5.6, stringToFloat<double>("5.6"));

  ASSERT_TRUE(stringToFloat("-9.1", d));
  ASSERT_EQ(-9.1, d);
  ASSERT_EQ(-9.1, stringToFloat<double>("-9.1"));

  ASSERT_TRUE(stringToFloat("  4.2  ", d));
  ASSERT_EQ(4.2, d);
  ASSERT_EQ(4.2, stringToFloat<double>("  4.2  "));

  ASSERT_FALSE(stringToFloat("abc", d));
  ASSERT_EQ(4.2, d);  // should be untouched

  ASSERT_EQ(0, stringToFloat<double>("abc"));
}

}
}
}