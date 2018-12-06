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

#include <vstgui4/vstgui/lib/vstguibase.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

/**
 * Margin is a similar concept to css: used to create space around elements, outside of any defined borders.
 */
struct Margin
{
  // Empty constructor (no margin)
  Margin() = default;

  // Constructor. Assign same value to all components
  explicit Margin(CCoord iValue) noexcept :
    fTop{iValue}, fRight{iValue}, fBottom{iValue}, fLeft{iValue} {}

  // Constructor
  Margin(CCoord iTop, CCoord iRight, CCoord iBottom, CCoord iLeft) noexcept :
    fTop{iTop}, fRight{iRight}, fBottom{iBottom}, fLeft{iLeft} {}

  CCoord fTop{};
  CCoord fRight{};
  CCoord fBottom{};
  CCoord fLeft{};
};

}
}
}