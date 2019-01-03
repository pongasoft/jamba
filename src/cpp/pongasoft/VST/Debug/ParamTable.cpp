/*
 * Copyright (c) 2018-2019 pongasoft
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

#include "ParamTable.h"

#include <base/source/fstring.h>
#include "pongasoft/VST/Parameters.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>

namespace pongasoft {
namespace VST {
namespace Debug {

using Key = ParamDisplay::Key;
using ValueMap = ParamDisplay::ValueMap;
using ParamMap = ParamDisplay::ParamMap;

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamTable::toString() const
{
  auto rowsVector = rows();

  std::ostringstream s;

  for(size_t i = 0; i < rowsVector.size(); i++)
  {
    if(i > 0)
      s << std::endl;
    s << rowsVector[i];
  }

  return s.str();
}

//------------------------------------------------------------------------
// ParamTable::print
//------------------------------------------------------------------------
void ParamTable::print(std::string const &iFirstLine) const
{
#ifndef NDEBUG
  auto rowsVector = rows();

  if(!iFirstLine.empty())
    DLOG_F(INFO, "%s", iFirstLine.c_str());

  for(auto &row : rowsVector)
  {
    DLOG_F(INFO, "%s", row.c_str());
  }
#endif
}

//------------------------------------------------------------------------
// ParamTable::rows
//------------------------------------------------------------------------
std::vector<std::string> ParamTable::rows(ParamDisplay::ParamMap const &iParamMap) const
{
  std::vector<std::string> res{};

  ValueMap header = fDisplayHeader ? computeHeader() : ValueMap{};

  std::map<Key, std::string::size_type> sizes{};
  std::string::size_type totalSize = computeSizes(header, iParamMap, sizes);

  std::string cellStart = !fDisplayCellSeparation ? "" : "| ";
  std::string cellEnd = " ";
  std::string lineEnd = !fDisplayCellSeparation ? "" : "|";

  std::string::size_type separationLineSize = totalSize + (cellStart.size() + cellEnd.size()) * fParamDisplay.keys().size() + lineEnd.size();

  if(fDisplayHeader)
  {
    std::ostringstream s;

    for(auto key: fParamDisplay.keys())
    {
      s << cellStart << std::left << std::setw(static_cast<int>(sizes.at(key))) << std::setfill(' ')
        << header.at(key) << cellEnd;
    }
    s << lineEnd;

    res.emplace_back(s.str());

    if(fDisplayHeaderSeparation)
      res.emplace_back(separationLineSize, '-');
  }

  for(auto paramID : fParamDisplay.ids())
  {
    auto const &param = iParamMap.at(paramID);

    std::ostringstream s;

    for(auto key: fParamDisplay.keys())
    {
      s << cellStart << std::left << std::setw(static_cast<int>(sizes.at(key))) << std::setfill(' ') << param.at(key)
        << cellEnd;
    }
    s << lineEnd;

    res.emplace_back(s.str());

    if(fDisplayLineSeparation)
      res.emplace_back(separationLineSize, '-');
  }

  return res;
}

//------------------------------------------------------------------------
// ParamTable::rows
//------------------------------------------------------------------------
std::vector<std::string> ParamTable::rows() const
{
  return rows(fParamDisplay.getParamMap());
}

//------------------------------------------------------------------------
// ParamTable::rows
//------------------------------------------------------------------------
std::vector<std::string> ParamTable::rows(NormalizedState const &iNormalizedState) const
{
  return rows(fParamDisplay.getParamMap(iNormalizedState));
}

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamTable::toString(NormalizedState const &iNormalizedState) const
{
  auto rowsVector = rows(iNormalizedState);

  std::ostringstream s;

  for(size_t i = 0; i < rowsVector.size(); i++)
  {
    if(i > 0)
      s << std::endl;
    s << rowsVector[i];
  }

  return s.str();
}

//------------------------------------------------------------------------
// ParamTable::print
//------------------------------------------------------------------------
void ParamTable::print(NormalizedState const &iNormalizedState, std::string const &iFirstLine) const
{
#ifndef NDEBUG
  auto rowsVector = rows(iNormalizedState);

  if(!iFirstLine.empty())
    DLOG_F(INFO, "%s", iFirstLine.c_str());

  for(auto &row : rowsVector)
  {
    DLOG_F(INFO, "%s", row.c_str());
  }
#endif
}

//------------------------------------------------------------------------
// ParamTable::computeHeader
//------------------------------------------------------------------------
ParamDisplay::ValueMap ParamTable::computeHeader() const
{
  ValueMap header{};

  for(auto key: fParamDisplay.keys())
  {
    header[key] = fParamDisplay.getValue(key);
  }

  return header;
}

//------------------------------------------------------------------------
// ParamTable::computeSizes
//------------------------------------------------------------------------
std::string::size_type ParamTable::computeSizes(ValueMap const &iHeader,
                                                ParamMap const &iParams,
                                                std::map<Key, std::string::size_type> &oSizes) const
{
  std::string::size_type totalSize = 0;

  for(auto key : fParamDisplay.keys())
  {
    std::string::size_type size = iHeader.empty() ? 0 : iHeader.at(key).size();
    for(auto paramID : fParamDisplay.ids())
    {
      auto const &param = iParams.at(paramID);
      size = std::max(size, param.at(key).size());

    }
    oSizes[key] = size;
    totalSize += size;
  }

  return totalSize;
}

using Key = ParamDisplay::Key;

std::vector<Key> DEFAULT_VST_KEYS { // NOLINT
  Key::kID, Key::kTitle, Key::kType, Key::kOwner, Key::kTransient, Key::kShared,
  Key::kNormalizedDefault, Key::kDefault,
  Key::kSteps, Key::kFlags, Key::kShortTitle,
  Key::kPrecision, Key::kUnitID, Key::kUnits
};

std::vector<Key> DEFAULT_STATE_KEYS { // NOLINT
  Key::kID, Key::kTitle, Key::kNormalizedValue, Key::kValue
};


//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(Parameters const &iParams)
{
  return ParamTable(ParamDisplay::from(iParams).keys(DEFAULT_VST_KEYS));
}

//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(RT::RTState const *iState, bool iSaveStateOnly)
{
  return ParamTable(ParamDisplay::from(iState, iSaveStateOnly).keys(DEFAULT_STATE_KEYS));
}

//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(GUI::GUIState const *iState, bool iSaveStateOnly)
{
  return ParamTable(ParamDisplay::from(iState, iSaveStateOnly).keys(DEFAULT_STATE_KEYS));
}

}
}
}