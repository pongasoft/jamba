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

  ValueMap header = fDisplayHeader ? computeHeader() : ValueMap{};

  ParamMap paramMap = fParamDisplay.getParamMap();

  std::map<Key, std::string::size_type> sizes{};
  std::string::size_type totalSize = computeSizes(header, paramMap, sizes);

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
    auto const &param = paramMap.at(paramID);

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

}
}
}