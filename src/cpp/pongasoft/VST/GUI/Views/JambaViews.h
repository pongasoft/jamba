#pragma once

#include "ToggleButtonView.h"
#include "MomentaryButtonView.h"
#include "TextEditView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

/**
 * Defines/Registers the views exported by the framework (since jamba is a static library defining the creators
 * as global variables does not work).
 */
struct JambaViews : public FObject
{
  SINGLETON(JambaViews);

  const ToggleButtonView::Creator fToggleButtonCreator{"jamba::ToggleButton", "Jamba - Toggle Button (on/off)"};
  const MomentaryButtonView::Creator fMomentaryButtonCreator{"jamba::MomentaryButton", "Jamba - Momentary Button (on when pressed)"};
  const TextEditView::Creator fTextEditCreator{"jamba::TextEdit", "Jamba - Text Edit (user input string)"};
};

}
}
}
}