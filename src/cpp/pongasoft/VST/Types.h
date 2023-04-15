/*
 * Copyright (c) 2019-2023 pongasoft
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
#include <pluginterfaces/base/ftypes.h>
#include <pluginterfaces/vst/vsttypes.h>

namespace pongasoft::VST {

using namespace Steinberg::Vst;

/**
 * Strings made of `char16` characters are represented by the native C++11 type
 * `std::basic_string<Steinberg::char16>` and properly converted to what is required by the %VST classes.
 * The `Steinberg::String` class can be used to generate a formatted string like this:
 *
 * ```
 * int pad = 3;
 * String title;
 * title.printf(STR16("Pad [%d]"), pad);
 *
 * // because std::basic_string<Steinberg::char16> can be created from a char16 const *,
 * // then you can simply use title.text16() wherever a VstString16 is requested
 * myParam = vst<BooleanParamConverter>(id, title.text16()).add();
 * ```
 */
using VstString16 = std::basic_string<Steinberg::char16>;

/**
 * Constant used throughout the code to test whether the `ParamID` represents a valid id or an undefined one */
constexpr ParamID UNDEFINED_PARAM_ID = static_cast<ParamID>(-1);

/**
 * Defining a type for tags.
 *
 * @warning Since 4.0.0, this type has changed to be non negative. The issue stems from the fact that the %VST SDK uses
 *          `ParamID` throughout and is mapped to an `uint32` **but** the %VSTGUI layer uses `int32_t` for what they
 *          call tags (ex: `CControl::getTag()`) but is in fact a parameter ID, hence a `uint32` (essentially this is
 *          a mess...). So reverting to use what the %VST SDK deals with: `uint32`
 */
using TagID = ParamID;

/**
 * Constant used to test whether the `TagID` represents a valid id or an undefined one */
constexpr TagID UNDEFINED_TAG_ID = UNDEFINED_PARAM_ID;
}