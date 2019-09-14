/*
 * Copyright (c) 2018-2019 pongasoft
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
template<typename T>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(TCustomControlView, CustomControlView)

  // set/getControlValue
  T getControlValue() const;
  virtual void setControlValue(T const &iControlValue);

  // registerParameters
  void registerParameters() override;

protected:
  // the gui parameter tied to the control (handle vst/jmb or simple value)
  GUIOptionalParam<T> fControlParameter{};

public:
  using Creator = CustomViewCreator<TCustomControlView<T>, CustomControlView>;
};

/**
 * Specialization for raw parameter (`ParamValue` / no conversion).
 */
using RawCustomControlView = TCustomControlView<ParamValue>;

/**
 * Base class for custom views providing a single discrete parameter only (similar to CControl).
 * This base class automatically registers the custom control and also keeps a control value for the case when
 * the control does not exist (for example in editor the control tag may not be defined). This type of view is
 * designed to work with any parameter (both Vst and Jmb) that is (or can be interpreted as) a discrete parameter
 * which means:
 *
 * - for Vst parameters
 *   - a discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` > 0). Note that in this case
 *     the attribute `step-count` is ignored (and should be set to its default `-1`).
 *   - a non discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` = 0) can be interpreted
 *     as a discrete parameter by defining the number of steps `step-count`. For example, setting `step-count` to `10`
 *     will "split" the continuous range into 11 values (0.0, 0.1, 0.2, 0.3, 0.4, ..., 1.0).
 * - for Jmb parameters
 *   - a discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` > 0, which is the case if
 *     a discrete converter is defined, like for `DiscreteTypeParamSerializer` that handle enums). Note that in this
 *     case the attribute `step-count` is  ignored (and should be set to its default `-1`).
 *   - a parameter whose underlying type (`T`) is convertible (both ways) to an `int32` can be interpreted as a
 *     discrete parameter by defining the number of steps `step-count`.
 */
class CustomDiscreteControlView : public TCustomControlView<int32>
{
public:
  // CustomDiscreteControlView
  explicit CustomDiscreteControlView(const CRect &iSize) : TCustomControlView<int32>(iSize) {}

  int32 getStepCount() const { return fStepCount; }
  void setStepCount(int32 iStepCount) { fStepCount = iStepCount; }

public:
  CLASS_METHODS_NOCOPY(CustomDiscreteControlView, TCustomControlView<int32>)

  // registerParameters
  void registerParameters() override;

protected:
  int32 fStepCount{-1};

public:
  class Creator : public CustomViewCreator<CustomDiscreteControlView, TCustomControlView<int32>>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerIntegerAttribute<int32>("step-count", &CustomDiscreteControlView::getStepCount, &CustomDiscreteControlView::setStepCount);
    }
  };
};

//------------------------------------------------------------------------
// TCustomControlView<T>::getControlValue
//------------------------------------------------------------------------
template<typename T>
T TCustomControlView<T>::getControlValue() const
{
  return fControlParameter.getValue();
}

//------------------------------------------------------------------------
// TCustomControlView<T>::setControlValue
//------------------------------------------------------------------------
template<typename T>
void TCustomControlView<T>::setControlValue(T const &iControlValue)
{
  fControlParameter.update(iControlValue);
}

//------------------------------------------------------------------------
// TCustomControlView<T>::registerParameters
//------------------------------------------------------------------------
template<typename T>
void TCustomControlView<T>::registerParameters()
{
  CustomControlView::registerParameters();

  registerOptionalParam(getControlTag(), fControlParameter);
}

}
}
}
}
