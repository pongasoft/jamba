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

//------------------------------------------------------------------------
// ParamTable::toString
//------------------------------------------------------------------------
std::string ParamTable::toString() const
{
  auto rowsVector = rows();

  std::ostringstream s;

  for(auto &row : rowsVector)
  {
    s << row << std::endl;
  }

  return s.str();
}

//------------------------------------------------------------------------
// ParamTable::print
//------------------------------------------------------------------------
void ParamTable::print() const
{
  auto rowsVector = rows();

  for(auto &row : rowsVector)
  {
    DLOG_F(INFO, "%s", row.c_str());
  }
}

//------------------------------------------------------------------------
// ParamTable::rows
//------------------------------------------------------------------------
std::vector<std::string> ParamTable::rows() const
{
  std::vector<std::string> res{};

  Row header = fDisplayHeader ? computeHeader() : Row{};

  Rows rows{};
  if(fParameters)
    computeRows(fParameters, rows);
  if(fRTState)
    computeRows(fRTState, rows);
  if(fGUIState)
    computeRows(fGUIState, rows);

  std::map<Column, std::string::size_type> sizes{};
  computeSizes(header, rows, sizes);

  unsigned long totalSize = 0;
  if(fDisplayHeader)
  {
    std::ostringstream s;

    for(auto column: fColumns)
    {
      s << "| " << std::left << std::setw(static_cast<int>(sizes.at(column))) << std::setfill(' ')
        << header.at(column) << " ";
      totalSize += sizes.at(column) + 3;
    }
    s << "|";

    res.emplace_back(s.str());

    if(fDisplayHeaderSeparation)
      res.emplace_back(std::string(s.str().size(), '-'));
  }

  for(auto const &row : rows)
  {
    std::ostringstream s;

    for(auto column: fColumns)
    {
      s << "| " << std::left << std::setw(static_cast<int>(sizes.at(column))) << std::setfill(' ') << row.at(column)
        << " ";
    }
    s << "|";

    res.emplace_back(s.str());
  }

  return res;
}

//------------------------------------------------------------------------
// ParamTable::computeHeader
//------------------------------------------------------------------------
ParamTable::Row ParamTable::computeHeader() const
{
  Row header{};

  for(auto column: fColumns)
  {
    header[column] = getHeaderName(column);
  }

  return header;
}

//------------------------------------------------------------------------
// ParamTable::computeSizes
//------------------------------------------------------------------------
void ParamTable::computeSizes(Row const &iHeader, Rows const &iRows, std::map<Column, std::string::size_type> &oSizes) const
{
  for(auto column : fColumns)
  {
    std::string::size_type size = iHeader.empty() ? 0 : iHeader.at(column).size();
    for(auto const &row : iRows)
    {
      size = std::max(size, row.at(column).size());
    }
    oSizes[column] = size;
  }
}

//------------------------------------------------------------------------
// ParamTable::computeParamRow
//------------------------------------------------------------------------
template<typename T, typename... Args>
void ParamTable::computeParamRow(T const &iParam, Row &oRow, Args... args) const
{
  for(auto column: fColumns)
  {
    oRow[column] = getValue(iParam, column, args...);
  }
}

//------------------------------------------------------------------------
// ParamTable::computeRows - Parameters
//------------------------------------------------------------------------
void ParamTable::computeRows(Parameters const *iParams, ParamTable::Rows &oRows) const
{
  if(!iParams)
    return;

  for(auto paramID : fParamIDs)
  {
    Row row{};
    auto vst = iParams->getRawVstParamDef(paramID);
    if(vst)
      computeParamRow(vst, row);
    else
    {
      auto jmb = iParams->getJmbParamDef(paramID);
      if(jmb)
        computeParamRow(jmb, row);
    }

    if(!row.empty())
      oRows.emplace_back(row);
  }
}

