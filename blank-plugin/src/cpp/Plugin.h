#ifndef VST3_JAMBA_[-name-]_PLUGIN_H
#define VST3_JAMBA_[-name-]_PLUGIN_H

#include "[-name-]CIDs.h"

#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/RT/RTState.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/GUI/Params/GUIParamSerializers.h>

#ifndef NDEBUG
#include <pongasoft/VST/Debug/ParamLine.h>
#include <pongasoft/VST/Debug/ParamTable.h>
#endif

#include <pluginterfaces/vst/ivstaudioprocessor.h>

namespace [-namespace-] {

using namespace pongasoft::VST;
using namespace GUI::Params;

// keeping track of the version of the state being saved so that it can be upgraded more easily later
// should be > 0
constexpr uint16 PROCESSOR_STATE_VERSION = 1;
constexpr uint16 CONTROLLER_STATE_VERSION = 1;

//------------------------------------------------------------------------
// [-name-]Parameters
//------------------------------------------------------------------------
class [-name-]Parameters : public Parameters
{
public:
  VstParam<bool> fBypassParam;    // the bypass toggle (bypasses the processing if Fx)

public:
  [-name-]Parameters()
  {
    // bypass
    fBypassParam =
      vst<BooleanParamConverter>(E[-name-]ParamID::kBypass, STR16 ("Bypass"))
        .defaultValue(false)
        .flags(ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass)
        .shortTitle(STR16 ("Bypass"))
        .add();


    setRTSaveStateOrder(PROCESSOR_STATE_VERSION,
                        fBypassParam);

    // same for GUI - note that if the GUI does not save anything then you don't need this
    // setGUISaveStateOrder(CONTROLLER_STATE_VERSION);
  }
};

using namespace RT;

//------------------------------------------------------------------------
// [-name-]RTState
//------------------------------------------------------------------------
class [-name-]RTState : public RTState
{
public:
  RTVstParam<bool> fBypass;

public:
  explicit [-name-]RTState([-name-]Parameters const &iParams) :
    RTState(iParams),
    fBypass{add(iParams.fBypassParam)}
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
// [-name-]GUIState
//------------------------------------------------------------------------
class [-name-]GUIState : public GUIPluginState<[-name-]Parameters>
{
public:
  //------------------------------------------------------------------------
  // GUI Parameters go here...
  //------------------------------------------------------------------------

public:
  explicit [-name-]GUIState([-name-]Parameters const &iParams) :
    GUIPluginState(iParams)
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

#endif // VST3_JAMBA_[-name-]_PLUGIN_H