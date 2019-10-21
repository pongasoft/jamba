/*
 * Copyright (c) 2019 pongasoft
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

#include <pongasoft/VST/GUI/Views/ParamDisplayView.h>
#include <pongasoft/VST/Timer.h>

namespace pongasoft::VST::GUI::Views {

/**
 * This parameter simply extends `ParamDisplayView` to "highlight" the parameter for `highlight-duration-ms`
 * milliseconds (by default 1s) when its value changes. "Highlighting" is implemented by swapping the back color
 * and the font color.
 *
 * In addition to the attributes exposed by `ParamDisplayView`, this class exposes the following attribute:
 *
 * Attribute | Description | More
 * --------- | ----------- | ----
 * `highlight-duration-ms` | duration (in ms) to highlight the parameter for when it changes | `getHighlightDurationMs()`
 */
class DebugParamDisplayView : public ParamDisplayView, public ITimerCallback
{
public:
  // Constructor
  explicit DebugParamDisplayView(const CRect &iSize) : ParamDisplayView(iSize) {}

  // afterApplyAttributes - simply store the back color and font color (to be able to swap/revert)
  void afterApplyAttributes() override
  {
    fBackColor = getBackColor();
    fFontColor = getFontColor();
    ParamDisplayView::afterApplyAttributes();
  }

  /**
   * Executed when the timer expires: revert the colors to their original state
   */
  void onTimer(Timer *timer) override
  {
    setBackColor(fBackColor);
    setFontColor(fFontColor);
    fTimer = nullptr;
    markDirty();
  }

  /**
   * Swap back and font colors and starts a timer to revert them back
   */
  void onParameterChange(ParamID iParamID) override
  {
    setBackColor(fFontColor);
    setFontColor(fBackColor);

    // this is safe to call even if there is already a timer pending because the previous one
    // will be released automatically (due to the behavior of AutoReleaseTimer)
    fTimer = AutoReleaseTimer::create(this, fHighlightDurationMs);

    CustomViewAdapter::onParameterChange(iParamID);
  }

  // get/set `highlight-duration-ms`
  uint32 getHighlightDurationMs() const { return fHighlightDurationMs; }
  void setHighlightDurationMs(uint32 iValue) { fHighlightDurationMs = iValue; }

private:
  CColor fBackColor{};
  CColor fFontColor{};

  uint32 fHighlightDurationMs{1000};

  std::unique_ptr<AutoReleaseTimer> fTimer{};

public:
  class Creator : public CustomViewCreator<DebugParamDisplayView, ParamDisplayView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<uint32>("highlight-duration-ms", &DebugParamDisplayView::getHighlightDurationMs, &DebugParamDisplayView::setHighlightDurationMs);
    }
  };
};

}