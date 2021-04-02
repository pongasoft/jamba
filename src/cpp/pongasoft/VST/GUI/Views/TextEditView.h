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
#pragma once

#include <vstgui4/vstgui/lib/controls/ctextedit.h>
#include <vstgui4/vstgui/lib/controls/icontrollistener.h>
#include <pongasoft/VST/GUI/Views/CustomView.h>

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;
using namespace Params;

/**
 * The purpose of this class is to adapt the constructor signature to a single element, required in the framework */
class TextEdit : public CTextEdit
{
public:
  explicit TextEdit(const CRect &iSize) : CTextEdit(iSize, nullptr, UNDEFINED_PARAM_ID) {};
};

/**
 * Extends the `CTextEdit` view to tie it to a `GUIJmbParam<UTF8String>`.
 *
 * This allows to have multiple views tied to the same id which are all synchronized and the text edit
 * value will be saved and restored with the state. In essence this class behaves like "regular" Vst parameters but
 * for a `UTF8String` instead.
 *
 * This class does not have any specific attributes (besides the one exposed via `CTextEdit`).
 */
class TextEditView : public CustomViewAdapter<TextEdit>
{
public:
  // Constructor
  explicit TextEditView(const CRect &iSize) : CustomViewAdapter(iSize), fOnChangeListener{new CLabelListener(fText)}
  {
    registerControlListener(fOnChangeListener.get());
  }

  // registerParameters
  void registerParameters() override;

  // setListener
  void setListener(IControlListener *l) override;

protected:
  // the underlying jmb parameter of type UTF8String
  GUIJmbParam<UTF8String> fText{};

private:
  //! Control listener to handle changes to the `CTextLabel` and propagates to `fText`
  struct CLabelListener : public IControlListener {
    CLabelListener(GUIJmbParam<UTF8String> &iText) : fText{iText} {}
    void valueChanged(CControl *pControl) override;
    GUIJmbParam<UTF8String> &fText;
  };

  std::unique_ptr<CLabelListener> fOnChangeListener;


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

