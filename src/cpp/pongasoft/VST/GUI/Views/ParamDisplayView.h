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

#include <vstgui4/vstgui/lib/controls/cparamdisplay.h>
#include <pongasoft/VST/GUI/Params/IGUIParameter.h>

#include "CustomView.h"

namespace pongasoft::VST::GUI::Views {

/**
 * This view extends `CParamDisplay` to work for both Vst and Jmb parameters. In addition, it provides a
 * `precision-override` attribute which, by default (-1), lets the parameter determine what the precision is (when
 * displaying numbers), but if set `>= 0` it will use the attribute value instead.
 */
class ParamDisplayView : public CustomViewAdapter<CParamDisplay>
{
public:
  using super_type = CustomViewAdapter<CParamDisplay>;

public:
  // Constructor
  explicit ParamDisplayView(const CRect &iSize) : super_type(iSize)
  {
    tag = UNDEFINED_PARAM_ID;
  }

  // draw - overridden to handle any parameter
  void draw(CDrawContext *iContext) override;

  // get/set precision-override
  int32 getPrecisionOverride() const { return fPrecisionOverride; }
  void setPrecisionOverride(int32 iPrecisionOverride) { fPrecisionOverride = iPrecisionOverride; markDirty(); }

  // registerParameters
  void registerParameters() override;

protected:
  int32 fPrecisionOverride{-1};

  IGUIParam fParam{};

public:
  class Creator : public CustomViewCreator<ParamDisplayView, super_type>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName, VSTGUI::UIViewCreator::kCParamDisplay)
    {
      registerIntegerAttribute<int32>("precision-override", &ParamDisplayView::getPrecisionOverride, &ParamDisplayView::setPrecisionOverride);
    }
  };
};

}

