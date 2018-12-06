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

#include <vstgui4/vstgui/lib/controls/cbuttons.h>
#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;
using namespace Params;

/**
 * Extends the CTextButton view to:
 * - have a (potentially) dynamic title tied to a parameter (fText)
 * - handle click conveniently (either inherit from this class and implement onClick, or provide an onClick listener)
 */
class TextButtonView : public CustomViewAdapter<CTextButton>
{
public:
  using OnClickListener = std::function<void()>;

  // Constructor
  explicit TextButtonView(const CRect &iSize) : CustomViewAdapter(iSize)
  {}

  // setTag => overridden to be able to change the param (in the editor)
  void setTag(int32_t val) override;

  // registerParameters
  void registerParameters() override;

  // handles value change
  void valueChanged() override;

  // callback when the button is clicked
  virtual void onClick();

  // setOnClickListener
  void setOnClickListener(OnClickListener iListener) { fOnClickListener = std::move(iListener); }

  CLASS_METHODS_NOCOPY(TextButtonView, CustomViewAdapter<CTextButton>)

protected:
  // the underlying jmb parameter of type UTF8String
  GUIJmbParam<UTF8String> fTitle{};

  // the onclick listener
  OnClickListener fOnClickListener{};

public:
  class Creator : public TCustomViewCreator<TextButtonView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      TCustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCTextButton)
    {
    }
  };
};

}
}
}
}

