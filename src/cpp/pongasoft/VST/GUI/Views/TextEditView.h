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

#include <vstgui4/vstgui/lib/controls/ctextedit.h>
#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;
using namespace Params;

/**
 * The purpose of this class is to adapt the constructor signature to a single element, required in the framework */
class TextEdit : public CTextEdit
{
public:
  explicit TextEdit(const CRect &iSize) : CTextEdit(iSize, nullptr, -1) {};
};

/**
 * Extends the CTextEdit view to tie it to a GUIJmbParam<UTF8String> => multiple views tied to the same paramID (aka Tag)
 * are all synchronized and the text edit value will be saved and restored with the state.
 */
class TextEditView : public CustomViewAdapter<TextEdit>
{
public:
  // Constructor
  explicit TextEditView(const CRect &iSize) : CustomViewAdapter(iSize)
  {}

  // setTag => overridden to be able to change the param (in the editor)
  void setTag(int32_t val) override;

  // registerParameters
  void registerParameters() override;

  // valueChanged
  void valueChanged() override;

  // onParameterChange
  void onParameterChange(ParamID iParamID) override;

  CLASS_METHODS_NOCOPY(TextEditView, CustomViewAdapter<TextEdit>)

protected:
  // the underlying jmb parameter of type UTF8String
  GUIJmbParam<UTF8String> fText{};

public:
  class Creator : public CustomViewCreator<TextEditView, CustomViewAdapter<TextEdit>>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCTextEdit)
    {
    }
  };
};

}
}
}
}

