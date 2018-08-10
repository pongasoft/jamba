/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

//------------------------------------------------------------------------
// TextEditView::registerParameters
//------------------------------------------------------------------------
void TextEditView::registerParameters()
{
  if(!fParamCxMgr || getTag() < 0)
    return;

  if(fText.exists())
    unregisterParam(fText.getParamID());

  auto paramID = static_cast<ParamID>(getTag());

  fText = registerJmbParam<UTF8String>(paramID);
  if(fText.exists())
  {
    setText(fText);
  }
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
// TextEditView::setTag
//------------------------------------------------------------------------
void TextEditView::setTag(int32_t val)
{
  CTextEdit::setTag(val);
  registerParameters();
}

//------------------------------------------------------------------------
// TextEditView::onParameterChange
//------------------------------------------------------------------------
void TextEditView::onParameterChange(ParamID iParamID)
{
  if(fText.exists() && fText.getParamID() == iParamID)
  {
    setText(fText);
  }

  CustomViewAdapter::onParameterChange(iParamID);
}

}
}
}
}