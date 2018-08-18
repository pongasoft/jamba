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

#include "ParamDisplay.h"

#include <string>

namespace pongasoft {
namespace VST {
namespace Debug {

/**
 * This helper class is used to display the parameters (vst/jmb) in a table
 * WARNING: this class is allocating memory and as a result should be used in RT only during development! */
class ParamTable
{
public:
  explicit ParamTable(ParamDisplay iParamDisplay) : fParamDisplay{std::move(iParamDisplay)} {}

public:
  // builder pattern to initialize the table
  ParamTable &precision(int32 iPrecision) { fParamDisplay.precision(iPrecision); return *this; }
  ParamTable &keys(std::vector<ParamDisplay::Key> const &iKeys) { fParamDisplay.keys(iKeys); return *this; };
  ParamTable &key(ParamDisplay::Key iKey) { fParamDisplay.key(iKey); return *this; }
  ParamTable &ids(std::vector<ParamID> const &iParamIDs) { fParamDisplay.ids(iParamIDs); return *this; };
  ParamTable &id(ParamID iParamID) { fParamDisplay.id(iParamID); return *this; };
  ParamTable &header(bool iDisplayHeader = true) { fDisplayHeader = iDisplayHeader; return *this; };
  ParamTable &showHeader() { return header(true); };
  ParamTable &hideHeader() { return header(false); };
  ParamTable &headerSeparation(bool iDisplayHeaderSeparation = true) { fDisplayHeaderSeparation = iDisplayHeaderSeparation; return *this; };
  ParamTable &showHeaderSeparation() { return headerSeparation(true); };
  ParamTable &hideHeaderSeparation() { return headerSeparation(false); };
  ParamTable &lineSeparation(bool iDisplayLineSeparation = true) { fDisplayLineSeparation = iDisplayLineSeparation; return *this; };
  ParamTable &showLineSeparation() { return lineSeparation(true); };
  ParamTable &hideLineSeparation() { return lineSeparation(false); };
  ParamTable &cellSeparation(bool iDisplayCellSeparation = true) { fDisplayCellSeparation = iDisplayCellSeparation; return *this; };
  ParamTable &showCellSeparation() { return cellSeparation(true); };
  ParamTable &hideCellSeparation() { return cellSeparation(false); };

  ParamTable &compact() { return hideHeaderSeparation().hideLineSeparation().hideCellSeparation(); };
  ParamTable &full() { return showHeaderSeparation().showLineSeparation().showCellSeparation(); };

  // returns the table as a vector of rows
  std::vector<std::string> rows() const;

  // returns a tabular representation of the params
  std::string toString() const;

  // print the table
  void print(std::string const &iFirstLine = "") const;

  // returns the table as a vector of rows
  std::vector<std::string> rows(NormalizedState const &iNormalizedState) const;

  // returns a tabular representation of the params
  std::string toString(NormalizedState const &iNormalizedState) const;

  // print the table
  void print(NormalizedState const &iNormalizedState, std::string const &iFirstLine = "") const;

protected:
  // computeSizes
  std::string::size_type computeSizes(ParamDisplay::ValueMap const &iHeader,
                                      ParamDisplay::ParamMap const &iParams,
                                      std::map<ParamDisplay::Key, std::string::size_type> &oSizes) const;

  // computeHeader
  ParamDisplay::ValueMap computeHeader() const;

  // returns the table as a vector of rows
  std::vector<std::string> rows(ParamDisplay::ParamMap const &iParamMap) const;

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
  bool fDisplayHeaderSeparation{false};
  bool fDisplayLineSeparation{false};
  bool fDisplayCellSeparation{false};

  ParamDisplay fParamDisplay;
};

}
}
}

