/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

#include <vstgui4/vstgui/uidescription/uiviewfactory.h>
#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace Params;

/**
 * interface to access gui state
 */
class GUIStateProvider
{
public:
  virtual GUIState *getGUIState() const = 0;
};

/**
 * Custom view factory to give access to vst parameters
 */
class CustomUIViewFactory : public VSTGUI::UIViewFactory, public GUIStateProvider
{
public:
  explicit CustomUIViewFactory(GUIState *iGUIState) : fGUIState{iGUIState}
  {
  }

  ~CustomUIViewFactory() override = default;

  GUIState *getGUIState() const override
  {
    return fGUIState;
  }

private:
  GUIState *fGUIState{};
};


}
}
}
}