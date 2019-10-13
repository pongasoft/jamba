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
#include <pongasoft/VST/GUI/Params/ParamAware.hpp>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/GUIController.h>
#include <stdexcept>

namespace pongasoft::VST::GUI::Params::TestParamAware {

enum ParamIDs : ParamID {
  kRawVst = 1000,
  kInt64Vst = 2000,
  kInt32Jmb = 3000,
  kInvalid = 666
};


constexpr int kInt64Vst_StepCount = 5; // [0 => 0.0, 1 => 0.2, 2 => 0.4, 3 => 0.6, 4 => 0.8, 5 => 1.0]

//------------------------------------------------------------------------
// MyParameters
//------------------------------------------------------------------------
class MyParameters : public Parameters
{
public:
  RawVstParam fRawVst;
  VstParam<int64> fInt64Vst;
  JmbParam<int32> fInt32Jmb;

public:
  MyParameters()
  {
    fRawVst =
      raw(ParamIDs::kRawVst, STR16("rawVst")).add();

    // fInt64Vst
    fInt64Vst =
      vst<DiscreteValueParamConverter<kInt64Vst_StepCount, int64>>(ParamIDs::kInt64Vst, STR16("int64Vst"),
                                                                   STR16("int64Vst [%d]")) // converter argument
        .add();

    fInt32Jmb = jmbFromType<int32>(ParamIDs::kInt32Jmb, STR16("int32Jmb"))
      .serializer<Int32ParamSerializer>()
      .add();

    setGUISaveStateOrder(1, fInt32Jmb);
  }
};

//------------------------------------------------------------------------
// MyGUIState
//------------------------------------------------------------------------
class MyGUIState : public GUIPluginState<MyParameters>
{
public:
  GUIJmbParam<int32> fInt32Jmb;

public:
  explicit MyGUIState(MyParameters const &iParams) :
    GUIPluginState(iParams),
    fInt32Jmb{add(iParams.fInt32Jmb)}
  {};
};

//------------------------------------------------------------------------
// MyController
//------------------------------------------------------------------------
class MyController : public GUIController, public ParamAware
{
public:
  enum class CallbackType
  {
    kListener,
    kChangeCallback,
    kChangeCallback1
  };

public:
  // Constructor
  explicit MyController() :
    GUIController("JambaTestPlugin.uidesc"), fParams{}, fState{fParams}
    {
      // implementation note: this is only for testing! in real life scenario the host/DAW is the one
      // instantiating the controller and calling initialize with a host context
      initialize(nullptr);
    }

  // getGUIState
  GUIState *getGUIState() override { return &fState; }

  Parameters::ChangeCallback changeCallback(ParamID iParamID) {
    if(iParamID != UNDEFINED_PARAM_ID)
    {
      fExpectedCallback = CallbackType::kChangeCallback;
      return [this, iParamID] () {
        CHECK_EQ_F(CallbackType::kChangeCallback, fExpectedCallback);
        fCallbacks.emplace_back(iParamID);
      };
    }
    return []() {
      ABORT_F("should not be called!");
    };
  }

  template<typename Param>
  Parameters::ChangeCallback1<Param> changeCallback1(ParamID iParamID) {
    if(iParamID != UNDEFINED_PARAM_ID)
    {
      fExpectedCallback = CallbackType::kChangeCallback1;
      return [this, iParamID] (Param &iParam) {
        CHECK_EQ_F(iParamID, iParam.getParamID());
        CHECK_EQ_F(CallbackType::kChangeCallback1, fExpectedCallback);
        fCallbacks.emplace_back(iParamID);
      };
    }
    return [](Param &iParam) {
      ABORT_F("should not be called!");
    };
  }

  void onParameterChange(ParamID iParamID) override
  {
    CHECK_EQ_F(CallbackType::kListener, fExpectedCallback);
    fCallbacks.emplace_back(iParamID);
  }

  // reset
  inline void reset()
  {
    unregisterAll();
    raw(0);
    vst(0);
    jmb(0);
    fCallbacks.clear();
    fExpectedCallback = CallbackType::kListener;
  }

  // empty
  inline bool empty() const { return fCallbacks.empty(); };

  ParamValue raw()
  {
    return getGUIState()->getRawVstParameter(ParamIDs::kRawVst)->getValue();
  }

  bool raw(ParamValue iValue)
  {
    return getGUIState()->getRawVstParameter(ParamIDs::kRawVst)->update(iValue);
  }


