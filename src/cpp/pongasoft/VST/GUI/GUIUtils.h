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
#pragma once

#include <vstgui4/vstgui/lib/ccolor.h>
#include <vstgui4/vstgui/lib/crect.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;

//------------------------------------------------------------------------
// CColor related utils
//------------------------------------------------------------------------
namespace CColorUtils {

// convenient call to check if a color is transparent
inline bool isTransparent(CColor const &iColor) {
  return iColor.alpha == 0;
}

}

//------------------------------------------------------------------------
// CRect related utils
//------------------------------------------------------------------------
namespace CRectUtils {

// offsetTop
inline CRect offsetTop(CRect const &iRect, CCoord iTop) {
  CRect res = iRect;
  res.top += iTop;
  return res;
}

// offsetRight
inline CRect offsetRight(CRect const &iRect, CCoord iRight) {
  CRect res = iRect;
  res.right += iRight;
  return res;
}

// offsetLeft
inline CRect offsetLeft(CRect const &iRect, CCoord iLeft) {
  CRect res = iRect;
  res.left += iLeft;
  return res;
}

// offsetBottom
inline CRect offsetBottom(CRect const &iRect, CCoord iBottom) {
  CRect res = iRect;
  res.bottom += iBottom;
  return res;
}

}



}
}
}