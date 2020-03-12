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
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

namespace pongasoft::Utils {

//! Convenient call to convert a boolean into a string
constexpr char const *to_string(bool iValue) { return iValue ? "true" : "false"; }

/**
 * Split a string according to a delimiter and writes the result into Out (can be a vector, array, etc...)
 *
 * @param iString the string to split
 * @param iDelimiter the delimiter
 * @param oResult the result
 * @param iSkipEmptyEntries whether to include or not empty entries
 */
template<typename Out>
void splitString(const std::string &iString, char iDelimiter, Out oResult, bool iSkipEmptyEntries = false)
{
  std::stringstream ss(iString);
  std::string item;
  while(std::getline(ss, item, iDelimiter))
  {
    if(!item.empty() || !iSkipEmptyEntries)
      *(oResult++) = item;
  }
  if(!iSkipEmptyEntries)
  {
    if(!iString.empty() && iString[iString.size() - 1] == iDelimiter)
      *oResult = "";
  }
}

/**
 * Split a string according to a delimiter and returns a vector
 *
 * @param iString the string to split
 * @param iDelimiter the delimiter
 * @param iSkipEmptyEntries whether to include or not empty entries
 */
std::vector<std::string> splitString(const std::string &iString, char iDelimiter, bool iSkipEmptyEntries = false);

/**
 * Converts the string to a TFloat (float or double)
 *
 * @return true if it could be extracted in which case oValue will contain the float, false otherwise
 */
template<typename TFloat>
bool stringToFloat(const std::string &iString, TFloat &oValue)
{
  char *endPtr = nullptr;
  auto value = static_cast<TFloat>(strtod(iString.c_str(), &endPtr));
  if(endPtr == iString.c_str())
    return false;
  oValue = value;
  return true;
}

/**
 * Converts the string to a TFloat
 *
 * @return the float or nan if not a number
 */
template<typename TFloat>
TFloat stringToFloat(const std::string &iString)
{
  char *endPtr = nullptr;
  auto value = static_cast<TFloat>(strtod(iString.c_str(), &endPtr));
  if(endPtr == iString.c_str())
    return std::numeric_limits<TFloat>::quiet_NaN();
  return value;
}

/**
 * Converts the string to a TFloat
 *
 * @return the float or nan if not a number
 */
template<typename TFloat>
std::vector<TFloat> splitFloats(const std::string &iString, char iDelimiter, bool iSkipEmptyEntries = false)
{
  static auto f = [] (const std::string &iString) -> TFloat { return stringToFloat<TFloat>(iString); };

  std::vector<TFloat> res{};

  auto strings = splitString(iString, iDelimiter, iSkipEmptyEntries);
  if(strings.empty())
    return res;

  res.reserve(strings.size());

  std::transform(strings.cbegin(), strings.cend(), std::back_inserter(res), f);

  return res;
}

}
