#pragma once

#include <vstgui4/vstgui/lib/cbitmap.h>
#include <vstgui4/vstgui/lib/cfont.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

// Defines a BitmapSPtr shortcut notation
using BitmapPtr = CBitmap *;
using BitmapSPtr = SharedPointer<CBitmap>;

// Defines a FontSPtr shortcut notation
using FontPtr = CFontDesc *;
using FontSPtr = SharedPointer<CFontDesc>;


}
}
}