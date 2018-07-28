#pragma once

#include <pongasoft/VST/GUI/Params/GUIParameter.h>
#include "CustomControlView.h"

#include <iostream>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;
using namespace Params;

class ToggleButtonView : public TCustomControlView<BooleanParamConverter>
{
public:
  explicit ToggleButtonView(const CRect &iSize) : TCustomControlView(iSize)
  {
    // off color is grey
    fBackColor = CColor{200, 200, 200};
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
  bool isOn() const
  { return getControlValue(); }

  bool isOff() const
  { return !isOn(); }

  // get/set frames (should be either 2 or 4) 4 includes the pressed state
  int getFrames() const
  { return fFrames; }

  void setFrames(int iFrames);

  // get/setOnColor (the off color is the back color...)
  CColor const &getOnColor() const
  { return fOnColor; }

  void setOnColor(CColor const &iColor)
  { fOnColor = iColor; }

  // get/setInverse (toggles which image is on and which is off)
  bool getInverse() const
  { return fInverse; }

  void setInverse(bool iInverse)
  { fInverse = iInverse; }

  /**
   * get/setImage for the button which should have 2 or 4 frames depending on the fFrames value
   * The images should contain the following :
   * - for 2 frames each is of size image height / 2:
   *   - at y = 0, the button in its off state
   *   - at y = image height / 2, the button in its on state
   * - for 4 frames each is of size image height / 4:
   *   - at y = 0              0/4, the button in its off state
   *   - at y = image height * 1/4, the button in its off state depressed
   *   - at y = image height * 2/4, the button in its on state
   *   - at y = image height * 3/4, the button in its on state depressed
   */
  BitmapPtr getImage() const
  { return fImage; }

  void setImage(BitmapPtr iImage)
  { fImage = std::move(iImage); }

public:
  CLASS_METHODS_NOCOPY(ToggleButtonView, TCustomControlView<BooleanParamConverter>)

protected:
  int fFrames{4};
  CColor fOnColor{kRedCColor};
  BitmapPtr fImage{nullptr};
  bool fInverse{false};

  bool fPressed{false};

public:
  class Creator : public CustomViewCreator<ToggleButtonView, TCustomControlView<BooleanParamConverter>>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntAttribute("frames", &ToggleButtonView::getFrames, &ToggleButtonView::setFrames);
      registerColorAttribute("on-color", &ToggleButtonView::getOnColor, &ToggleButtonView::setOnColor);
      registerBitmapAttribute("button-image", &ToggleButtonView::getImage, &ToggleButtonView::setImage);
      registerBooleanAttribute("inverse", &ToggleButtonView::getInverse, &ToggleButtonView::setInverse);
    }
  };
};

}
}
}
}