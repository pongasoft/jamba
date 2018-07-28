#pragma once

#include <vstgui4/vstgui/lib/cbitmap.h>
#include <vstgui4/vstgui/lib/cfont.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

// Defines a BitmapPtr shortcut notation
using BitmapPtr = std::shared_ptr<CBitmap>;

// Defines a FontPtr shortcut notation
using FontPtr = std::shared_ptr<CFontDesc>;


}
}
}