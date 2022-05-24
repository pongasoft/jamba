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

#include <vstgui4/vstgui/lib/cbitmap.h>
#include <vstgui4/vstgui/lib/cfont.h>
#include <vstgui4/vstgui/lib/vstkeycode.h>
#include <pongasoft/Utils/Lerp.h>

#if VSTGUI_LIVE_EDITING
#define EDITOR_MODE 1
#endif

namespace VSTGUI {
/**
 * VSTGUI defines a `CMouseEventResult` enum for mouse handling but no enumeration for keyboard events which seems
 * to be an oversight. Adding it for convenience.
 */
enum CKeyboardEventResult : int32_t
{
  kKeyboardEventNotHandled = -1,
  kKeyboardEventHandled = 1
};

}

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

// Defines a Bitmap types shortcut notation
using BitmapPtr = CBitmap *;
using BitmapSPtr = SharedPointer<CBitmap>;

// Defines a Font types shortcut notation
using FontPtr = CFontDesc *;
using FontSPtr = SharedPointer<CFontDesc>;

// Defines a Gradient types shortcut notation
using GradientPtr = CGradient *;
using GradientSPtr = SharedPointer<CGradient>;

/**
 * Defines a Range
 */
using Range = Utils::Range<CCoord>;

/**
 * Used to register global keyboard hooks */
using KeyboardEventCallback = std::function<void(KeyboardEvent &)>;

}
}
}