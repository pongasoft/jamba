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
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/GUIController.h>
#include <memory>

namespace pongasoft::VST::GUI::Params::TestGUIParameters {

struct TrivialStruct
{
  int fValue{};

//  friend constexpr bool operator==(const TrivialStruct &lhs, const TrivialStruct &rhs);
//
//  friend constexpr bool operator<(const TrivialStruct &lhs, const TrivialStruct &rhs);
//
//  friend constexpr bool operator==(const TrivialStruct &lhs, const int &rhs);
//
//  friend constexpr bool operator<(const TrivialStruct &lhs, const int &rhs);
};

constexpr bool operator==(const TrivialStruct &lhs, const TrivialStruct &rhs)
{
  return lhs.fValue == rhs.fValue;
}

constexpr bool operator<(const TrivialStruct &lhs, const TrivialStruct &rhs)
{
  return lhs.fValue < rhs.fValue;
}

constexpr bool operator==(const TrivialStruct &lhs, const int &rhs)
{
  return lhs.fValue == rhs;
}

constexpr bool operator<(const TrivialStruct &lhs, const int &rhs)
{
  return lhs.fValue < rhs;
}

struct TrivialStructWithDiffOperator
{
  int fValue{};

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

  int fValue{};
};

struct NoCopyCtorStruct
{
  NoCopyCtorStruct() = default;

  NoCopyCtorStruct(NoCopyCtorStruct const &) = delete;

  int fValue{};
};

struct NoCopyAssignStruct
{
  NoCopyAssignStruct &operator=(NoCopyAssignStruct const &) = delete;

  int fValue{};
};

enum ParamIDs : ParamID {
  // vst
  kRawVst = 1000,
  kInt32Vst = 2000,
  kInt64Vst = 2001,
  kTrivialStructVst = 2002,
  // jmb
  kTrivialStructJmb = 2010,
};

constexpr int kInt32Vst_StepCount = 5; // [0 => 0.0, 1 => 0.2, 2 => 0.4, 3 => 0.6, 4 => 0.8, 5 => 1.0]
constexpr int kInt64Vst_StepCount = 10; // [0 => 0.0, 1 => 0.1, 2 => 0.2, ..., 10 => 1.0]

template<typename Struct>
class StructParamConverter : public IParamConverter<Struct>
{
public:
  Struct denormalize(ParamValue value) const override
  {
    auto s = Struct{};
    s.fValue = fConverter.denormalize(value);
    return s;
  }

  // normalize
  ParamValue normalize(Struct const &iStruct) const override
  {
    return fConverter.normalize(iStruct.fValue);
  }

private:
  DiscreteValueParamConverter<5> fConverter{};
};

//------------------------------------------------------------------------
// MyParameters
//------------------------------------------------------------------------
class MyParameters : public Parameters
{
public:
  RawVstParam fRawVst;
  VstParam<int32> fInt32Vst;
  VstParam<int64> fInt64Vst;
  VstParam<TrivialStruct> fTrivialStructVst;

  JmbParam<TrivialStruct> fTrivialStructJmb;

public:
  MyParameters()
  {
    fRawVst = raw(ParamIDs::kRawVst, STR16("rawVst")).add();

    // fInt32Vst
    fInt32Vst =
      vst<DiscreteValueParamConverter<kInt32Vst_StepCount, int32>>(ParamIDs::kInt32Vst, STR16("int32Vst"),
                                                                   STR16("int32Vst [%d]")) // converter argument
        .add();

    // fInt64Vst
    fInt64Vst =
      vst<DiscreteValueParamConverter<kInt64Vst_StepCount, int64>>(ParamIDs::kInt64Vst, STR16("int64Vst"),
                                                                   STR16("int64Vst [%d]")) // converter argument
        .add();

    // fTrivialStructVst
    fTrivialStructVst =
      vst<StructParamConverter<TrivialStruct>>(ParamIDs::kTrivialStructVst, STR16("trivialStructVst"))
        .add();

    fTrivialStructJmb = jmbFromType<TrivialStruct>(ParamIDs::kTrivialStructJmb, STR16("trivialStructJmb"))
      .transient()
      .add();

//    setGUISaveStateOrder(1, fInt32Vst);
  }

};

//------------------------------------------------------------------------
// MyGUIState
//------------------------------------------------------------------------
class MyGUIState : public GUIPluginState<MyParameters>
{
public:
  explicit MyGUIState(MyParameters const &iParams) :  GUIPluginState(iParams) {}
//public:
//  GUIJmbParam<int32> fInt32Jmb;
//
//public:
//  explicit MyGUIState(MyParameters const &iParams) :
//    GUIPluginState(iParams),
//    fInt32Jmb{add(iParams.fInt32Jmb)}
//  {};
};

//------------------------------------------------------------------------
// MyController
//------------------------------------------------------------------------
class MyController : public GUIController
{
public:
  // Constructor
  explicit MyController() :
    GUIController("JambaTestPlugin.uidesc"), fParams{}, fState{fParams}
  {
    // implementation note: this is only for testing! in real life scenario the host/DAW is the one
    // instantiating the controller and calling initialize with a host context
    GUIController::initialize(nullptr);
  }

