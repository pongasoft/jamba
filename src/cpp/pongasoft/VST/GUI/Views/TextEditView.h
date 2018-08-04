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
 * Extends the CTextEdit view to tie it to a GUISerParam<UTF8String> => multiple views tied to the same paramID (aka Tag)
 * are all synchronized and the text edit value will be saved and restored with the state.
 */
class TextEditView : public CustomViewAdapter<CTextEdit>
{
public:
  // Constructor
  explicit TextEditView(const CRect &iSize) : CustomViewAdapter(iSize, nullptr, -1)
  {}

  // setTag => overridden to be able to change the param (in the editor)
  void setTag(int32_t val) override;

  // registerParameters
  void registerParameters() override;

  // valueChanged
  void valueChanged() override;

  // onParameterChange
  void onParameterChange(ParamID iParamID) override;

  CLASS_METHODS_NOCOPY(StringTextEditView, CustomViewAdapter<CTextEdit>)

protected:
  // the underlying ser parameter of type UTF8String
  GUISerParam<UTF8String> fText{};

public:
  class Creator : public TCustomViewCreator<TextEditView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      TCustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCTextEdit)
    {
    }
  };
};

}
}
}
}

