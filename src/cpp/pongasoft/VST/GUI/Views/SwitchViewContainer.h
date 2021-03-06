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

#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft::VST::GUI::Views {

/**
 * This view offers dynamic switching between multiple views.
 *
 * Switching is based on the value of any parameter (both Vst and Jmb) that is (or can be interpreted as) a
 * discrete parameter.
 *
 * The %VST SDK comes with a similar implementation which a) is buggy, b) requires an actual control tied to a vst
 * parameter (so for example does not work with `StepButtonView` or cannot be changed by the %RT). This implementation
 * uses a parameter directly so has none of these restrictions.
 *
 * \note When editing the layout using the editor, and saving the xml file, unfortunately the editor will save the
 * children of this class (in this case, the one that was added dynamically). Although the code handles this case,
 * it is recommended (for production) to manually edit the xml file to remove any child of this entry (otherwise
 * objects will be created to be destroyed right away).
 *
 * In addition to the attributes exposed by `CViewContainer`, this class exposes the following attributes:
 *
 * Attribute            | Description
 * ---------            | -----------
 * `switch-control-tag` | @copydoc getSwitchControlTag()
 * `template-names`     | @copydoc getTemplateNames()
 */
class SwitchViewContainer : public CustomViewAdapter<CViewContainer>, ViewContainerListenerAdapter
{
public:
  // Lifecycle
  explicit SwitchViewContainer(const CRect &iSize);
  ~SwitchViewContainer() override;

  //! Attribute `switch-control-tag`
  virtual void setSwitchControlTag (ParamID iTag) { fSwitchControlTag = iTag; };

  /**
   * id for the parameter tied to switching. It can be any parameter (both Vst and Jmb) that is
   * (or can be interpreted as) a discrete parameter. */
  ParamID getSwitchControlTag () const { return fSwitchControlTag; }

  /**
   * A comma separated list of template names. Should refer to valid templates (in the xml file)
   *
   * @note You can use `_` for a template name which means displays nothing (make sure you set the container to
   *       transparent in this case). Can be used for overlay for example. */
  const std::vector<std::string> &getTemplateNames() const { return fTemplateNames; }
  void setTemplateNames(const std::vector<std::string> &iNames) { fTemplateNames = iNames; switchCurrentView(); }

  // registerParameters
  void registerParameters() override;

  // afterCreate
  virtual void afterCreate(IUIDescription const *iDescription, IController *iController);

  // onParameterChange
  void onParameterChange(ParamID iParamID) override;

protected:
  /**
   * The only purpose of this callback (from `IViewContainerListenerAdapter`) is to catch the unavoidable cases
   * when the editor will save the children of this class as there does not seem to be a way to prevent this
   * behavior from happening.
   */
  void viewContainerViewAdded(CViewContainer *container, CView *view) override;

  /**
   * Called whenever something has changed to switch to a new current view */
  virtual void switchCurrentView();

  /**
   * Sets the current view to the one provided (remove previous view from container children and add this one).
   * Handles no view properly.
   */
  virtual void setCurrentView(CView *iCurrentView);

  /**
   * Lookup the name of the template in the vector. Can be overridden to implement different behavior
   */
  virtual std::string computeTemplateName(int iIndex);

protected:
  IUIDescription const *fUIDescription{};
  IController *fUIController{};

  ParamID fSwitchControlTag{UNDEFINED_PARAM_ID};
  GUIOptionalParam<int32> fControlSwitch{};

  std::vector<std::string> fTemplateNames;

  CView *fCurrentView{};
  std::string fCurrentTemplateName{};

public:
  class Creator : public CustomViewCreator<SwitchViewContainer, CustomViewAdapter<CViewContainer>>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCViewContainer)
    {
      registerTagAttribute("switch-control-tag", &SwitchViewContainer::getSwitchControlTag, &SwitchViewContainer::setSwitchControlTag);
      registerVectorStringAttribute("template-names", &SwitchViewContainer::getTemplateNames, &SwitchViewContainer::setTemplateNames);
    }
  };
};

/**
 * This specialization is required to inject the `IUIDescription` object used to dynamically create a view
 * from a template name.
 */
template<>
SwitchViewContainer *createCustomView<SwitchViewContainer>(CRect const &iSize,
                                                           const UIAttributes &iAttributes,
                                                           const IUIDescription *iDescription);

}