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

#include "Misc.h"

namespace pongasoft {
namespace Utils {
/**
 * Util class to compute linear interpolation. Note that assembly code totally removes this class which is great!
 * ex: Utils::Lerp(MAX_ZOOM_FACTOR_Y, 1.0).computeY(fPropZoomFactorY.getValue());
 */
template <typename T>
class Lerp
{
public:
  Lerp(T iX1, T iY1, T iX2, T iY2) : fA((iY1 - iY2) / (iX1 - iX2)), fB(iY1 - fA * iX1) {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  Lerp(T iY0, T iY1) : fA(iY1 - iY0), fB(iY0) {};

  inline T computeY(T iX) const
  {
    return (iX * fA) + fB;
  }

  inline T computeX(T iY) const
  {
    DCHECK_F(fA != 0);
    return (iY - fB) / fA;
  }

  /**
   * Inspired by the map function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: [iFromLow, iFromHigh] -> [iToLow, iToHigh]. Note that low can be greater than high: for
   * example you can map [1, -1] to the range [0, height] (display where 0 is at the top and height is the bottom)
   */
  static inline Lerp mapRange(T iFromLow, T iFromHigh, T iToLow, T iToHigh)
  {
    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh);
  }

  /**
   * Inspired by the map function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: [iFromLow, iFromHigh] -> [iToLow, iToHigh]. This function then return the iValue mapped from
   * the first range into the second range.
   *
   * @param iValue will be constrained to the range [min(iFromLow, iFromHigh), max(iFromLow, iFromHigh)]
   */
  template<typename U>
  static U mapValue(T iValue, T iFromLow, T iFromHigh, U iToLow, U iToHigh)
  {
    // if the first range is empty (computation would be dividing by 0)
    if(iFromLow == iFromHigh)
      return iValue <= iFromLow ? iToLow : iToHigh;

    // if the second range is empty, no need for computation
    if(iToLow == iToHigh)
      return iToLow;

    if(iFromLow < iFromHigh)
      iValue = clamp(iValue, iFromLow, iFromHigh);
    else
      iValue = clamp(iValue, iFromHigh, iFromLow);
    

    return static_cast<U>(Lerp(iFromLow, static_cast<T>(iToLow), iFromHigh, static_cast<T>(iToHigh)).computeY(iValue));
  }

  /**
   * This is the eXtended version of mapValue which allow iValue to be outside the range. Example:
   * mapValue(5, 10, 20, 100, 200) returns 100
   * mapValueX(5, 10, 20, 100, 200) returns 50
   * @see mapValue
   */
  template<typename U>
  static inline U mapValueX(T iValue, T iFromLow, T iFromHigh, U iToLow, U iToHigh)
  {
    // if the first range is empty (computation would be dividing by 0)
    if(iFromLow == iFromHigh)
      return iValue <= iFromLow ? iToLow : iToHigh;

    // if the second range is empty, no need for computation
    if(iToLow == iToHigh)
      return iToLow;

    return static_cast<U>(Lerp(iFromLow, static_cast<T>(iToLow), iFromHigh, static_cast<T>(iToHigh)).computeY(iValue));
  }

private:
  const T fA;
  const T fB;
};

/**
 * Defines a range of values.
 */
template<typename T>
struct Range
{
  // Empty constructor (no range)
  Range() = default;

  // Single value range
  explicit Range(T iValue) noexcept : fFrom{iValue}, fTo{iValue} {}

  // Constructor
  Range(T iFrom, T iTo) : fFrom{iFrom}, fTo{iTo} {}

  // return true if the range is a single value (aka degenerate range)
  bool isSingleValue() const { return fFrom == fTo; }

  /**
   * Map the value from this range into the provide range
   *
   * @param iValue the value from *this* range [fFrom, fTo]
   * @param iRange the range to map the iValue into
   * @return the new value
   */
  template<typename U>
  inline U mapValue(T iValue, Range<U> const &iRange)
  {
    return Lerp<T>::mapValue(iValue, fFrom, fTo, iRange.fFrom, iRange.fTo);
  }

public:
  T fFrom{};
  T fTo{};
};

}
}
