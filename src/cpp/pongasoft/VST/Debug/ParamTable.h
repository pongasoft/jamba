#include <utility>

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
 * This helper class is used to display the parameters (vst/jmb) */
class ParamTable
{
public:
  enum class Column
  {
    kID,
    kType,
    kTitle,
    kOwner,
    kTransient,
    kDefault,
    kDefaultAsString,
    kValue,
    kValueAsString,
    kSteps,
    kFlags,
    kShortTitle,
    kPrecision,
    kUnitID,
    kUnits,
    kShared
  };

public:
  using Row = std::map<Column, std::string>;
  using Rows = std::vector<Row>;

  // builder pattern to initialize the table
  ParamTable &precision(int32 iPrecision) { fPrecision = iPrecision; return *this; }
  ParamTable &columns(std::vector<Column> const &iColumns) { fColumns = iColumns; return *this; };
  ParamTable &column(Column iColumn) { fColumns.emplace_back(iColumn); return *this; }
  ParamTable &header(bool iDisplayHeader = true) { fDisplayHeader = iDisplayHeader; return *this; };
  ParamTable &headerSeparation(bool iDisplayHeaderSeparation = true) { fDisplayHeaderSeparation = iDisplayHeaderSeparation; return *this; };
  ParamTable &ids(std::vector<ParamID> const &iParamIDs) { fParamIDs = iParamIDs; return *this; };
  ParamTable &id(ParamID iParamID) { fParamIDs.emplace_back(iParamID); return *this; };
  ParamTable &rows(Parameters const &iParams) { fParameters = &iParams; return *this; }
  ParamTable &rows(Parameters const *iParams) { fParameters = iParams; return *this; }
  ParamTable &rows(RT::RTState const *iState) { fRTState = iState; return *this; };
  ParamTable &rows(GUI::GUIState const *iState) { fGUIState = iState; return *this; };

  // returns a tabular representation of the table
  std::string toString() const;

  // returns the table as a vector of rows
  std::vector<std::string> rows() const;

  // print the table
  void print() const;

  // retrieve each cell value
  std::string getHeaderName(Column iColumn) const;
  std::string getValue(std::shared_ptr<RawVstParamDef> const &iParamDef, Column iColumn) const;
  std::string getValue(std::shared_ptr<IJmbParamDef> const &iParamDef, Column iColumn) const;
  std::string getValue(std::unique_ptr<RT::RTRawVstParameter> const &iParam, Column iColumn) const;
  std::string getValue(std::unique_ptr<RT::IRTJmbInParameter> const &iParam, Column iColumn) const;
  std::string getValue(std::unique_ptr<RT::IRTJmbOutParameter> const &iParam, Column iColumn) const;
  std::string getValue(std::unique_ptr<GUI::GUIRawVstParameter> const &iParam, Column iColumn, Parameters const *iParameters) const;
  std::string getValue(GUI::IGUIJmbParameter const &iParam, Column iColumn) const;

protected:
  // convenient call which uses the precision to render the raw value
  std::string toString(ParamValue iValue) const;

  // computeSizes
  void computeSizes(Row const &iHeader, Rows const &iRows, std::map<Column, std::string::size_type> &oSizes) const;

  // computeHeader
  Row computeHeader() const;

  // generic call to iterate over all columns
  template<typename T, typename... Args>
  void computeParamRow(T const &iParam, Row &oRow, Args... args) const;

  void computeRows(Parameters const *iParams, Rows &oRows) const;
  void computeRows(RT::RTState const *iState, Rows &oRows) const;
  void computeRows(GUI::GUIState const *iState, Rows &oRows) const;

public:
  /**
   * Shortcut to create a table for all registered parameter (definition not current value)
   */
  static ParamTable from(Parameters const &iParams);

  /**
   * Shortcut to create a table for the RTState (current values)
   */
  static ParamTable from(RT::RTState const *iState, bool iSaveStateOnly = false);

  /**
   * Shortcut to create a table for the GUIState (current values)
   */
  static ParamTable from(GUI::GUIState const *iState, bool iSaveStateOnly = false);

private:
  bool fDisplayHeader{true};
  bool fDisplayHeaderSeparation{true};
  std::vector<Column> fColumns{};
  std::vector<ParamID> fParamIDs{};
  int32 fPrecision{3};

  Parameters const *fParameters{};
  RT::RTState const *fRTState{};
  GUI::GUIState const *fGUIState{};
};

}
}
}

