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

#include "CustomView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

/**
 * Base class for custom views providing one parameter only (similar to CControl)
 */
class CustomControlView : public CustomView
{
public:
  explicit CustomControlView(const CRect &iSize) : CustomView(iSize) {}

  // get/setControlTag
  virtual void setControlTag (TagID iTag) { fControlTag = iTag; };
  TagID getControlTag () const { return fControlTag; }

public:
  CLASS_METHODS_NOCOPY(CustomControlView, CustomView)

protected:
  TagID fControlTag{UNDEFINED_PARAM_ID};

public:
  class Creator : public CustomViewCreator<CustomControlView, CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerTagAttribute("control-tag", &CustomControlView::getControlTag, &CustomControlView::setControlTag);
    }
  };
};

/**
 * Base class for custom views providing one parameter only (similar to CControl).
 * This base class automatically registers the custom control and also keeps a control value for the case when
 * the control does not exist (for example in editor the control tag may not be defined).
 */
template<typename T, typename TGUIParam = GUIOptionalParam<T>>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(TCustomControlView, CustomControlView)

  // when the control tag changes we need to handle it
  void setControlTag(TagID iTag) override;

  // set/getControlValue
  T getControlValue() const;
  virtual void setControlValue(T const &iControlValue);

  // registerParameters
  void registerParameters() override;

protected:
  // the gui parameter tied to the control (handle vst/jmb or simple value)
  TGUIParam fControlParameter{};

public:
  using Creator = CustomViewCreator<TCustomControlView<T, TGUIParam>, CustomControlView>;
};

/**
 * Specialization for raw parameter (`ParamValue` / no conversion).
 */
using RawCustomControlView = TCustomControlView<ParamValue, GUIRawOptionalParam>;

//------------------------------------------------------------------------
// TCustomControlView<T>::getControlValue
//------------------------------------------------------------------------
template<typename T, typename TGUIParam>
T TCustomControlView<T, TGUIParam>::getControlValue() const
{
  return fControlParameter.getValue();
}

//------------------------------------------------------------------------
// TCustomControlView<T>::setControlValue
//------------------------------------------------------------------------
template<typename T, typename TGUIParam>
void TCustomControlView<T, TGUIParam>::setControlValue(T const &iControlValue)
{
  fControlParameter.update(iControlValue);
}

//------------------------------------------------------------------------
// TCustomControlView<ParamValue, GUIRawVstParameter>::registerParameters
// Specialization for ParamValue/GUIRawVstParameter
//------------------------------------------------------------------------
template<>
void TCustomControlView<ParamValue, GUIRawOptionalParam>::registerParameters();

//------------------------------------------------------------------------
// TCustomControlView<T>::registerParameters
//------------------------------------------------------------------------
template<typename T, typename TGUIParam>
void TCustomControlView<T, TGUIParam>::registerParameters()
{
  CustomControlView::registerParameters();

  registerOptionalParam(getControlTag(), fControlParameter);
}

//------------------------------------------------------------------------
// TCustomControlView<T>::setControlTag
//------------------------------------------------------------------------
template<typename T, typename TGUIParam>
void TCustomControlView<T, TGUIParam>::setControlTag(TagID iTag)
{
  CustomControlView::setControlTag(iTag);
  registerParameters();
}

}
}
}
}