//------------------------------------------------------------------------
// ParamTable::rows - RT::RTState
//------------------------------------------------------------------------
void ParamTable::computeRows(RT::RTState const *iState, ParamTable::Rows &oRows) const
{
  if(!iState)
    return;

  for(auto paramID : fParamIDs)
  {
    Row row{};
    // vst - RawVstParamDef
    auto vst = iState->fVstParameters.find(paramID);
    if(vst != iState->fVstParameters.cend())
    {
      computeParamRow(vst->second, row);
    }
    else
    {
      // jmbIn - IRTJmbInParameter
      auto jmbIn = iState->fInboundMessagingParameters.find(paramID);
      if(jmbIn != iState->fInboundMessagingParameters.cend())
      {
        computeParamRow(jmbIn->second, row);
      }
      else
      {
        // IRTJmbOutParameter
        auto jmbOut = iState->fOutboundMessagingParameters.find(paramID);
        if(jmbOut != iState->fOutboundMessagingParameters.cend())
        {
          computeParamRow(jmbOut->second, row);
        }
      }
    }

    if(!row.empty())
      oRows.emplace_back(row);
  }
}

//------------------------------------------------------------------------
// ParamTable::rows - GUI::GUIState
//------------------------------------------------------------------------
void ParamTable::computeRows(GUI::GUIState const *iState, ParamTable::Rows &oRows) const
{
  if(!iState)
    return;

  for(auto paramID : fParamIDs)
  {
    Row row{};
    // vst - GUIRawVstParameter
    auto vst = iState->getRawVstParameter(paramID);
    if(vst)
    {
      computeParamRow(vst, row, &iState->fPluginParameters);
    }
    else
    {
      // jmb - IGUIJmbParameter
      auto jmb = iState->getJmbParameter(paramID);
      if(jmb)
      {
        computeParamRow(*jmb, row);
      }
    }

    if(!row.empty())
      oRows.emplace_back(row);
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - RawVstParamDef
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::shared_ptr<RawVstParamDef> const &iParamDef, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kID:
      return std::to_string(iParamDef->fParamID);
    case Column::kType:
      return "vst";
    case Column::kTitle:
      return String(iParamDef->fTitle).text8();
    case Column::kOwner:
      return iParamDef->fOwner == IParamDef::Owner::kGUI ? "ui" : "rt";
    case Column::kTransient:
      return iParamDef->fTransient ? "x" : "";
    case Column::kDefault:
      return toString(iParamDef->fDefaultValue);
    case Column::kDefaultAsString:
    {
      String128 s;
      iParamDef->toString(iParamDef->fDefaultValue, s);
      return String(s).text8();
    }
    case Column::kSteps:
      return std::to_string(iParamDef->fStepCount);
    case Column::kFlags:
      return std::to_string(iParamDef->fFlags);
    case Column::kShortTitle:
      return String(iParamDef->fShortTitle).text8();
    case Column::kPrecision:
      return std::to_string(iParamDef->fPrecision);
    case Column::kUnitID:
      return std::to_string(iParamDef->fUnitID);
    case Column::kUnits:
      return String(iParamDef->fUnits).text8();
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - RTRawVstParameter
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::unique_ptr<RT::RTRawVstParameter> const &iParam, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kValue:
      return toString(iParam->getNormalizedValue());

    case Column::kValueAsString:
    {
      String128 s;
      iParam->getParamDef()->toString(iParam->getNormalizedValue(), s);
      return String(s).text8();
    }

    default:
      return getValue(iParam->getParamDef(), iColumn);
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - IRTJmbInParameter
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::unique_ptr<RT::IRTJmbInParameter> const &iParam, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kValueAsString:
    {
      std::ostringstream s;
      iParam->writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam->getParamDef(), iColumn);
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - IRTJmbOutParameter
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::unique_ptr<RT::IRTJmbOutParameter> const &iParam, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kValueAsString:
    {
      std::ostringstream s;
      iParam->writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam->getParamDef(), iColumn);
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - IJmbParamDef
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::shared_ptr<IJmbParamDef> const &iParamDef, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kID:
      return std::to_string(iParamDef->fParamID);
    case Column::kType:
      return "jmb";
    case Column::kTitle:
      return String(iParamDef->fTitle).text8();
    case Column::kOwner:
      return iParamDef->fOwner == IParamDef::Owner::kGUI ? "ui" : "rt";
    case Column::kTransient:
      return iParamDef->fTransient ? "x" : "";
    case Column::kDefaultAsString:
    {
      std::ostringstream s;
      iParamDef->writeDefaultValue(s);
      return s.str();
    }
    case Column::kShared:
      return iParamDef->fShared ? "x" : "";
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - GUIRawVstParameter
//------------------------------------------------------------------------
std::string ParamTable::getValue(std::unique_ptr<GUI::GUIRawVstParameter> const &iParam,
                                 Column iColumn,
                                 Parameters const *iParameters) const
{
  switch(iColumn)
  {
    case Column::kValue:
      return toString(iParam->getValue());

    case Column::kValueAsString:
    {
      String128 s;
      iParameters->getRawVstParamDef(iParam->getParamID())->toString(iParam->getValue(), s);
      return String(s).text8();
    }

    default:
      return getValue(iParameters->getRawVstParamDef(iParam->getParamID()), iColumn);
  }
}

//------------------------------------------------------------------------
// ParamTable::getValue - IGUIJmbParameter
//------------------------------------------------------------------------
std::string ParamTable::getValue(GUI::IGUIJmbParameter const &iParam, Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kValueAsString:
    {
      std::ostringstream s;
      iParam.writeToStream(s);
      return s.str();
    }

    default:
      return getValue(iParam.getParamDef(), iColumn);
  }
}


//------------------------------------------------------------------------
// ParamTable::getHeaderName
//------------------------------------------------------------------------
std::string ParamTable::getHeaderName(Column iColumn) const
{
  switch(iColumn)
  {
    case Column::kID:
      return "ID";
    case Column::kTitle:
      return "TITLE";
    case Column::kType:
      return "TYP";
    case Column::kOwner:
      return "OW";
    case Column::kTransient:
      return "TRS";
    case Column::kDefault:
      return "DEF.V";
    case Column::kDefaultAsString:
      return "DEF.S";
    case Column::kValue:
      return "VAL.V";
    case Column::kValueAsString:
      return "VAL.S";
    case Column::kSteps:
      return "STP";
    case Column::kFlags:
      return "FLG";
    case Column::kShortTitle:
      return "SHORT";
    case Column::kPrecision:
      return "PRE";
    case Column::kUnitID:
      return "UID";
    case Column::kUnits:
      return "UNS";
    case Column::kShared:
      return "SHA";
    default:
      return "";
  }
}

//------------------------------------------------------------------------
// ParamTable::toString(ParamValue)
//------------------------------------------------------------------------
std::string ParamTable::toString(ParamValue iValue) const
{
  std::ostringstream s;

  s.precision(fPrecision);
  s.setf(std::ios::fixed);
  s << iValue;

  return s.str();
}

using Column = ParamTable::Column;

std::vector<Column> DEFAULT_VST_COLUMNS { // NOLINT
  Column::kID, Column::kTitle, Column::kType, Column::kOwner, Column::kTransient, Column::kShared,
  Column::kDefault, Column::kDefaultAsString,
  Column::kSteps, Column::kFlags, Column::kShortTitle,
  Column::kPrecision, Column::kUnitID, Column::kUnits
};

std::vector<Column> DEFAULT_STATE_COLUMNS { // NOLINT
  Column::kID, Column::kTitle, Column::kValue, Column::kValueAsString
};

//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(Parameters const &iParams)
{
  return ParamTable{}
    .columns(DEFAULT_VST_COLUMNS)
    .ids(iParams.getAllRegistrationOrder())
    .rows(iParams)
    ;
}

//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(RT::RTState const *iState, bool iSaveStateOnly)
{
  return ParamTable{}
    .columns(DEFAULT_STATE_COLUMNS)
    .ids(iSaveStateOnly ? iState->fPluginParameters.getRTSaveStateOrder().fOrder : iState->getAllRegistrationOrder())
    .rows(iState)
    ;
}

//------------------------------------------------------------------------
// ParamTable::from
//------------------------------------------------------------------------
ParamTable ParamTable::from(GUI::GUIState const *iState, bool iSaveStateOnly)
{
  std::vector<ParamID> ids;
  if(iSaveStateOnly)
  {
    ids = iState->fPluginParameters.getGUISaveStateOrder().fOrder;
  }
  else
  {
    // ALL vst parameters are accessible from gui state (no registration required)
    ids = iState->fPluginParameters.getVstRegistrationOrder();
    ids.insert(ids.end(), iState->getAllRegistrationOrder().cbegin(), iState->getAllRegistrationOrder().cend());
  }

  return ParamTable{}
    .columns(DEFAULT_STATE_COLUMNS)
    .ids(ids)
    .rows(iState)
    ;
}


}
}
}