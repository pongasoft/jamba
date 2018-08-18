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

#include "ParamDisplay.h"

#include <sstream>

namespace pongasoft {
namespace VST {
namespace Debug {

/**
 * This helper class is used to display the parameters (vst/jmb) in a line
 * WARNING: this class is allocating memory and as a result should be used in RT only during development! */
class ParamLine
{
public:
  explicit ParamLine(ParamDisplay iParamDisplay) : fParamDisplay{std::move(iParamDisplay)} {}

  ParamLine &precision(int32 iPrecision) { fParamDisplay.precision(iPrecision); return *this; }
  ParamLine &keys(std::vector<ParamDisplay::Key> const &iKeys) { fParamDisplay.keys(iKeys); return *this; };
  ParamLine &key(ParamDisplay::Key iKey) { fParamDisplay.key(iKey); return *this; }
  ParamLine &ids(std::vector<ParamID> const &iParamIDs) { fParamDisplay.ids(iParamIDs); return *this; };
  ParamLine &id(ParamID iParamID) { fParamDisplay.id(iParamID); return *this; };
  ParamLine &firstSeparator(std::string iString) { fFirstSeparator = std::move(iString); return *this; }
  ParamLine &nextSeparator(std::string iString) { fNextSeparator = std::move(iString); return *this; }
  ParamLine &paramSeparator(std::string iString) { fParamSeparator = std::move(iString); return *this; }
  ParamLine &skipEmptyEntry(bool iFlag = true) { fSkipEmptyEntry = iFlag; return *this; }

  // returns a line representation of the params
  std::string toString() const;

  // returns a line representation of normalized state
  std::string toString(NormalizedState const &iNormalizedState) const;

public:
  /**
   * Shortcut to create a table for all registered parameter (definition not current value)
   */
  static ParamLine from(Parameters const &iParams);

  /**
   * Shortcut to create a table for the RTState (current values)
   */
  static ParamLine from(RT::RTState const *iState, bool iSaveStateOnly = false);

  /**
   * Shortcut to create a table for the GUIState (current values)
   */
  static ParamLine from(GUI::GUIState const *iState, bool iSaveStateOnly = false);

protected:
  // toString
  std::string toString(ParamDisplay::ParamMap const &iParamMap) const;

private:
  ParamDisplay fParamDisplay;

  std::string fFirstSeparator{"="};
  std::string fNextSeparator{"/"};
  std::string fParamSeparator{", "};
  bool fSkipEmptyEntry{true};
};

}
}
}
