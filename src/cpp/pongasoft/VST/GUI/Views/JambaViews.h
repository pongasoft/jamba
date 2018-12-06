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

#include "ToggleButtonView.h"
#include "MomentaryButtonView.h"
#include "TextEditView.h"
#include "TextButtonView.h"

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
  const TextButtonView::Creator fTextButtonCreator{"jamba::TextButton", "Jamba - Text Button"};
};

}
}
}
}