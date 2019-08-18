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
#include <pongasoft/VST/Parameters.h>

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

  // make sure the syntax is "nice" for parameter building
  Parameters p{};

  // invoke default constructor
  auto b = p.vst<EnumParamConverter<TestEnum, TestEnum::kValue5>>(100, STR16("test"));
  ASSERT_EQ("0", b.fConverter->toString(TestEnum::kValue1, 0));
  ASSERT_EQ("4", b.fConverter->toString(TestEnum::kValue5, 0));

  b = p.vst<EnumParamConverter<TestEnum, TestEnum::kValue5>>(100, STR16("test"), 1);
  ASSERT_EQ("1", b.fConverter->toString(TestEnum::kValue1, 0));
  ASSERT_EQ("5", b.fConverter->toString(TestEnum::kValue5, 0));

  b = p.vst<EnumParamConverter<TestEnum, TestEnum::kValue5>>(100, STR16("test"), STR16("Page %d"));
  ASSERT_EQ("Page 0", b.fConverter->toString(TestEnum::kValue1, 0));
  ASSERT_EQ("Page 4", b.fConverter->toString(TestEnum::kValue5, 0));

  b = p.vst<EnumParamConverter<TestEnum, TestEnum::kValue5>>(100, STR16("test"), STR16("Page %d"), 1);
  ASSERT_EQ("Page 1", b.fConverter->toString(TestEnum::kValue1, 0));
  ASSERT_EQ("Page 5", b.fConverter->toString(TestEnum::kValue5, 0));

  // no need to use std::array anymore!
  b = p.vst<EnumParamConverter<TestEnum, TestEnum::kValue5>>(100, STR16("test"),
                                                             {STR16("v0"),
                                                              STR16("v1"),
                                                              STR16("v2"),
                                                              STR16("v3"),
                                                              STR16("v4")});
  ASSERT_EQ("v0", b.fConverter->toString(TestEnum::kValue1, 0));
  ASSERT_EQ("v4", b.fConverter->toString(TestEnum::kValue5, 0));
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
  ASSERT_EQ("val0", converter.toString(values[0], 2));
  ASSERT_EQ("val1", converter.toString(values[1], 2));
  ASSERT_EQ("val2", converter.toString(values[2], 2));
  ASSERT_EQ("val3", converter.toString(values[3], 2));
  ASSERT_EQ("val4", converter.toString(values[4], 2));

  ASSERT_EQ("", converter.toString(TestStruct{100}, 2));

  // make sure the syntax is "nice" for parameter building
  Parameters p{};
  auto b = p.vst<DiscreteTypeParamConverter<TestStruct>>(100, STR16("test"),
                                                         {
                                                           {values[0], STR16("val0")},
                                                           {values[1], STR16("val1")},
                                                           {values[2], STR16("val2")},
                                                           {values[3], STR16("val3")},
                                                           {values[4], STR16("val4")}
                                                         });
  ASSERT_EQ("val0", b.fConverter->toString(values[0], 2));
  ASSERT_EQ("val4", b.fConverter->toString(values[4], 2));


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