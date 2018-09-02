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
  virtual void setControlTag (int32_t iTag) { fControlTag = iTag; };
  int32_t getControlTag () const { return fControlTag; }

public:
  CLASS_METHODS_NOCOPY(CustomControlView, CustomView)

protected:
  int32_t fControlTag{-1};

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
 * Base class for custom views providing one parameter only (similar to CControl)
 * This base class automatically registers the custom control and also keeps a control value for the case when
 * the control does not exist (for example in editor the control tag may not be defined).
 */
template<typename T>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(TCustomControlView, CustomControlView)

  // when the control tag changes we need to handle it
  void setControlTag(int32_t iTag) override;

  // set/getControlValue
  T getControlValue() const;
  virtual void setControlValue(T const &iControlValue);

  // registerParameters
  void registerParameters() override;

protected:
  // the gui parameter tied to the control (optional)
  GUIVstParam<T> fControlParameter{};

  // the value when the control parameter is not assigned
  T fControlValue{};

public:
  class Creator : public CustomViewCreator<TCustomControlView<T>, CustomControlView>
  {
  private:
    using CustomViewCreatorT = CustomViewCreator<TCustomControlView<T>, CustomControlView>;
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreatorT(iViewName, iDisplayName)
    {
    }
  };
};

///////////////////////////////////////////
// TCustomControlView<T>::getControlValue
///////////////////////////////////////////
template<typename T>
T TCustomControlView<T>::getControlValue() const
{
  if(fControlParameter.exists())
    return fControlParameter.getValue();
  else
    return fControlValue;
}

///////////////////////////////////////////
// TCustomControlView<T>::setControlValue
///////////////////////////////////////////
template<typename T>
void TCustomControlView<T>::setControlValue(T const &iControlValue)
{
  if(fControlParameter.exists())
    fControlParameter.setValue(iControlValue);
  else
    fControlValue = iControlValue;
}

///////////////////////////////////////////
// TCustomControlView<T>::registerParameters
///////////////////////////////////////////
template<typename T>
void TCustomControlView<T>::registerParameters()
{
  CustomControlView::registerParameters();

  if(!fParamCxMgr || getControlTag() < 0)
    return; // not set yet

  if(fControlParameter.exists())
  {
    fControlValue = fControlParameter.getValue();
    fControlParameter = unregisterParam(fControlParameter);
  }

  auto paramID = static_cast<ParamID>(getControlTag());
  fControlParameter = registerVstParam<T>(paramID);
}

//------------------------------------------------------------------------
// TCustomControlView<T>::setControlTag
//------------------------------------------------------------------------
template<typename T>
void TCustomControlView<T>::setControlTag(int32_t iTag)
{
  CustomControlView::setControlTag(iTag);
  registerParameters();
}

}
}
}
}
