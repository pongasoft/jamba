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

#pragma once

#include "JambaTestPluginCIDs.h"
#include "Model.h"

#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/RT/RTState.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/Params/GUIParamSerializers.h>

#ifndef NDEBUG
#include <pongasoft/VST/Debug/ParamLine.h>
#include <pongasoft/VST/Debug/ParamTable.h>
#endif

#include <pluginterfaces/vst/ivstaudioprocessor.h>

namespace pongasoft::test::jamba {

using namespace pongasoft::VST;
using namespace GUI::Params;

// keeping track of the version of the state being saved so that it can be upgraded more easily later
// should be > 0
constexpr uint16 PROCESSOR_STATE_VERSION = 1;
constexpr uint16 CONTROLLER_STATE_VERSION = 1;

//------------------------------------------------------------------------
// JambaTestPluginParameters
//------------------------------------------------------------------------
class JambaTestPluginParameters : public Parameters
{
public:
  VstParam<bool> fBypass; // the bypass toggle (bypasses the processing if Fx)

  VstParam<ETabs> fTab;        // multiple tabs to test different aspects of the framework

  // various flavors of Vst parameters
  RawVstParam fRawVst;
  VstParam<ParamValue> fParamValueVst;
  VstParam<int32> fInt32Vst;
  VstParam<int64> fInt64Vst;
  VstParam<bool> fBoolVst;
  VstParam<EEnum> fEnumVst;
  VstParam<EEnumClass> fEnumClassVst;
  VstParam<ENonContiguousEnum> fNonContiguousEnumVst;
  VstParam<Gain> fGainVst;

  // various flavors of Jmb parameters
  JmbParam<ParamValue> fParamValueJmb;
  JmbParam<int32> fInt32Jmb;
  JmbParam<int64> fInt64Jmb;
  JmbParam<bool> fBoolJmb;
  JmbParam<EEnum> fEnumJmb;
  JmbParam<EEnumClass> fEnumClassJmb;
  JmbParam<ENonContiguousEnum> fNonContiguousEnumJmb;
  JmbParam<Gain> fGainJmb;
  JmbParam<UTF8String> fUTF8String;

public:
  JambaTestPluginParameters();
};

using namespace RT;

//------------------------------------------------------------------------
// JambaTestPluginRTState
//------------------------------------------------------------------------
class JambaTestPluginRTState : public RTState
{
public:
  RTVstParam<bool> fBypass;
  RTRawVstParam fRawVst;
  RTVstParam<ParamValue> fParamValueVst;
  RTVstParam<int32> fInt32Vst;
  RTVstParam<int64> fInt64Vst;
  RTVstParam<bool> fBoolVst;
  RTVstParam<EEnum> fEnumVst;
  RTVstParam<EEnumClass> fEnumClassVst;
  RTVstParam<ENonContiguousEnum> fNonContiguousEnumVst;
  RTVstParam<Gain> fGainVst;

public:
  explicit JambaTestPluginRTState(JambaTestPluginParameters const &iParams) :
    RTState(iParams),
    fBypass{add(iParams.fBypass)},
    fRawVst{add(iParams.fRawVst)},
    fParamValueVst{add(iParams.fParamValueVst)},
    fInt32Vst{add(iParams.fInt32Vst)},
    fInt64Vst{add(iParams.fInt64Vst)},
    fBoolVst{add(iParams.fBoolVst)},
    fEnumVst{add(iParams.fEnumVst)},
    fEnumClassVst{add(iParams.fEnumClassVst)},
    fNonContiguousEnumVst{add(iParams.fNonContiguousEnumVst)},
    fGainVst{add(iParams.fGainVst)}
  {
  }

//------------------------------------------------------------------------
// Debug read/write RT state
//------------------------------------------------------------------------
#ifndef NDEBUG
protected:
  // afterReadNewState
  void afterReadNewState(NormalizedState const *iState) override
  {
    // swap the commented line to display either on a line or in a table
    DLOG_F(INFO, "RTState::read - %s", Debug::ParamLine::from(this, true).toString(*iState).c_str());
    //Debug::ParamTable::from(this, true).showCellSeparation().print(*iState, "RTState::read ---> ");
  }

  // beforeWriteNewState
  void beforeWriteNewState(NormalizedState const *iState) override
  {
    // swap the commented line to display either on a line or in a table
    DLOG_F(INFO, "RTState::write - %s", Debug::ParamLine::from(this, true).toString(*iState).c_str());
    //Debug::ParamTable::from(this, true).showCellSeparation().print(*iState, "RTState::write ---> ");
  }
#endif
};

using namespace GUI;

//------------------------------------------------------------------------
// JambaTestPluginGUIState
//------------------------------------------------------------------------
class JambaTestPluginGUIState : public GUIPluginState<JambaTestPluginParameters>
{
public:
  GUIJmbParam<ParamValue> fParamValueJmb;
  GUIJmbParam<int32> fInt32Jmb;
  GUIJmbParam<int64> fInt64Jmb;
  GUIJmbParam<bool> fBoolJmb;
  GUIJmbParam<EEnum> fEnumJmb;
  GUIJmbParam<EEnumClass> fEnumClassJmb;
  GUIJmbParam<ENonContiguousEnum> fNonContiguousEnumJmb;
  GUIJmbParam<Gain> fGainJmb;
  GUIJmbParam<UTF8String> fUTF8String;

public:
  explicit JambaTestPluginGUIState(JambaTestPluginParameters const &iParams) :
    GUIPluginState(iParams),
    fParamValueJmb{add(iParams.fParamValueJmb)},
    fInt32Jmb{add(iParams.fInt32Jmb)},
    fInt64Jmb{add(iParams.fInt64Jmb)},
    fBoolJmb{add(iParams.fBoolJmb)},
    fEnumJmb{add(iParams.fEnumJmb)},
    fEnumClassJmb{add(iParams.fEnumClassJmb)},
    fNonContiguousEnumJmb{add(iParams.fNonContiguousEnumJmb)},
    fGainJmb{add(iParams.fGainJmb)},
    fUTF8String{add(iParams.fUTF8String)}
  {};

//------------------------------------------------------------------------
// Debug read/write GUI state
//------------------------------------------------------------------------
#ifndef NDEBUG
protected:
  // readGUIState
  tresult readGUIState(IBStreamer &iStreamer) override
  {
    tresult res = GUIState::readGUIState(iStreamer);
    if(res == kResultOk)
    {
      // swap the commented line to display either on a line or in a table
      DLOG_F(INFO, "GUIState::read - %s", Debug::ParamLine::from(this, true).toString().c_str());
      //Debug::ParamTable::from(this, true).showCellSeparation().print("GUIState::read ---> ");
    }
    return res;
  }

  // writeGUIState
  tresult writeGUIState(IBStreamer &oStreamer) const override
  {
    tresult res = GUIState::writeGUIState(oStreamer);
    if(res == kResultOk)
    {
      // swap the commented line to display either on a line or in a table
      DLOG_F(INFO, "GUIState::write - %s", Debug::ParamLine::from(this, true).toString().c_str());
      //Debug::ParamTable::from(this, true).showCellSeparation().print("GUIState::write ---> ");
    }
    return res;
  }
#endif

};

}
