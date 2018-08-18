/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/RT/RTState.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <string>

namespace pongasoft {
namespace VST {
namespace Debug {

/**
 * This helper class is used to display the parameters (vst/jmb)
 * WARNING: this class is allocating memory and as a result should be used in RT only during development! */
class ParamDisplay
{
public:
  enum class Key
  {
    kID,
    kType,
    kTitle,
    kOwner,
    kTransient,
    kNormalizedDefault,
    kDefault,
    kNormalizedValue,
    kValue,
    kSteps,
    kFlags,
    kShortTitle,
    kPrecision,
    kUnitID,
    kUnits,
    kShared
  };

public:
  using Value = std::string;
  using ValueMap = std::map<Key, Value>;
  using ParamMap = std::map<ParamID, ValueMap>;

public:
  explicit ParamDisplay(Parameters const &iParams) : fParameters{&iParams} {}
  explicit ParamDisplay(RT::RTState const *iState) : fRTState{iState} {}
  explicit ParamDisplay(GUI::GUIState const *iState) : fGUIState{iState} {}

  // builder pattern to initialize the display
  ParamDisplay &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
  ParamDisplay &keys(std::vector<Key> const &iKeys) { fKeys = iKeys; return *this; };
  ParamDisplay &key(Key iKey) { fKeys.emplace_back(iKey); return *this; }
  ParamDisplay &ids(std::vector<ParamID> const &iParamIDs) { fParamIDs = iParamIDs; return *this; };
  ParamDisplay &id(ParamID iParamID) { fParamIDs.emplace_back(iParamID); return *this; };

  Value getValue(Key iKey) const;
  Value getValue(ParamID iParamID, Key iKey) const;
  ValueMap getValues(ParamID iParamID, std::vector<Key> const &iKeys = {}) const;
  ParamMap getParamMap(std::vector<ParamID> const &iParamIDs = {}) const;
  ParamMap getParamMap(NormalizedState const &iNormalizedState) const;

  std::vector<Key> const &keys() const { return fKeys; }
  std::vector<ParamID> const &ids() const { return fParamIDs; }

protected:
  Value getValue(std::shared_ptr<RawVstParamDef> const &iParamDef, Key iKey) const;
  Value getValue(std::shared_ptr<IJmbParamDef> const &iParamDef, Key iKey) const;
  Value getValue(std::unique_ptr<RT::RTRawVstParameter> const &iParam, Key iKey) const;
  Value getValue(std::unique_ptr<RT::IRTJmbInParameter> const &iParam, Key iKey) const;
  Value getValue(std::unique_ptr<RT::IRTJmbOutParameter> const &iParam, Key iKey) const;
  Value getValue(std::unique_ptr<GUI::GUIRawVstParameter> const &iParam, Key iKey, Parameters const *iParameters) const;
  Value getValue(GUI::IGUIJmbParameter const &iParam, Key iKey) const;

  Value getValue(Parameters const *iParams, ParamID iParamID, Key iKey) const;
  Value getValue(RT::RTState const *iState, ParamID iParamID, Key iKey) const;
  Value getValue(GUI::GUIState const *iState, ParamID iParamID, Key iKey) const;

  // convenient call which uses the precision to render the raw value
  Value getValue(ParamValue iValue) const;

  // getRawVstParamDef (nullptr if not found)
  std::shared_ptr<RawVstParamDef> getRawVstParamDef(ParamID iParamID) const;

public:
  /**
   * Shortcut to create a table for all registered parameter (definition not current value). Use all registered
   * parameters
   */
  static ParamDisplay from(Parameters const &iParams);

  /**
   * Shortcut to create a table for the RTState (current values). Use all registered parameters or the one used
   * in save state only.
   */
  static ParamDisplay from(RT::RTState const *iState, bool iSaveStateOnly = false);

  /**
   * Shortcut to create a table for the GUIState (current values). Use all registered parameters or the one used
   * in save state only.
   */
  static ParamDisplay from(GUI::GUIState const *iState, bool iSaveStateOnly = false);

private:
  std::vector<Key> fKeys{};
  std::vector<ParamID> fParamIDs{};
  int32 fPrecision{3};

  Parameters const *fParameters{};
  RT::RTState const *fRTState{};
  GUI::GUIState const *fGUIState{};
};

}
}
}

