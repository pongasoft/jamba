/*
 * Copyright (c) 2020 pongasoft
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

#ifndef JAMBA_MODALVIEWCONTAINER_H
#define JAMBA_MODALVIEWCONTAINER_H

#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft::VST::GUI::Views {

/**
 * The purpose of this container is to be positioned on top of other views while making sure that any event
 * (for example mouse clicks) are not sent to the views below: it acts in a modal fashion. In other words,
 * all children of this container will receive events while siblings of this container will not.
 *
 * This class exposes the `CViewContainer` attributes only.
 */
class ModalViewContainer : public CustomViewAdapter<CViewContainer>
{
public:
  // Lifecycle
  explicit ModalViewContainer(const CRect &iSize) : CustomViewAdapter(iSize) {}

  ~ModalViewContainer() override = default;

  //! Send event to child and always return `kMouseEventHandled`
  CMouseEventResult onMouseDown(CPoint &where, CButtonState const &buttons) override
  {
    CViewContainer::onMouseDown(where, buttons);
    return kMouseEventHandled;
  }

  //! Send event to child and always return `kMouseEventHandled`
  CMouseEventResult onMouseUp(CPoint &where, CButtonState const &buttons) override
  {
    CViewContainer::onMouseUp(where, buttons);
    return kMouseEventHandled;
  }

  //! Send event to child and always return `kMouseEventHandled`
  CMouseEventResult onMouseMoved(CPoint &where, CButtonState const &buttons) override
  {
    CViewContainer::onMouseMoved(where, buttons);
    return kMouseEventHandled;
  }

  //! Send event to child and always return `kMouseEventHandled`
  CMouseEventResult onMouseCancel() override
  {
    CViewContainer::onMouseCancel();
    return kMouseEventHandled;
  }

  //! Send event to child and always return `1`
  int32_t onKeyDown(VstKeyCode &keyCode) override
  {
    CView::onKeyDown(keyCode);
    return 1;
  }

  //! Send event to child and always return `1`
  int32_t onKeyUp(VstKeyCode &keyCode) override
  {
    CView::onKeyUp(keyCode);
    return 1;
  }

public:
  class Creator : public CustomViewCreator<ModalViewContainer, CustomViewAdapter<CViewContainer>>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCViewContainer)
    {
    }
  };
};

}

#endif //JAMBA_MODALVIEWCONTAINER_H
