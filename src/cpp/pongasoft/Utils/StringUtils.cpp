/*
 * Copyright (c) 2018 pongasoft
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
#include "StringUtils.h"

namespace pongasoft {
namespace Utils {

/**
 * Split a string according to a delimiter and returns a vector
 *
 * @param iString the string to split
 * @param iDelimiter the delimiter
 * @param iSkipEmptyEntries whether to include or not empty entries
 */
std::vector<std::string> splitString(const std::string &iString, char iDelimiter, bool iSkipEmptyEntries)
{
  std::vector<std::string> elems;
  splitString(iString, iDelimiter, std::back_inserter(elems), iSkipEmptyEntries);
  return elems;
}

}
}
