#include "DrawContext.h"
#include <vstgui4/vstgui/lib/controls/ccontrol.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

void RelativeDrawContext::drawString(UTF8String const &fText, RelativeRect const &fSize, StringDrawContext &sdc)
{
  if(!(sdc.fStyle & kNoTextStyle))
  {
    fDrawContext->saveGlobalState();
    CRect textRect = toAbsoluteRect(fSize);
    textRect.inset(sdc.fTextInset.x, sdc.fTextInset.y);

    CRect oldClip;
    fDrawContext->getClipRect(oldClip);
    CRect newClip(textRect);
    newClip.bound(oldClip);
    fDrawContext->setClipRect(newClip);

    if(sdc.fAntialias)
      fDrawContext->setDrawMode(kAntiAliasing);
    fDrawContext->setFont(sdc.fFont ? sdc.fFont.get() : nullptr);

    // draw darker text (as shadow)
    if(sdc.fStyle & kShadowText)
    {
      CRect newSize(textRect);
      newSize.offset(sdc.fShadowTextOffset);
      fDrawContext->setFontColor(sdc.fShadowColor);
      fDrawContext->drawString(fText.getPlatformString(), newSize, sdc.fHoriTxtAlign, sdc.fAntialias);
    }
    fDrawContext->setFontColor(sdc.fFontColor);
    fDrawContext->drawString(fText.getPlatformString(), textRect, sdc.fHoriTxtAlign, sdc.fAntialias);
    fDrawContext->restoreGlobalState();
  }

}
}
}
}