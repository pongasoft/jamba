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

#include <pluginterfaces/vst/ivstaudioprocessor.h>

#include <cmath>
#include <type_traits>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

#define BIT_SET(a,b) ((a) |= (static_cast<std::make_unsigned_t<decltype(a)>>(1)<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(static_cast<std::make_unsigned_t<decltype(a)>>(1)<<(b)))
#define BIT_TEST(a,b) (((a) & (static_cast<std::make_unsigned_t<decltype(a)>>(1)<<(b))) != 0)

//! Sets bit `bit` in `a`
template<typename T>
constexpr T bitSet(T a, int bit) { return a | (static_cast<std::make_unsigned_t<T>>(1) << bit); }

//! Clears bit `bit` in `a`
template<typename T>
constexpr T bitClear(T a, int bit) { return a & ~(static_cast<std::make_unsigned_t<T>>(1) << bit); }

//! Test if bit `bit` is set in `a`
template<typename T>
constexpr bool bitTest(T a, int bit) { return (a & (static_cast<std::make_unsigned_t<T>>(1) << bit)) != 0; }

// defines the threshold of silence as constants
constexpr Sample32 Sample32SilentThreshold = ((Sample32)2.0e-8);
constexpr Sample64 Sample64SilentThreshold = ((Sample64)2.0e-8);

// defines the threshold of silence as a templated method
template<typename SampleType>
SampleType getSampleSilentThreshold() noexcept;
template<>
constexpr Sample32 getSampleSilentThreshold<Sample32>() noexcept { return Sample32SilentThreshold; }
template<>
constexpr Sample64 getSampleSilentThreshold<Sample64>() noexcept { return Sample64SilentThreshold; }

//------------------------------------------------------------------------
// check if sample is silent (lower than threshold) Sample32 version
//------------------------------------------------------------------------
inline bool isSilent(Sample32 value)
{
  if(value < 0)
    value = -value;
  return value <= Sample32SilentThreshold;
}

//------------------------------------------------------------------------
// check if sample is silent (lower than threshold) Sample64 version
//------------------------------------------------------------------------
inline bool isSilent(Sample64 value)
{
  if(value < 0)
    value = -value;
  return value <= Sample64SilentThreshold;
}

//------------------------------------------------------------------------
// dbToSample
//------------------------------------------------------------------------
template<typename SampleType>
inline SampleType dbToSample(double valueInDb)
{
  return static_cast<SampleType>(std::pow(10.0, valueInDb / 20.0));
}

//------------------------------------------------------------------------
// sampleToDb
//------------------------------------------------------------------------
template<typename SampleType>
inline double sampleToDb(SampleType valueInSample)
{
  return std::log10(valueInSample) * 20.0;
}

}
}
