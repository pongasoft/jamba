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

#include "SwitchViewContainer.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

//------------------------------------------------------------------------
// SwitchViewContainer::SwitchViewContainer
//------------------------------------------------------------------------
SwitchViewContainer::SwitchViewContainer(const CRect &iSize) : CustomViewAdapter(iSize)
{
  registerViewContainerListener(this);
}

//------------------------------------------------------------------------
// SwitchViewContainer::~SwitchViewContainer
//------------------------------------------------------------------------
SwitchViewContainer::~SwitchViewContainer()
{
  unregisterViewContainerListener(this);
  setCurrentView(nullptr);
}

//------------------------------------------------------------------------
// SwitchViewContainer::afterCreate
//------------------------------------------------------------------------
void SwitchViewContainer::afterCreate(IUIDescription const *iDescription, IController *iController)
{
  fUIDescription = iDescription;
  fUIController = iController;
}

//------------------------------------------------------------------------
// SwitchViewContainer::registerParameters
//------------------------------------------------------------------------
void SwitchViewContainer::registerParameters()
{
  fControlSwitch = registerRawVstParam(getSwitchControlTag());

#ifndef NDEBUG
  if(fControlSwitch.exists())
  {
    if(fControlSwitch.getStepCount() == 0)
      DLOG_F(WARNING, "Param [%d] should be a discrete parameter (stepCount != 0) to be used as the switch control",
             getSwitchControlTag());
  }
#endif

  switchCurrentView();
}

//------------------------------------------------------------------------
// SwitchViewContainer::switchCurrentView
//------------------------------------------------------------------------
void SwitchViewContainer::switchCurrentView()
{
  if(fControlSwitch.exists())
  {
    auto index = convertNormalizedValueToDiscreteValue(fControlSwitch.getStepCount(), fControlSwitch);
    auto templateName = UTF8String(computeTemplateName(index));

    if(templateName != fCurrentTemplateName)
    {
      setCurrentView(fUIDescription->createView(templateName, fUIController));
      fCurrentTemplateName = templateName;
      invalid();
    }
  }
  else
  {
    setCurrentView(nullptr);
    fCurrentTemplateName = "";
    invalid();
  }
}

//------------------------------------------------------------------------
// SwitchViewContainer::switchCurrentView
//------------------------------------------------------------------------
void SwitchViewContainer::onParameterChange(ParamID iParamID)
{
  switchCurrentView();
  CustomViewAdapter::onParameterChange(iParamID);
}


//------------------------------------------------------------------------
// SwitchViewContainer::setCurrentView
//------------------------------------------------------------------------
void SwitchViewContainer::setCurrentView(CView *iCurrentView)
{
  if(fCurrentView == iCurrentView)
    return;

  if(fCurrentView)
    removeView(fCurrentView);

  fCurrentView = iCurrentView;

  if(fCurrentView)
    addView(fCurrentView);
}

//------------------------------------------------------------------------
// SwitchViewContainer::viewContainerViewAdded
//------------------------------------------------------------------------
void SwitchViewContainer::viewContainerViewAdded(CViewContainer * /* unused */, CView *iView)
{
  if(iView != fCurrentView)
  {
    DLOG_F(WARNING, "SwitchViewContainer has children...");
    removeView(iView);
  }
}

//------------------------------------------------------------------------
// SwitchViewContainer::computeTemplateName
//------------------------------------------------------------------------
std::string SwitchViewContainer::computeTemplateName(int iIndex)
{
  if(iIndex >= 0 && iIndex < fTemplateNames.size())
    return fTemplateNames[iIndex];

  return "";
}

//------------------------------------------------------------------------
// createCustomView<SwitchViewContainer>
//------------------------------------------------------------------------
template<>
SwitchViewContainer *createCustomView<SwitchViewContainer>(CRect const &iSize,
                                                           const UIAttributes &iAttributes,
                                                           const IUIDescription *iDescription)
{
  auto view = new SwitchViewContainer(iSize);
  // YP Impl note: for some reason iDescription->getController() becomes nullptr later so need to
  // keep a handle of it right away (see similar code in vstsdk: uiviewcreator.cpp line 3667)
  // 		new UIDescriptionViewSwitchController (vsc, description, description->getController ());
  view->afterCreate(iDescription, iDescription->getController());
  return view;
}

}
}
}
}
