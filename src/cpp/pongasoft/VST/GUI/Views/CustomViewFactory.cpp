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
#include "CustomViewFactory.h"
#include <vstgui4/vstgui/lib/cview.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace Params;

//------------------------------------------------------------------------
// CustomUIViewFactory::applyAttributeValues
//------------------------------------------------------------------------
bool CustomUIViewFactory::applyAttributeValues(CView *view,
                                               const UIAttributes &attributes,
                                               const IUIDescription *desc) const
{
  auto res = UIViewFactory::applyAttributeValues(view, attributes, desc);
  auto paramAware = dynamic_cast<GUIParamCxAware *>(view);
  if(paramAware)
    paramAware->registerParameters();
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
  auto paramAware = dynamic_cast<GUIParamCxAware *>(customView);
  if(paramAware)
    paramAware->registerParameters();
  return res;
}

//------------------------------------------------------------------------
// CustomUIViewFactory::createViewByName
//------------------------------------------------------------------------
CView *CustomUIViewFactory::createViewByName(const std::string *className,
                                             const UIAttributes &attributes,
                                             const IUIDescription *description) const
{
  auto view = UIViewFactory::createViewByName(className, attributes, description);
  auto paramAware = dynamic_cast<GUIParamCxAware *>(view);
  if(paramAware)
  {
    paramAware->initState(fGUIState);
    paramAware->registerParameters();
  }
  return view;
}

}
}
}
}