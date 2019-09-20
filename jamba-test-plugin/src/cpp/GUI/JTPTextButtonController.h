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

#pragma once

#include <pongasoft/VST/GUI/Views/CustomController.h>
#include <pongasoft/VST/GUI/Views/TextButtonView.h>
#include "../Plugin.h"

namespace pongasoft::test::jamba::GUI {

using namespace pongasoft::VST::GUI;

/**
 * Controller which handles adding listener to the text buttons */
class JTPTextButtonController : public Views::PluginCustomController<JambaTestPluginGUIState>
{
public:
  explicit JTPTextButtonController(IController *iBaseController) : PluginCustomController(iBaseController) {}

  // verifyView
  CView *verifyView(CView *iView, const UIAttributes &attributes, const IUIDescription *description) override
  {
    auto button = dynamic_cast<Views::TextButtonView *>(iView);

    if(button)
    {
      switch(button->getCustomViewTag())
      {
        case EJambaTestPluginParamID::kButton1Action:
          addDisableStateHandling(button);
          addListener(button, "Button1");
          break;

        case EJambaTestPluginParamID::kButton2Action:
          addDisableStateHandling(button);
          addListener(button, "Button2");
          break;

        case EJambaTestPluginParamID::kButton3Action:
          addListener(button, "Button3");
          break;

        case EJambaTestPluginParamID::kButton4Action:
          addListener(button, "Button4");
          break;

        case EJambaTestPluginParamID::kButton5Action:
          addListener(button, "Button5");
          break;

        default:
          // do nothing
          break;
      }
    }

    return iView;
  }

private:
  // addListener -> prints message when button clicked
  void addListener(Views::TextButtonView *iButton, std::string const &iButtonName)
  {
    iButton->setOnClickListener([this, iButtonName] {
      fState->fUTF8String.update(msg(iButtonName));
    });
  }

  // addDisableStateHandling -> button can be disabled based on state of fState->fBoolJmb
  void addDisableStateHandling(Views::TextButtonView *iButton)
  {
    registerConnectionFor(iButton)->registerCallback<bool>(fState->fBoolJmb,
                                                           [](Views::TextButtonView *iButton, GUIJmbParam<bool> &iParam) {
                                                             iButton->setMouseEnabled(iParam);
                                                           },
                                                           true);

  }

  // appends a counter to the original message
  std::string msg(std::string const &iMsg)
  {
    std::stringstream s;
    s << iMsg << " [" << fCounter++ << "]";
    return s.str();
  }

private:
  int fCounter{};
};

}