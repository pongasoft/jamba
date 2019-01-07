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
#pragma once

#include "Misc.h"

namespace pongasoft {
namespace Utils {

/**
 * Util class to compute linear interpolation. Use SPLerp/DPLerp and mapValueSP/mapRangeSP (resp mapValueDP/mapRangeDP)
 * for convenience.
 *
 * Providing iX1 == iX2 will result in undefined behavior
 */
template <typename TFloat, typename X, typename Y>
class Lerp
{
public:
  Lerp(X iX1, Y iY1, X iX2, Y iY2) :
    fA((static_cast<TFloat>(iY1 - iY2)) / (static_cast<TFloat>(iX1 - iX2))),
    fB(static_cast<TFloat>(iY1) - fA * static_cast<TFloat>(iX1)) {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  Lerp(Y iY0, Y iY1) : fA(static_cast<TFloat>(iY1 - iY0)), fB(static_cast<TFloat>(iY0)) {};

  inline Y computeY(X iX) const
  {
    return static_cast<Y>((static_cast<TFloat>(iX) * fA) + fB);
  }

  inline X computeX(Y iY) const
  {
    DCHECK_F(fA != 0);
    return static_cast<X>((static_cast<TFloat>(iY) - fB) / fA);
  }

  /**
   * Inspired by the map function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: [iFromLow, iFromHigh] -> [iToLow, iToHigh]. Note that low can be greater than high: for
   * example you can map [1, -1] to the range [0, height] (display where 0 is at the top and height is the bottom)
   */
  static inline Lerp mapRange(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
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
  static Y mapValue(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
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

    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh).computeY(iValue);
  }

private:
  const TFloat fA;
  const TFloat fB;
};

//------------------------------------------------------------------------
// SPLerp - Single Precision Lerp (float)
//------------------------------------------------------------------------
template<typename X, typename Y>
using SPLerpXY = Lerp<float, X, Y>;

template<typename X>
using SPLerpX = Lerp<float, X, float>;

template<typename Y>
using SPLerpY = Lerp<float, float, Y>;

using SPLerp = Lerp<float, float, float>;

/**
 * Convenient shortcut for single precision. See Lerp::mapRange */
template<typename X, typename Y>
static inline SPLerpXY<X, Y> mapRangeSPXY(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
{
  return SPLerpXY<X, Y>(iFromLow, iToLow, iFromHigh, iToHigh);
}

template<typename Y>
constexpr auto mapRangeSPY = mapRangeSPXY<float, Y>;

template<typename X>
constexpr auto mapRangeSPX = mapRangeSPXY<X, float>;

constexpr auto mapRangeSP = mapRangeSPXY<float, float>;

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X, typename Y>
inline static Y mapValueSPXY(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return SPLerpXY<X, Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename Y>
inline static Y mapValueSPY(float iValue, float iFromLow, float iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return SPLerpY<Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X>
inline static float mapValueSPX(X iValue, X iFromLow, X iFromHigh, float iToLow, float iToHigh, bool iClamp = true)
{
  return SPLerpX<X>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
inline static float mapValueSP(float iValue, float iFromLow, float iFromHigh, float iToLow, float iToHigh, bool iClamp = true)
{
  return SPLerp::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}


//------------------------------------------------------------------------
// DPLerp - Double Precision Lerp (double)
//------------------------------------------------------------------------
template<typename X, typename Y>
using DPLerpXY = Lerp<double, X, Y>;

template<typename X>
using DPLerpX = Lerp<double, X, double>;

template<typename Y>
using DPLerpY = Lerp<double, double, Y>;

using DPLerp = Lerp<double, double, double>;

/**
 * Convenient shortcut for double precision. See Lerp::mapRange */
template<typename X, typename Y>
static inline DPLerpXY<X, Y> mapRangeDPXY(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
{
  return DPLerpXY<X, Y>(iFromLow, iToLow, iFromHigh, iToHigh);
}

template<typename Y>
constexpr auto mapRangeDPY = mapRangeDPXY<double, Y>;

template<typename X>
constexpr auto mapRangeDPX = mapRangeDPXY<X, double>;

constexpr auto mapRangeDP = mapRangeDPXY<double, double>;

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X, typename Y>
inline static Y mapValueDPXY(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return DPLerpXY<X, Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename Y>
inline static Y mapValueDPY(double iValue, double iFromLow, double iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return DPLerpY<Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X>
inline static double mapValueDPX(X iValue, X iFromLow, X iFromHigh, double iToLow, double iToHigh, bool iClamp = true)
{
  return DPLerpX<X>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
inline static double mapValueDP(double iValue, double iFromLow, double iFromHigh, double iToLow, double iToHigh, bool iClamp = true)
{
  return DPLerp::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

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
  template<typename U, typename TLerp = DPLerpXY<T, U>>
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
  template<typename U, typename TLerp = DPLerpXY<T, U>>
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
  template<typename U, typename TLerp = DPLerpXY<T, U>>
  inline Range<U> mapSubRange(Range<T> const &iSubRange, Range<U> const &iRange, bool iClampToRange = true) const
  {
    /// @todo optimize by computing `TLerp` only once
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
