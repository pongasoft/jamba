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
#include <pongasoft/VST/ParamConverters.h>
#include <gtest/gtest.h>

namespace pongasoft {
namespace VST {

// ParamConverters - testEnumParamConverter
TEST(ParamConverters, testEnumParamConverter) {
  enum TestEnum
  {
    kValue1,
    kValue2,
    kValue3,
    kValue4,
    kValue5
  };

  EnumParamConverter<TestEnum, TestEnum::kValue5> converter;

  ASSERT_EQ(4, converter.getStepCount());

  ASSERT_EQ(converter.normalize(TestEnum::kValue1), 0);
  ASSERT_EQ(converter.normalize(TestEnum::kValue2), 0.25);
  ASSERT_EQ(converter.normalize(TestEnum::kValue3), 0.50);
  ASSERT_EQ(converter.normalize(TestEnum::kValue4), 0.75);
  ASSERT_EQ(converter.normalize(TestEnum::kValue5), 1.0);

  ASSERT_EQ(converter.denormalize(0), TestEnum::kValue1);
  ASSERT_EQ(converter.denormalize(0.25), TestEnum::kValue2);
  ASSERT_EQ(converter.denormalize(0.5), TestEnum::kValue3);
  ASSERT_EQ(converter.denormalize(0.75), TestEnum::kValue4);
  ASSERT_EQ(converter.denormalize(1.0), TestEnum::kValue5);

  ASSERT_EQ("0", converter.toString(TestEnum::kValue1, 0));
  ASSERT_EQ("1", converter.toString(TestEnum::kValue2, 0));
  ASSERT_EQ("2", converter.toString(TestEnum::kValue3, 0));
  ASSERT_EQ("3", converter.toString(TestEnum::kValue4, 0));
  ASSERT_EQ("4", converter.toString(TestEnum::kValue5, 0));

}

}
}