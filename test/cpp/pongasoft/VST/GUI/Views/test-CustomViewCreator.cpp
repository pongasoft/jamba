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
#include <pongasoft/VST/GUI/Views/CustomViewCreator.h>
#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

enum MyEnum
{
  kUndefined,
  kMyEnum0,
  kMyEnum1
};

class MyView : public CustomView {
public:
  MyView(const CRect &iSize) : CustomView(iSize) {}

  MyEnum getVal() const
  {
    return fVal;
  }

  void setVal(MyEnum iVal)
  {
    fVal = iVal;
  }

  MyEnum fVal{kUndefined};

public:
  class Creator : public CustomViewCreator<MyView, CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerListAttribute<MyEnum>("my-enum", &MyView::getVal, &MyView::setVal,
                                    {
                                      {"enum-0", MyEnum::kMyEnum0},
                                      {"enum-1", MyEnum::kMyEnum1}
                                    });
    };
  };
};


// CustomViewCreator - listAttribute
TEST(CustomViewCreator, listAttribute) {
  MyView::Creator creator{"testViewName", "testViewDisplayName"};

  MyView v{CRect{0, 0, 100, 100}};

  // val => kUndefined
  ASSERT_EQ(MyEnum::kUndefined, v.getVal());

  UIAttributes attributes;
  attributes.setAttribute("my-enum", "enum-1");

  // val => kMyEnum1
  ASSERT_TRUE(creator.apply(&v, attributes, nullptr));
  ASSERT_EQ(MyEnum::kMyEnum1, v.getVal());

  // toString => "enum-1"
  std::string value;
  ASSERT_TRUE(creator.getAttributeValue(&v, "my-enum", value, nullptr));
  ASSERT_EQ(value, "enum-1");

  // setting to invalid string should fail
  attributes.setAttribute("my-enum", "invalid");
  ASSERT_TRUE(creator.apply(&v, attributes, nullptr));
  ASSERT_EQ(MyEnum::kMyEnum1, v.getVal());

  // although kUndefined is a valid enum entry it was not provided in the list of attributes => should
  // return false
  v.setVal(MyEnum::kUndefined);
  ASSERT_FALSE(creator.getAttributeValue(&v, "my-enum", value, nullptr));
  ASSERT_EQ(MyEnum::kUndefined, v.getVal());

  std::list<std::string const *> possibleValues{};
#ifndef NDEBUG
  ASSERT_TRUE(creator.getPossibleListValues("my-enum", possibleValues));
  ASSERT_EQ(2, possibleValues.size());
  ASSERT_EQ("enum-0", *possibleValues.front());
  possibleValues.pop_front();
  ASSERT_EQ("enum-1", *possibleValues.front());
  possibleValues.pop_front();
#else
  ASSERT_FALSE(creator.getPossibleListValues("my-enum", possibleValues));
#endif
}

}
}
}
}