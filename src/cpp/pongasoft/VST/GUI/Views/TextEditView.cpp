
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

  fText = registerSerParam<UTF8String>(paramID);
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
    fText.setValue(getText());
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