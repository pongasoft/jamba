/*
 * Copyright (c) 2018-2019 pongasoft
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
#include "CustomViewFactory.h"
#include "CustomView.h"
#include "CustomViewLifecycle.h"
#include <vstgui4/vstgui/lib/cview.h>

namespace pongasoft::VST::GUI::Views {

using namespace Params;

//------------------------------------------------------------------------
// CustomUIViewFactory::applyAttributeValues
//------------------------------------------------------------------------
bool CustomUIViewFactory::applyAttributeValues(CView *view,
                                               const UIAttributes &attributes,
                                               const IUIDescription *desc) const
{
  auto res = UIViewFactory::applyAttributeValues(view, attributes, desc);

  auto lifecycle = dynamic_cast<ICustomViewLifecycle *>(view);
  if(lifecycle)
    lifecycle->afterApplyAttributes();

  return res;
}

//------------------------------------------------------------------------
// CustomUIViewFactory::applyCustomViewAttributeValues
//------------------------------------------------------------------------
bool CustomUIViewFactory::applyCustomViewAttributeValues(CView *customView,
                                                         IdStringPtr baseViewName,
                                                         const UIAttributes &attributes,
                                                         const IUIDescription *desc) const
{
  auto res = UIViewFactory::applyCustomViewAttributeValues(customView, baseViewName, attributes, desc);

  auto lifecycle = dynamic_cast<ICustomViewLifecycle *>(customView);
  if(lifecycle)
    lifecycle->afterApplyAttributes();

  return res;
}

//------------------------------------------------------------------------
// CustomUIViewFactory::createView
//------------------------------------------------------------------------
CView *CustomUIViewFactory::createView(const UIAttributes &attributes, const IUIDescription *description) const
{
  auto view = UIViewFactory::createView(attributes, description);

  auto paramAware = dynamic_cast<GUIParamCxAware *>(view);
  if(paramAware)
    paramAware->initState(fGUIState);

  auto lifecycle = dynamic_cast<ICustomViewLifecycle *>(view);
  if(lifecycle)
    lifecycle->afterApplyAttributes();

  return view;
}

}