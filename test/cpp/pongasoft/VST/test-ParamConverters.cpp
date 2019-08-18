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

// ParamConverters - testDiscreteTypeParamConverter
TEST(ParamConverters, testDiscreteTypeParamConverter) {
  struct TestStruct {
    float fMyVal;

    bool operator<(const TestStruct &o)  const {
      return fMyVal < o.fMyVal;
    }

    bool operator==(const TestStruct &rhs) const
    {
      return fMyVal == rhs.fMyVal;
    }

    bool operator!=(const TestStruct &rhs) const
    {
      return !(rhs == *this);
    }
  };

  auto values =
    std::array<TestStruct, 5>{ TestStruct{0.9}, TestStruct{1.4}, TestStruct{-4}, TestStruct{50}, TestStruct{1} };

  DiscreteTypeParamConverter<TestStruct> converter{
    {values[0], STR16("val0")},
    {values[1], STR16("val1")},
    {values[2], STR16("val2")},
    {values[3], STR16("val3")},
    {values[4], STR16("val4")}
  };

  // stepCount
  ASSERT_EQ(4, converter.getStepCount());

  // test normalize
  ASSERT_EQ(converter.normalize(values[0]), 0);
  ASSERT_EQ(converter.normalize(values[1]), 0.25);
  ASSERT_EQ(converter.normalize(values[2]), 0.50);
  ASSERT_EQ(converter.normalize(values[3]), 0.75);
  ASSERT_EQ(converter.normalize(values[4]), 1.0);

  // invalid struct (warning + return 0)
  ASSERT_EQ(converter.normalize(TestStruct{100}), 0);

  // test denormalize
  ASSERT_EQ(converter.denormalize(0),    values[0]);
  ASSERT_EQ(converter.denormalize(0.25), values[1]);
  ASSERT_EQ(converter.denormalize(0.5),  values[2]);
  ASSERT_EQ(converter.denormalize(0.75), values[3]);
  ASSERT_EQ(converter.denormalize(1.0),  values[4]);

  // out of range
  ASSERT_EQ(converter.denormalize(-2.0), values[0]);
  ASSERT_EQ(converter.denormalize(1.2),  values[4]);
  ASSERT_EQ(converter.denormalize(0.29), values[1]);

  // test toString
  String128 s;
  converter.toString(values[0], s, 2);
  ASSERT_EQ(VstString16(STR16("val0")), VstString16(s));
  converter.toString(values[1], s, 2);
  ASSERT_EQ(VstString16(STR16("val1")), VstString16(s));
  converter.toString(values[2], s, 2);
  ASSERT_EQ(VstString16(STR16("val2")), VstString16(s));
  converter.toString(values[3], s, 2);
  ASSERT_EQ(VstString16(STR16("val3")), VstString16(s));
  converter.toString(values[4], s, 2);
  ASSERT_EQ(VstString16(STR16("val4")), VstString16(s));

  s[0] = 0;
  converter.toString(TestStruct{100}, s, 2);
  ASSERT_EQ(VstString16(STR16("")), VstString16(s));

  // should be valid for an enum class (and only a subset of the values)
  enum class MyEnum {
    kVal0 = 90,
    kVal1 = 140,
    kVal2 = -40,
    kVal3 = 500,
    kVal4 = 10,
  };

  DiscreteTypeParamConverter<MyEnum> ec {
    { MyEnum::kVal4, STR16("val4")},
    { MyEnum::kVal1, STR16("val1")}
  };

  // stepCount
  ASSERT_EQ(1, ec.getStepCount());

  // test normalize
  ASSERT_EQ(ec.normalize(MyEnum::kVal4), 0);
  ASSERT_EQ(ec.normalize(MyEnum::kVal1), 1.0);

  // invalid for this converter
  ASSERT_EQ(ec.normalize(MyEnum::kVal3), 0.0);

}

}
}