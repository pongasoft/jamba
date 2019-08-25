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
#include <pongasoft/VST/GUI/Params/IGUIParameter.hpp>
#include <pongasoft/VST/GUI/Params/GUIValParameter.h>
#include <pongasoft/VST/GUI/Params/GUIOptionalParam.h>
#include <memory>

namespace pongasoft::VST::GUI::Params::TestGUIParameters {

struct TrivialStruct
{
  int fValue;
};

struct TrivialStructWithDiffOperator
{
  int fValue;

  bool operator==(const TrivialStructWithDiffOperator &rhs) const
  {
    return fValue == rhs.fValue;
  }

  bool operator!=(const TrivialStructWithDiffOperator &rhs) const
  {
    return !(rhs == *this);
  }
};

struct NoDefaultCtorStruct
{
  explicit NoDefaultCtorStruct(int iValue) : fValue{iValue}
  {}

  int fValue;
};

struct NoCopyCtorStruct
{
  NoCopyCtorStruct() = default;

  NoCopyCtorStruct(NoCopyCtorStruct const &) = delete;

  int fValue;
};

struct NoCopyAssignStruct
{
  NoCopyAssignStruct &operator=(NoCopyAssignStruct const &) = delete;

  int fValue;
};

// IGUIParameter - testCast
TEST(IGUIParameter, testCast)
{
  auto ref = std::make_shared<GUIValParameter<bool>>(1, false);

  std::shared_ptr<IGUIParameter> ptr = ref;

  ASSERT_EQ(1, ptr->getParamID());

  auto pb = ptr->cast<bool>();

  // cast should work!
  ASSERT_TRUE(pb.get() != nullptr);
  ASSERT_EQ(1, pb->getParamID());
  pb->accessValue([](auto const &iValue) {
    ASSERT_EQ(false, iValue);
  });

  // making sure that we get a pointer to the SAME object (ref)
  pb->setValue(true);
  pb->accessValue([](auto const &iValue) {
    ASSERT_EQ(true, iValue);
  });
  ASSERT_EQ(true, ref->getValue());
  ASSERT_EQ(ref.get(), pb.get()); // clearly shows it is the same pointer

  auto pi = ptr->cast<int>();
  // cast should NOT work
  ASSERT_TRUE(pi.get() == nullptr);
}

// IGUIParameter - testTypeRestrictions
TEST(GUIOptionalParam, testTypeRestrictions)
{
  // int / all primitives should work
  {
    GUIOptionalParam<int> p{};
    ASSERT_EQ(0, p.getValue());
    ASSERT_TRUE(p.update(10));
    ASSERT_EQ(10, p.getValue());
    ASSERT_EQ(kResultOk, p.setValue(20));
    ASSERT_EQ(20, p.getValue());
    ASSERT_FALSE(p.update(20));
    ASSERT_EQ(20, p.getValue());
  }

  // int + copy constructor
  {
    GUIOptionalParam<int> p{34};
    ASSERT_EQ(34, p.getValue());
  }

  // TrivialStruct should work (update always return true)
  {
    GUIOptionalParam<TrivialStruct> p{};
    ASSERT_EQ(0, p.getValue().fValue);
    ASSERT_TRUE(p.update(TrivialStruct{10}));
    ASSERT_EQ(10, p.getValue().fValue);
    ASSERT_EQ(kResultOk, p.setValue(TrivialStruct{20}));
    ASSERT_EQ(20, p.getValue().fValue);
    ASSERT_TRUE(p.update(TrivialStruct{20})); //no operator!= defined => update returns true
    ASSERT_EQ(20, p.getValue().fValue);
  }

  // TrivialStruct should work
  {
    GUIOptionalParam<TrivialStruct> p{TrivialStruct{34}};
    ASSERT_EQ(34, p.getValue().fValue);
  }

  // TrivialStructWithDiffOperator should work (update returns proper value since compare is possible)
  {
    GUIOptionalParam<TrivialStructWithDiffOperator> p{};
    ASSERT_EQ(0, p.getValue().fValue);
    ASSERT_TRUE(p.update(TrivialStructWithDiffOperator{10}));
    ASSERT_EQ(10, p.getValue().fValue);
    ASSERT_EQ(kResultOk, p.setValue(TrivialStructWithDiffOperator{20}));
    ASSERT_EQ(20, p.getValue().fValue);
    ASSERT_FALSE(p.update(TrivialStructWithDiffOperator{20}));
    ASSERT_EQ(20, p.getValue().fValue);
  }

  // NoDefaultCtorStruct => does not work
  {
    // THIS SHOULD NOT COMPILE (commented for obvious reasons)
    // GUIOptionalParam<NoDefaultCtorStruct> pInvalid{};
    // GUIOptionalParam<NoDefaultCtorStruct> p{NoDefaultCtorStruct{0}};
  }

  // NoCopyCtorStruct => does not work
  {
    // THIS SHOULD NOT COMPILE (commented for obvious reasons)
    //GUIOptionalParam<NoCopyCtorStruct> p{};
  }

  // NoCopyAssignStruct => does not work
  {
    // THIS SHOULD NOT COMPILE (commented for obvious reasons)
    // GUIOptionalParam<NoCopyAssignStruct> p{};
  }

}

}