  int64 vst()
  {
    auto param = getGUIState()->findParam(ParamIDs::kInt64Vst)->cast<int64>();
    int64 res;
    EXPECT_TRUE(kResultOk == param->accessValue([&res](int64 const &iValue) {res = iValue;}));
    return res;
  }

  bool vst(int64 iValue)
  {
    auto param = getGUIState()->findParam(ParamIDs::kInt64Vst)->cast<int64>();
    return param->update(iValue);
  }

  int32 jmb()
  {
    auto param = getGUIState()->findParam(ParamIDs::kInt32Jmb)->cast<int32>();
    int32 res;
    EXPECT_TRUE(kResultOk == param->accessValue([&res](int32 const &iValue) {res = iValue;}));
    return res;
  }

  bool jmb(int32 iValue)
  {
    auto param = getGUIState()->findParam(ParamIDs::kInt32Jmb)->cast<int32>();
    return param->update(iValue);
  }

  MyParameters fParams;
  MyGUIState fState;
  std::vector<ParamID> fCallbacks{};
  CallbackType fExpectedCallback{CallbackType::kListener};
};

//class HostContext : public FUnknown
//{
//
//};

#define CHECK(c, id) ASSERT_EQ(c.fCallbacks, std::vector<ParamID>{static_cast<ParamID>(id)}); \
                     c.fCallbacks.clear()

#define CHECK_EMPTY(c) ASSERT_TRUE(c.empty())

//------------------------------------------------------------------------
// ParamAware - testSetup
// Make sure that the setup works properly (controller initialization,
// accessing parameters, changing their value, etc...)
//------------------------------------------------------------------------
TEST(ParamAware, testSetup)
{
  MyController c{};

  CHECK_EMPTY(c);

  ASSERT_EQ(0, c.vst());
  ASSERT_EQ(0, c.jmb());

  c.vst(3);
  ASSERT_EQ(3, c.vst());
  ASSERT_EQ(0, c.jmb());
  CHECK_EMPTY(c);

  c.jmb(7);
  ASSERT_EQ(3, c.vst());
  ASSERT_EQ(7, c.jmb());
  CHECK_EMPTY(c);
}

//------------------------------------------------------------------------
// ParamAware - Base Param
//------------------------------------------------------------------------
TEST(ParamAware, testBaseParam)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerBaseParam
  //------------------------------------------------------------------------
  IGUIParam param = c.registerBaseParam(ParamIDs::kInt64Vst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ("int64Vst [0]", param.toUTF8String(2));

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ("int64Vst [3]", param.toUTF8String(2));

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  // now register but not the callback
  param = c.registerBaseParam(ParamIDs::kInt64Vst, false);
  c.vst(2);
  CHECK_EMPTY(c);
  ASSERT_EQ("int64Vst [2]", param.toUTF8String(2));

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerBaseParam(ParamIDs::kInvalid);
  ASSERT_FALSE(param.exists());

  //------------------------------------------------------------------------
  // registerBaseCallback / Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerBaseCallback(ParamIDs::kInt32Jmb,
                                 c.changeCallback(ParamIDs::kInt32Jmb),
                                 true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getStepCount());
  ASSERT_EQ("0", param.toUTF8String(2));

  c.unregisterAll();

  param = c.registerBaseCallback(ParamIDs::kInt32Jmb,
                                 c.changeCallback(ParamIDs::kInt32Jmb),
                                 false);
  CHECK_EMPTY(c);
  c.jmb(8);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ("8", param.toUTF8String(2));

  c.unregisterAll();

  // not a valid param
  param = c.registerBaseCallback(ParamIDs::kInvalid,
                                 c.changeCallback(ParamIDs::kInvalid),
                                 true);
  CHECK_EMPTY(c);
  ASSERT_FALSE(param.exists());

  //------------------------------------------------------------------------
  // registerBaseCallback / Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerBaseCallback(ParamIDs::kInt32Jmb,
                                 c.changeCallback1<IGUIParam>(ParamIDs::kInt32Jmb),
                                 true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getStepCount());
  ASSERT_EQ("0", param.toUTF8String(2));

  c.unregisterAll();

  param = c.registerBaseCallback(ParamIDs::kInt32Jmb,
                                 c.changeCallback1<IGUIParam>(ParamIDs::kInt32Jmb),
                                 false);
  CHECK_EMPTY(c);
  c.jmb(7);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ("7", param.toUTF8String(2));

  c.unregisterAll();

  // not a valid param
  param = c.registerBaseCallback(ParamIDs::kInvalid,
                                 c.changeCallback1<IGUIParam>(ParamIDs::kInvalid),
                                 true);
  CHECK_EMPTY(c);
  ASSERT_FALSE(param.exists());
}

