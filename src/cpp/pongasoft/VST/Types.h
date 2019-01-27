/*
 * Copyright (c) 2019 pongasoft
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

#include <string>

namespace pongasoft {
namespace VST {

/**
 * Strings made of char16 characters are represented by the native C++11 type `std::u16string` and properly converted
 * to what is required by the VST classes. The `Steinberg::String` class can be used to generate a formatted string like
 * this:
 *
 *     int pad = 3;
 *     String title;
 *     title.printf(STR16("Pad [%d]", pad);
 *     // because std::u16string can be created from a char16 const *, then you can simply
 *     // use title.text16() wherever a VstString16 is requested
 *     myParam = vst<BooleanParamConverter>(id, title.text16()).add();
 */
using VstString16 = std::u16string;

}
}