  GUIRawVstParam rawVstParam()
  {
    auto param = getGUIState()->getRawVstParameter(ParamIDs::kRawVst);
    return GUIRawVstParam(param);

  }

  GUIVstParam<int32> int32VstParam()
  {
    auto param = getGUIState()->getGUIVstParameter<int32>(ParamIDs::kInt32Vst);
    return GUIVstParam<int32>(param);
  }

  GUIVstParam<int64> int64VstParam()
  {
    auto param = getGUIState()->getGUIVstParameter<int64>(ParamIDs::kInt64Vst);
    return GUIVstParam<int64>(param);
  }

  GUIVstParam<TrivialStruct> trivialStructVstParam()
  {
    auto param = getGUIState()->getGUIVstParameter<TrivialStruct>(ParamIDs::kTrivialStructVst);
    return GUIVstParam<TrivialStruct>(param);
  }

  GUIJmbParam<TrivialStruct> trivialStructJmbParam()
  {
    auto param = getGUIState()->findParam(ParamIDs::kTrivialStructJmb);
    return GUIJmbParam<TrivialStruct>(castToJmb<TrivialStruct>(param));
  }


  // getGUIState
  GUIState *getGUIState() override { return &fState; }

  MyParameters fParams;
  MyGUIState fState;
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

// GUIOptionalParam - testTypeRestrictions
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

// GUIParam - testOperators
TEST(GUIParam, testOperators)
{
  MyController c{};

  auto rawVstParam = c.rawVstParam();
  auto int32VstParam = c.int32VstParam();
  auto int64VstParam = c.int64VstParam();
  auto trivialStructVstParam = c.trivialStructVstParam();
  auto trivialStructJmbParam = c.trivialStructJmbParam();

  ASSERT_EQ(0, rawVstParam.value());
  ASSERT_EQ(0, int32VstParam.value());
  ASSERT_EQ(0, int64VstParam.value());
  ASSERT_EQ(0, trivialStructVstParam.value().fValue);
  ASSERT_EQ(0, trivialStructJmbParam.value().fValue);

  int32VstParam = 1;
  ASSERT_EQ(1, int32VstParam.getValue());
  ASSERT_EQ(1, int32VstParam.value());
  ASSERT_EQ(1, *int32VstParam);
  ASSERT_EQ(0.2, int32VstParam.getNormalizedValue());

  rawVstParam = 0.6;
  ASSERT_EQ(0.6, rawVstParam.getValue());
  ASSERT_EQ(0.6, rawVstParam.value());
  ASSERT_EQ(0.6, *rawVstParam);

  int32VstParam.copyValueFrom(rawVstParam);
  ASSERT_EQ(3, int32VstParam.value());
  ASSERT_EQ(0.6, int32VstParam.getNormalizedValue());
  ASSERT_TRUE(int32VstParam == 3);
  ASSERT_TRUE(int32VstParam < 4);
  ASSERT_TRUE(int32VstParam <= 3);
  ASSERT_TRUE(int32VstParam > 2);
  ASSERT_TRUE(int32VstParam >= 3);
  ASSERT_TRUE(3 == int32VstParam);
  ASSERT_TRUE(2 < int32VstParam);
  ASSERT_TRUE(3 <= int32VstParam);
  ASSERT_TRUE(4 > int32VstParam);
  ASSERT_TRUE(3 >= int32VstParam);

  auto copy = c.int32VstParam();
  ASSERT_TRUE(int32VstParam == copy);
  ASSERT_FALSE(int32VstParam != copy);


  int64VstParam.copyValueFrom(int32VstParam);
  ASSERT_EQ(6, int64VstParam.value());
  ASSERT_EQ(0.6, int64VstParam.getNormalizedValue());

  // int32 x = int32VstParam; // deprecated

  // TrivialStruct x = trivialStructVstParam; // deprecated

  trivialStructVstParam.copyValueFrom(int32VstParam);
  ASSERT_EQ(3, trivialStructVstParam.value().fValue);
  ASSERT_EQ(3, (*trivialStructVstParam).fValue);
  ASSERT_EQ(3, trivialStructVstParam->fValue); // optimized syntax!
  ASSERT_TRUE(trivialStructVstParam == TrivialStruct{3});
  ASSERT_FALSE(trivialStructVstParam == TrivialStruct{4});
  ASSERT_TRUE(trivialStructVstParam < TrivialStruct{4});
  ASSERT_FALSE(trivialStructVstParam < TrivialStruct{3});
  ASSERT_FALSE(trivialStructVstParam < TrivialStruct{2});
  ASSERT_TRUE(trivialStructVstParam <= TrivialStruct{3});
  ASSERT_FALSE(trivialStructVstParam <= TrivialStruct{2});
  ASSERT_TRUE(trivialStructVstParam > TrivialStruct{2});
  ASSERT_FALSE(trivialStructVstParam > TrivialStruct{3});
  ASSERT_FALSE(trivialStructVstParam > TrivialStruct{4});
  ASSERT_TRUE(trivialStructVstParam >= TrivialStruct{3});
  ASSERT_FALSE(trivialStructVstParam >= TrivialStruct{4});
  ASSERT_TRUE(TrivialStruct{3} == trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{4} == trivialStructVstParam);
  ASSERT_TRUE(TrivialStruct{2} < trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{3} < trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{4} < trivialStructVstParam);
  ASSERT_TRUE(TrivialStruct{3} <= trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{4} <= trivialStructVstParam);
  ASSERT_TRUE(TrivialStruct{4} > trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{3} > trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{2} > trivialStructVstParam);
  ASSERT_TRUE(TrivialStruct{3} >= trivialStructVstParam);
  ASSERT_FALSE(TrivialStruct{2} >= trivialStructVstParam);
  ASSERT_TRUE(trivialStructVstParam == 3);
  ASSERT_FALSE(trivialStructVstParam == 4);
  ASSERT_TRUE(trivialStructVstParam < 4);
  ASSERT_FALSE(trivialStructVstParam < 3);
  ASSERT_FALSE(trivialStructVstParam < 2);
  ASSERT_TRUE(trivialStructVstParam <= 3);
  ASSERT_FALSE(trivialStructVstParam <= 2);
  ASSERT_TRUE(trivialStructVstParam > 2);
  ASSERT_FALSE(trivialStructVstParam > 3);
  ASSERT_FALSE(trivialStructVstParam > 4);
  ASSERT_TRUE(trivialStructVstParam >= 3);
  ASSERT_FALSE(trivialStructVstParam >= 4);
  ASSERT_TRUE(3 == trivialStructVstParam);
  ASSERT_FALSE(4 == trivialStructVstParam);
  ASSERT_TRUE(2 < trivialStructVstParam);
  ASSERT_FALSE(3 < trivialStructVstParam);
  ASSERT_FALSE(4 < trivialStructVstParam);
  ASSERT_TRUE(3 <= trivialStructVstParam);
  ASSERT_FALSE(4 <= trivialStructVstParam);
  ASSERT_TRUE(4 > trivialStructVstParam);
  ASSERT_FALSE(3 > trivialStructVstParam);
  ASSERT_FALSE(2 > trivialStructVstParam);
  ASSERT_TRUE(3 >= trivialStructVstParam);
  ASSERT_FALSE(2 >= trivialStructVstParam);

  // TrivialStruct x = trivialStructJmbParam; // deprecated

  trivialStructJmbParam = TrivialStruct{4};
  ASSERT_EQ(4, trivialStructJmbParam.value().fValue);
  ASSERT_EQ(4, (*trivialStructJmbParam).fValue);
  ASSERT_EQ(4, trivialStructJmbParam->fValue);
}

}
