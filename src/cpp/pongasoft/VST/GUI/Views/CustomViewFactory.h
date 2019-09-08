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

#include <vstgui4/vstgui/uidescription/uiviewfactory.h>
#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft::VST::GUI::Views {

using namespace Params;

/**
 * Custom view factory to give access to vst parameters
 */
class CustomUIViewFactory : public VSTGUI::UIViewFactory
{
public:
  explicit CustomUIViewFactory(GUIState *iGUIState) : fGUIState{iGUIState}
  {
  }

protected:
  // overridden to detect GUIParamCxAware instances
  bool applyAttributeValues(CView *view, const UIAttributes &attributes, const IUIDescription *desc) const override;

  // overridden to detect GUIParamCxAware instances
  bool applyCustomViewAttributeValues(CView *customView,
                                      IdStringPtr baseViewName,
                                      const UIAttributes &attributes,
                                      const IUIDescription *desc) const override;

  // overridden to detect GUIParamCxAware instances
  CView *createView(const UIAttributes &attributes, const IUIDescription *description) const override;

private:
  GUIState *fGUIState{};
};


}