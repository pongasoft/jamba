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
 * Providing iX1 == iX2 will result in undefined behavior
 */
template <typename TFloat>
class Lerp
{
public:
  Lerp(TFloat iX1, TFloat iY1, TFloat iX2, TFloat iY2) : fA((iY1 - iY2) / (iX1 - iX2)), fB(iY1 - fA * iX1) {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  Lerp(TFloat iY0, TFloat iY1) : fA(iY1 - iY0), fB(iY0) {};

  template<typename T>
  inline T computeY(T iX) const
  {
    return static_cast<T>((iX * fA) + fB);
  }

  template<typename T>
  inline T computeX(T iY) const
  {
    DCHECK_F(fA != 0);
    return static_cast<T>((iY - fB) / fA);
  }

  /**
   * Inspired by the map function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: [iFromLow, iFromHigh] -> [iToLow, iToHigh]. Note that low can be greater than high: for
   * example you can map [1, -1] to the range [0, height] (display where 0 is at the top and height is the bottom)
   */
  static inline Lerp mapRange(TFloat iFromLow, TFloat iFromHigh, TFloat iToLow, TFloat iToHigh)
  {
    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh);
  }

  /**
   * Inspired by the map function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: [iFromLow, iFromHigh] -> [iToLow, iToHigh]. This function then return the iValue mapped from
   * the first range into the second range. When iClamp is set to true, the incoming value will first be clamped
   * to the from range. ex:
   *
   * mapValue(5, 10, 20, 100, 200, true) returns 100
   * mapValue(5, 10, 20, 100, 200, false) returns 50
   *
   * @param iValue will be constrained to the range [min(iFromLow, iFromHigh), max(iFromLow, iFromHigh)]
   */
  template<typename T, typename U>
  static U mapValue(T iValue, T iFromLow, T iFromHigh, U iToLow, U iToHigh, bool iClamp = true)
  {
    // if the first range is empty (computation would be dividing by 0)
    if(iFromLow == iFromHigh)
      return iValue <= iFromLow ? iToLow : iToHigh;

    // if the second range is empty, no need for computation
    if(iToLow == iToHigh)
      return iToLow;

    if(iClamp)
    {
      iValue = clampRange(iValue, iFromLow, iFromHigh);
    }

    return static_cast<U>(Lerp(static_cast<TFloat>(iFromLow), static_cast<TFloat>(iToLow), static_cast<TFloat>(iFromHigh), static_cast<TFloat>(iToHigh)).computeY(iValue));
  }

private:
  const TFloat fA;
  const TFloat fB;
};

//------------------------------------------------------------------------
// SPLerp - Single Precision Lerp (float)
//------------------------------------------------------------------------
using SPLerp = Lerp<float>;

//------------------------------------------------------------------------
// DPLerp - Double Precision Lerp (double)
//------------------------------------------------------------------------
using DPLerp = Lerp<double>;

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
   * Clamp the value to this range
   *
   * @return a value between fFrom and fTo
   */
  T clamp(T iValue) const
  {
    return Utils::clampRange(iValue, fFrom, fTo);
  }

  /**
   * Map the value from this range into the provide range
   *
   * @param iValue the value from *this* range [fFrom, fTo]
   * @param iRange the range to map the iValue into
   * @return the new value
   */
  template<typename U, typename TLerp = DPLerp>
  inline U mapValue(T iValue, Range<U> const &iRange, bool iClampToRange = true) const
  {
    return TLerp::mapValue(iValue, fFrom, fTo, iRange.fFrom, iRange.fTo, iClampToRange);
  }

  /**
   * Map this range to the other range
   *
   * @param iRange the range to map the iValue into
   * @return the new range
   */
  template<typename U, typename TLerp = DPLerp>
  inline Range<U> mapRange(Range<U> const &iRange, bool iClampToRange = true) const
  {
    return mapSubRange<U,TLerp>(*this, iRange, iClampToRange);
  }

  /**
   * Map a sub range of this range to the other range
   *
   * @param iRange the range to map the iValue into
   * @return the new range
   */
  template<typename U, typename TLerp = DPLerp>
  inline Range<U> mapSubRange(Range<T> const &iSubRange, Range<U> const &iRange, bool iClampToRange = true) const
  {
    // TODO: optimize by generating a computing TLerp only once
    return Range<U>{mapValue<U,TLerp>(iSubRange.fFrom, iRange, iClampToRange),
                    mapValue<U,TLerp>(iSubRange.fTo, iRange, iClampToRange)};
  }

  /**
   * Cast this range to another one
   */
  template<typename U>
  inline Range<U> cast() const
  {
    return Range<U>{static_cast<U>(fFrom), static_cast<U>(fTo)};
  }

  // operator==
  bool operator==(const Range &rhs) const
  {
    return fFrom == rhs.fFrom && fTo == rhs.fTo;
  }

  // operator!=
  bool operator!=(const Range &rhs) const
  {
    return !(rhs == *this);
  }

public:
  T fFrom{};
  T fTo{};
};

}
}
