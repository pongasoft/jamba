#include "MomentaryButtonView.h"

#include <vstgui4/vstgui/lib/cdrawcontext.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

///////////////////////////////////////////
// MomentaryButtonView::draw
///////////////////////////////////////////
void MomentaryButtonView::draw(CDrawContext *iContext)
{
  CustomView::draw(iContext);

  if(fImage)
  {
    CCoord y = isOn() ? fImage->getHeight() / 2 : 0;
    fImage->draw(iContext, getViewSize(), CPoint{0, y});
  }
  else
  {
    // no image => simply fill the surface with appropriate color (background and "on" color)
    // so that the button is fully functioning right away
    if(isOn())
    {
      iContext->setFillColor(getOnColor());
      iContext->drawRect(getViewSize(), kDrawFilled);
    }
  }

  setDirty(false);
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseDown
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseDown(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(true);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseUp
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseUp(CPoint &where, const CButtonState &buttons)
{
  if(!(buttons & kLButton))
    return kMouseEventNotHandled;

  setControlValue(false);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onMouseCancel
///////////////////////////////////////////
CMouseEventResult MomentaryButtonView::onMouseCancel()
{
  setControlValue(false);
  setDirty(true);
  return kMouseEventHandled;
}

///////////////////////////////////////////
// MomentaryButtonView::onKeyDown
///////////////////////////////////////////
int32_t MomentaryButtonView::onKeyDown(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(true);
    setDirty(true);
    return 1;
  }
  return -1;
}

///////////////////////////////////////////
// MomentaryButtonView::onKeyUp
///////////////////////////////////////////
int32_t MomentaryButtonView::onKeyUp(VstKeyCode &keyCode)
{
  if(keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
  {
    setControlValue(false);
    setDirty(true);
    return 1;
  }
  return -1;
}

///////////////////////////////////////////
// MomentaryButtonView::sizeToFit
///////////////////////////////////////////
bool MomentaryButtonView::sizeToFit()
{
  DLOG_F(INFO, "MomentaryButtonView::sizeToFit");
  return CustomView::sizeToFit(getImage(), 2);
}

MomentaryButtonView::Creator __gMomentaryButtonCreator("pongasoft::MomentaryButton", "pongasoft - Momentary Button (on when pressed)");

}
}
}
}