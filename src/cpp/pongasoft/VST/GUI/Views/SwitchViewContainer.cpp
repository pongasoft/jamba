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

namespace pongasoft::VST::GUI::Views {

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
  // will be adjusted appropriately in setCurrentView
  setVisible(false);
}

//------------------------------------------------------------------------
// SwitchViewContainer::registerParameters
//------------------------------------------------------------------------
void SwitchViewContainer::registerParameters()
{
  fControlSwitch = registerOptionalDiscreteParam(getSwitchControlTag(), static_cast<int32>(fTemplateNames.size() - 1));

#ifndef NDEBUG
  if(fControlSwitch.getStepCount() == 0 && getSwitchControlTag() != UNDEFINED_PARAM_ID)
    DLOG_F(WARNING, "%d parameter is not discrete (stepCount == 0)", fControlSwitch.getParamID());
#endif

  switchCurrentView();
}

//------------------------------------------------------------------------
// SwitchViewContainer::switchCurrentView
//------------------------------------------------------------------------
void SwitchViewContainer::switchCurrentView()
{
  auto index = fControlSwitch.getValue();
  auto templateName = UTF8String(computeTemplateName(index));

  if(templateName != fCurrentTemplateName)
  {
    setCurrentView(templateName == "" || templateName == "_" ?
                   nullptr :
                   fUIDescription->createView(templateName, fUIController));
    fCurrentTemplateName = templateName;
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
  if(fCurrentView != iCurrentView)
  {
    if(fCurrentView)
      removeView(fCurrentView);

    fCurrentView = iCurrentView;

    if(fCurrentView)
      addView(fCurrentView);
  }

  // when there is no current view, we make this view invisible to make sure that whatever is below
  // is handled properly (like drag'n'drop)
  setVisible(fCurrentView != nullptr);
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
  auto index = static_cast<decltype(fTemplateNames.size())>(iIndex);
  if(index >= 0 && index < fTemplateNames.size())
    return fTemplateNames[index];

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
