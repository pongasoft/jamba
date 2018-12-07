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

#include "TextButtonView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// TextButtonView::registerParameters
//------------------------------------------------------------------------
void TextButtonView::registerParameters()
{
  if(!fParamCxMgr || getTitleTag() < 0)
    return;

  if(fTitle.exists())
    unregisterParam(fTitle.getParamID());

  auto paramID = static_cast<ParamID>(getTitleTag());

  fTitle = registerJmbParam<UTF8String>(paramID, [this] {
    setTitle(fTitle);
  });

  if(fTitle.exists())
  {
    setTitle(fTitle);
  }
}

//------------------------------------------------------------------------
// TextButtonView::setTitleTag
//------------------------------------------------------------------------
void TextButtonView::setTitleTag(int32_t iValue)
{
  fTitleTag = iValue;
  registerParameters();
}

//------------------------------------------------------------------------
// TextButtonView::valueChanged
//------------------------------------------------------------------------
void TextButtonView::valueChanged()
{
  CControl::valueChanged();

  if(getValue() == getMax())
    onClick();
}

//------------------------------------------------------------------------
// TextButtonView::onClick
//------------------------------------------------------------------------
void TextButtonView::onClick()
{
  if(fOnClickListener)
    fOnClickListener();
}

}
}
}
}