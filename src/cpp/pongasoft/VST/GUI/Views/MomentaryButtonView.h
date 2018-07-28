#pragma once

#include <vstgui4/vstgui/lib/controls/ccontrol.h>
#include <pongasoft/VST/GUI/Params/GUIParameter.h>
#include "CustomControlView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

// TODO TODO TODO
/* at this time I am still getting unexplained behavior (if the mouse click/release is too fast)
2018-06-11 10:47:59.350 ( 177.197s) [           1F4B2]    MomentaryButton.cpp:60       0| MomentaryButton::onMouseDown
2018-06-11 10:47:59.356 ( 177.204s) [           1F561]      VAC6Processor.cpp:263      0| VAC6Processor::processParameters => kMaxLevelReset=1.000000
2018-06-11 10:47:59.397 ( 177.244s) [           1F4B2]    MomentaryButton.cpp:78       0| MomentaryButton::onMouseUp
2018-06-11 10:47:59.399 ( 177.246s) [           1F561]      VAC6Processor.cpp:263      0| VAC6Processor::processParameters => kMaxLevelReset=0.000000
2018-06-11 10:47:59.432 ( 177.280s) [           1F4B2]      VAC6Processor.cpp:307      0| VAC6Processor::getState => fSoftClippingLevel=0.170968
2018-06-11 10:47:59.432 ( 177.280s) [           1F4B2]     VAC6Controller.cpp:168      0| VAC6Controller::getState()
2018-06-11 10:47:59.433 ( 177.281s) [           1F4B2]      VAC6Processor.cpp:307      0| VAC6Processor::getState => fSoftClippingLevel=0.170968
2018-06-11 10:47:59.433 ( 177.281s) [           1F4B2]     VAC6Controller.cpp:168      0| VAC6Controller::getState()
2018-06-11 10:47:59.441 ( 177.289s) [           1F561]      VAC6Processor.cpp:263      0| VAC6Processor::processParameters => kMaxLevelReset=1.000000

 Update 2018/07/12 Cannot reproduce with Editor or Maschine 2... only happens in Reason so may just be an implementation
                   issue with VST plugins in Reason
 */

/**
 * Represents a momentary button: a button which is "on" only when pressed. The VSTGUI sdk has a class called
 * CKickButton which is similar but it behaves improperly (for example, the button gets stuck in "pressed" state
 * for some reason...)
 */
class MomentaryButtonView : public TCustomControlView<BooleanParamConverter>
{
public:
  explicit MomentaryButtonView(const CRect &iSize) : TCustomControlView(iSize)
  {
    // off color is grey
    fBackColor = CColor{200,200,200};
  }

  // draw => does the actual drawing job
  void draw(CDrawContext *iContext) override;

  // input events (mouse/keyboard)
  CMouseEventResult onMouseDown(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseUp(CPoint &where, const CButtonState &buttons) override;
  CMouseEventResult onMouseCancel() override;
  int32_t onKeyDown(VstKeyCode &keyCode) override;
  int32_t onKeyUp(VstKeyCode &keyCode) override;

  // sizeToFit
  bool sizeToFit() override;

  // is on or off
  bool isOn() const { return getControlValue(); }
  bool isOff() const { return !isOn(); }

  // get/setOnColor (the off color is the back color...)
  CColor const &getOnColor() const { return fOnColor; }
  void setOnColor(CColor const &iColor) { fOnColor = iColor; }

  /**
   * get/setImage for the button which should have 2 frames
   * The images should contain the following 2 frames (each is of size image height / 2):
   *   - at y = 0, the button in its off state
   *   - at y = image height / 2, the button in its on state
   */
  BitmapPtr getImage() const { return fImage; }
  void setImage(BitmapPtr iImage) { fImage = std::move(iImage); }

public:
  CLASS_METHODS_NOCOPY(MomentaryButtonView, TCustomControlView<BooleanParamConverter>)

protected:
  CColor fOnColor{kRedCColor};
  BitmapPtr fImage{nullptr};

public:
  class Creator : public CustomViewCreator<MomentaryButtonView, TCustomControlView<BooleanParamConverter>>
  {
    public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerColorAttribute("on-color", &MomentaryButtonView::getOnColor, &MomentaryButtonView::setOnColor);
      registerBitmapAttribute("button-image", &MomentaryButtonView::getImage, &MomentaryButtonView::setImage);
    }
  };
};

}
}
}
}