//------------------------------------------------------------------------
// ParamAware - Optional Param
//------------------------------------------------------------------------
TEST(ParamAware, testOptionalParam)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerOptionalParam - vst
  //------------------------------------------------------------------------
  GUIOptionalParam<int64> paramVst = c.registerOptionalParam<int64>(ParamIDs::kInt64Vst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalParam<int64>(ParamIDs::kInt64Vst, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => listener NOT called
  c.vst(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  //------------------------------------------------------------------------
  // registerOptionalParam - jmb
  //------------------------------------------------------------------------
  c.reset();

  GUIOptionalParam<int32> paramJmb = c.registerOptionalParam<int32>(ParamIDs::kInt32Jmb);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt32Jmb, paramJmb.getParamID());
  ASSERT_TRUE(paramJmb.exists());
  ASSERT_EQ(0, paramJmb.getStepCount());
  ASSERT_EQ(0, paramJmb.getValue());

  // changing jmb value => listener called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, paramJmb.getValue());

  ASSERT_TRUE(paramJmb.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, paramJmb.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing jmb value => listener NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramJmb = c.registerOptionalParam<int32>(ParamIDs::kInt32Jmb, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramJmb.getValue());

  // changing jmb value => listener NOT called
  c.jmb(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, paramJmb.getValue());

  ASSERT_TRUE(paramJmb.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramJmb.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerOptionalParam - not valid => optional kicks in
  //------------------------------------------------------------------------
  c.reset();

  GUIOptionalParam<int32> paramVal = c.registerOptionalParam<int32>(ParamIDs::kInvalid);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInvalid, paramVal.getParamID());
  ASSERT_TRUE(paramVal.exists());
  ASSERT_EQ(0, paramVal.getStepCount());
  ASSERT_EQ(0, paramVal.getValue());

  c.vst(2);
  c.jmb(3);
  CHECK_EMPTY(c);

  ASSERT_TRUE(paramVal.update(5));
  CHECK(c, ParamIDs::kInvalid);
  ASSERT_EQ(5, paramVal.getValue());


  //------------------------------------------------------------------------
  // registerOptionalCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();
  paramVst = c.registerOptionalCallback<int64>(ParamIDs::kInt64Vst,
                                               c.changeCallback(ParamIDs::kInt64Vst),
                                               true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalCallback<int64>(ParamIDs::kInt64Vst,
                                               c.changeCallback(ParamIDs::kInt64Vst),
                                               false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  //------------------------------------------------------------------------
  // registerOptionalCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();
  paramVst = c.registerOptionalCallback<int64>(ParamIDs::kInt64Vst,
                                               c.changeCallback1<GUIOptionalParam<int64>>(ParamIDs::kInt64Vst),
                                               true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalCallback<int64>(ParamIDs::kInt64Vst,
                                               c.changeCallback1<GUIOptionalParam<int64>>(ParamIDs::kInt64Vst),
                                               false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());
}

//------------------------------------------------------------------------
// ParamAware - Optional Discrete Param
//------------------------------------------------------------------------
TEST(ParamAware, testOptionalDiscreteParam)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerOptionalDiscreteParam - vst
  //------------------------------------------------------------------------
  GUIOptionalParam<int32> paramVst = c.registerOptionalDiscreteParam(ParamIDs::kInt64Vst, 10);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalDiscreteParam(ParamIDs::kInt64Vst, 10, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => listener NOT called
  c.vst(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  //------------------------------------------------------------------------
  // registerOptionalDiscreteParam - jmb
  //------------------------------------------------------------------------
  c.reset();

  GUIOptionalParam<int32> paramJmb = c.registerOptionalDiscreteParam(ParamIDs::kInt32Jmb, 10);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt32Jmb, paramJmb.getParamID());
  ASSERT_TRUE(paramJmb.exists());
  ASSERT_EQ(10, paramJmb.getStepCount());
  ASSERT_EQ(0, paramJmb.getValue());

  // changing jmb value => listener called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, paramJmb.getValue());

  ASSERT_TRUE(paramJmb.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, paramJmb.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing jmb value => listener NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramJmb = c.registerOptionalDiscreteParam(ParamIDs::kInt32Jmb, 10, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramJmb.getValue());

  // changing jmb value => listener NOT called
  c.jmb(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, paramJmb.getValue());

  ASSERT_TRUE(paramJmb.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramJmb.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerOptionalDiscreteParam - not valid => optional kicks in
  //------------------------------------------------------------------------
  c.reset();

  GUIOptionalParam<int32> paramVal = c.registerOptionalDiscreteParam(ParamIDs::kInvalid, 10);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInvalid, paramVal.getParamID());
  ASSERT_TRUE(paramVal.exists());
  ASSERT_EQ(10, paramVal.getStepCount());
  ASSERT_EQ(0, paramVal.getValue());

  c.vst(2);
  c.jmb(3);
  CHECK_EMPTY(c);

  ASSERT_TRUE(paramVal.update(5));
  CHECK(c, ParamIDs::kInvalid);
  ASSERT_EQ(5, paramVal.getValue());


  //------------------------------------------------------------------------
  // registerOptionalDiscreteCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();
  paramVst = c.registerOptionalDiscreteCallback(ParamIDs::kInt64Vst,
                                                10,
                                                c.changeCallback(ParamIDs::kInt64Vst),
                                                true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalDiscreteCallback(ParamIDs::kInt64Vst,
                                                10,
                                                c.changeCallback(ParamIDs::kInt64Vst),
                                                false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  //------------------------------------------------------------------------
  // registerOptionalDiscreteCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();
  paramVst = c.registerOptionalDiscreteCallback(ParamIDs::kInt64Vst,
                                                10,
                                                c.changeCallback1<GUIOptionalParam<int32>>(ParamIDs::kInt64Vst),
                                                true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, paramVst.getParamID());
  ASSERT_TRUE(paramVst.exists());
  ASSERT_EQ(kInt64Vst_StepCount, paramVst.getStepCount());
  ASSERT_EQ(0, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  paramVst = c.registerOptionalDiscreteCallback(ParamIDs::kInt64Vst,
                                                10,
                                                c.changeCallback1<GUIOptionalParam<int32>>(ParamIDs::kInt64Vst),
                                                false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, paramVst.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, paramVst.getValue());

  ASSERT_TRUE(paramVst.update(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, paramVst.getValue());
  ASSERT_EQ(2, c.vst());
}

//------------------------------------------------------------------------
// ParamAware - Raw Vst Param / Param ID
//------------------------------------------------------------------------
TEST(ParamAware, testRawParam_ParamID)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerRawVstParam
  //------------------------------------------------------------------------
  GUIRawVstParam param = c.registerRawVstParam(ParamIDs::kInt64Vst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerRawVstParam(ParamIDs::kInt64Vst, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing vst value => listener NOT called
  c.vst(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerRawVstParam(ParamIDs::kInvalid);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerRawVstParam(UNDEFINED_PARAM_ID);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerRawVstCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerRawVstCallback(ParamIDs::kInt64Vst,
                                   c.changeCallback(ParamIDs::kInt64Vst),
                                   true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerRawVstCallback(ParamIDs::kInt64Vst,
                                   c.changeCallback(ParamIDs::kInt64Vst),
                                   false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerRawVstCallback(ParamIDs::kInvalid,
                                   c.changeCallback(ParamIDs::kInvalid),
                                   false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerRawVstCallback(UNDEFINED_PARAM_ID,
                                   c.changeCallback(UNDEFINED_PARAM_ID),
                                   false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerRawVstCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerRawVstCallback(ParamIDs::kInt64Vst,
                                   c.changeCallback1<GUIRawVstParam>(ParamIDs::kInt64Vst),
                                   true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerRawVstCallback(ParamIDs::kInt64Vst,
                                   c.changeCallback1<GUIRawVstParam>(ParamIDs::kInt64Vst),
                                   false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerRawVstCallback(ParamIDs::kInvalid,
                                   c.changeCallback1<GUIRawVstParam>(ParamIDs::kInvalid),
                                   false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerRawVstCallback(UNDEFINED_PARAM_ID,
                                   c.changeCallback1<GUIRawVstParam>(UNDEFINED_PARAM_ID),
                                   false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);
}

//------------------------------------------------------------------------
// ParamAware - Raw Vst Param / ParamDef
//------------------------------------------------------------------------
TEST(ParamAware, testRawParam_ParamDef)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerParam
  //------------------------------------------------------------------------
  GUIRawVstParam param = c.registerParam(c.fParams.fRawVst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kRawVst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing raw value => listener called
  c.raw(0.6);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  // changing vst/jmb value => listener NOT called
  c.vst(4);
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerParam(c.fParams.fRawVst, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing raw value => listener NOT called
  c.raw(0.6);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fRawVst,
                             c.changeCallback(ParamIDs::kRawVst),
                             true);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(ParamIDs::kRawVst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing raw value => callback called
  c.raw(0.6);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  // changing jmb value => callback NOT called
  c.vst(4);
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fRawVst,
                             c.changeCallback(ParamIDs::kRawVst),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing raw value => callback called
  c.raw(0.6);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fRawVst,
                             c.changeCallback1<GUIRawVstParam>(ParamIDs::kRawVst),
                             true);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(ParamIDs::kRawVst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing raw value => callback called
  c.raw(0.6);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  // changing jmb value => callback NOT called
  c.vst(4);
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fRawVst,
                             c.changeCallback1<GUIRawVstParam>(ParamIDs::kRawVst),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(0.4, param.getValue());

  // changing vst value => callback called
  c.raw(0.6);
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.6, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(0.4));
  CHECK(c, ParamIDs::kRawVst);
  ASSERT_EQ(0.4, param.getValue());
  ASSERT_EQ(0.4, c.raw());

  // remove registration
  c.unregisterAll();
}

//------------------------------------------------------------------------
// ParamAware - Vst Param / Param ID
//------------------------------------------------------------------------
TEST(ParamAware, testVstParam_ParamID)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerVstParam
  //------------------------------------------------------------------------
  GUIVstParam<int64> param = c.registerVstParam<int64>(ParamIDs::kInt64Vst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerVstParam<int64>(ParamIDs::kInt64Vst, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => listener NOT called
  c.vst(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerVstParam<int64>(ParamIDs::kInvalid);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerVstParam<int64>(UNDEFINED_PARAM_ID);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerVstCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerVstCallback<int64>(ParamIDs::kInt64Vst,
                                       c.changeCallback(ParamIDs::kInt64Vst),
                                       true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerVstCallback<int64>(ParamIDs::kInt64Vst,
                                       c.changeCallback(ParamIDs::kInt64Vst),
                                       false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerVstCallback<int64>(ParamIDs::kInvalid,
                                       c.changeCallback(ParamIDs::kInvalid),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerVstCallback<int64>(UNDEFINED_PARAM_ID,
                                       c.changeCallback(UNDEFINED_PARAM_ID),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerVstCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerVstCallback<int64>(ParamIDs::kInt64Vst,
                                       c.changeCallback1<GUIVstParam<int64>>(ParamIDs::kInt64Vst),
                                       true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerVstCallback<int64>(ParamIDs::kInt64Vst,
                                       c.changeCallback1<GUIVstParam<int64>>(ParamIDs::kInt64Vst),
                                       false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerVstCallback<int64>(ParamIDs::kInvalid,
                                       c.changeCallback1<GUIVstParam<int64>>(ParamIDs::kInvalid),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerVstCallback<int64>(UNDEFINED_PARAM_ID,
                                       c.changeCallback1<GUIVstParam<int64>>(UNDEFINED_PARAM_ID),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);
}

//------------------------------------------------------------------------
// ParamAware - Vst Param / ParamDef
//------------------------------------------------------------------------
TEST(ParamAware, testVstParam_ParamDef)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerVstParam
  //------------------------------------------------------------------------
  GUIVstParam<int64> param = c.registerParam(c.fParams.fInt64Vst);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => listener called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => listener NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerParam(c.fParams.fInt64Vst, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => listener NOT called
  c.vst(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  //------------------------------------------------------------------------
  // registerVstCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fInt64Vst,
                             c.changeCallback(ParamIDs::kInt64Vst),
                             true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fInt64Vst,
                             c.changeCallback(ParamIDs::kInt64Vst),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  //------------------------------------------------------------------------
  // registerVstCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fInt64Vst,
                             c.changeCallback1<GUIVstParam<int64>>(ParamIDs::kInt64Vst),
                             true);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(ParamIDs::kInt64Vst, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(kInt64Vst_StepCount, param.getStepCount());
  ASSERT_EQ(0, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());

  // changing jmb value => callback NOT called
  c.jmb(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fInt64Vst,
                             c.changeCallback1<GUIVstParam<int64>>(ParamIDs::kInt64Vst),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing vst value => callback called
  c.vst(3);
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(3, param.getValue());

  ASSERT_EQ(kResultOk, param.setValue(2));
  CHECK(c, ParamIDs::kInt64Vst);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.vst());
}

//------------------------------------------------------------------------
// ParamAware - Jmb Param / Param ID
//------------------------------------------------------------------------
TEST(ParamAware, testJmbParam_ParamID)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerJmbParam
  //------------------------------------------------------------------------
  GUIJmbParam<int32> param = c.registerJmbParam<int32>(ParamIDs::kInt32Jmb);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => listener called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => listener NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerJmbParam<int32>(ParamIDs::kInt32Jmb, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => listener NOT called
  c.jmb(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerJmbParam<int32>(ParamIDs::kInvalid);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerJmbParam<int32>(UNDEFINED_PARAM_ID);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerJmbParam - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerJmbCallback<int32>(ParamIDs::kInt32Jmb,
                                       c.changeCallback(ParamIDs::kInt32Jmb),
                                       true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerJmbCallback<int32>(ParamIDs::kInt32Jmb,
                                       c.changeCallback(ParamIDs::kInt32Jmb),
                                       false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerJmbCallback<int32>(ParamIDs::kInvalid,
                                       c.changeCallback(ParamIDs::kInvalid),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerJmbCallback<int32>(UNDEFINED_PARAM_ID,
                                       c.changeCallback(UNDEFINED_PARAM_ID),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  //------------------------------------------------------------------------
  // registerVstCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerJmbCallback<int32>(ParamIDs::kInt32Jmb,
                                       c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                                       true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerJmbCallback<int32>(ParamIDs::kInt32Jmb,
                                       c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                                       false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // remove registration
  c.unregisterAll();

  // not a valid param
  param = c.registerJmbCallback<int32>(ParamIDs::kInvalid,
                                       c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInvalid),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);

  // undefined param
  param = c.registerJmbCallback<int32>(UNDEFINED_PARAM_ID,
                                       c.changeCallback1<GUIJmbParam<int32>>(UNDEFINED_PARAM_ID),
                                       false);
  ASSERT_FALSE(param.exists());
  CHECK_EMPTY(c);
}

//------------------------------------------------------------------------
// ParamAware - Jmb Param / ParamDef
//------------------------------------------------------------------------
TEST(ParamAware, testJmbParam_ParamDef)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerParam
  //------------------------------------------------------------------------
  GUIJmbParam<int32> param = c.registerParam(c.fParams.fInt32Jmb);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => listener called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => listener NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerParam(c.fParams.fInt32Jmb, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => listener NOT called
  c.jmb(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fInt32Jmb,
                             c.changeCallback(ParamIDs::kInt32Jmb),
                             true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fInt32Jmb,
                             c.changeCallback(ParamIDs::kInt32Jmb),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fParams.fInt32Jmb,
                             c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                             true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fParams.fInt32Jmb,
                             c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());
}

//------------------------------------------------------------------------
// ParamAware - Jmb Param / Param
//------------------------------------------------------------------------
TEST(ParamAware, testJmbParam_Param)
{
  MyController c{};

  //------------------------------------------------------------------------
  // registerParam
  //------------------------------------------------------------------------
  GUIJmbParam<int32> param = c.registerParam(c.fState.fInt32Jmb);
  CHECK_EMPTY(c);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => listener called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => listener NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerParam(c.fState.fInt32Jmb, false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => listener NOT called
  c.jmb(3);
  CHECK_EMPTY(c);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fState.fInt32Jmb,
                             c.changeCallback(ParamIDs::kInt32Jmb),
                             true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fState.fInt32Jmb,
                             c.changeCallback(ParamIDs::kInt32Jmb),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  //------------------------------------------------------------------------
  // registerCallback - Parameters::ChangeCallback1
  //------------------------------------------------------------------------
  c.reset();

  param = c.registerCallback(c.fState.fInt32Jmb,
                             c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                             true);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(ParamIDs::kInt32Jmb, param.getParamID());
  ASSERT_TRUE(param.exists());
  ASSERT_EQ(0, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());

  // changing vst value => callback NOT called
  c.vst(7);
  CHECK_EMPTY(c);

  // remove registration
  c.unregisterAll();

  param = c.registerCallback(c.fState.fInt32Jmb,
                             c.changeCallback1<GUIJmbParam<int32>>(ParamIDs::kInt32Jmb),
                             false);
  CHECK_EMPTY(c);
  ASSERT_EQ(2, param.getValue());

  // changing jmb value => callback called
  c.jmb(3);
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(3, param.getValue());

  ASSERT_TRUE(param.update(2));
  CHECK(c, ParamIDs::kInt32Jmb);
  ASSERT_EQ(2, param.getValue());
  ASSERT_EQ(2, c.jmb());
}

}
