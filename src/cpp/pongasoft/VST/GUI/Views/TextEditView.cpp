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

#include "TextEditView.h"
#include <vstgui4/vstgui/plugin-bindings/vst3editor.h>

namespace pongasoft::VST::GUI::Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// TextEditView::registerParameters
//------------------------------------------------------------------------
void TextEditView::registerParameters()
{
  fText = registerJmbCallback<UTF8String>(static_cast<ParamID>(getTag()),
                                          [this](GUIJmbParam<UTF8String> &iParam) {
                                            setText(*iParam);
                                          },
                                          true);
}

//------------------------------------------------------------------------
// TextEditView::valueChanged
//------------------------------------------------------------------------
void TextEditView::valueChanged()
{
  CTextEdit::valueChanged();
  if(fText.exists())
  {
    fText.update(getText());
  }
}

//------------------------------------------------------------------------
// TextEditView::setListener
//------------------------------------------------------------------------
void TextEditView::setListener(IControlListener *l)
{
  // For some reason in VST3.7.0, this methoed is called with VSTGUI::VST3Editor and it breaks the control entirely due to
  // what appear to be some hack ("fix textual representation") in the endEdit method
  // (see https://github.com/steinbergmedia/vstgui/blob/a9b1f800f1442846bf38280aaf62bc161e2137ca/vstgui/plugin-bindings/vst3editor.cpp#L200)
  if(dynamic_cast<VSTGUI::VST3Editor *>(l) == nullptr)
    CControl::setListener(l);
}

}