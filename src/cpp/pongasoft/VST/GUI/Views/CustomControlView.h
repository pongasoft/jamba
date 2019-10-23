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

namespace pongasoft::VST::GUI::Views {

/**
 * Base class for custom views which are tied to one parameter only (similar to `CControl`).
 *
 * @note Unlike %VSTGUI, it is actually very easy to have a view managing more than 1 parameter, nonetheless this
 *       class and subclasses have been created for convenience (and as an example) since 1 parameter per view
 *       is a pretty typical use case.
 *
 * @note This base class does not handle registration of the parameter itself because it doesn't know anything
 *       about the parameter (in particular its type).
 *
 * In addition to the attributes exposed by `CustomView`, this class exposes the following attributes:
 *
 * Attribute     | Description
 * ---------     | -----------
 * `control-tag` | @copydoc getControlTag()
 *
 */
class CustomControlView : public CustomView
{
public:
  explicit CustomControlView(const CRect &iSize) : CustomView(iSize) {}

  //! @see getControlTag()
  virtual void setControlTag (ParamID iTag) { fControlTag = iTag; };

  /**
   * Id of the parameter that this view manages.
   *
   * @note For consistency with the %VSTGUI `CControl` class, this code uses the name *tag* (but it is properly
   *       mapped to a `ParamID` since this is what is represented).
   *
   * @note In order to differentiate between the custom view tag and the control tag, the naming is made more explicit
   *       (in `CControl` it is simple called `getTag`).
   */
  ParamID getControlTag () const { return fControlTag; }

public:
  CLASS_METHODS_NOCOPY(CustomControlView, CustomView)

protected:
  ParamID fControlTag{UNDEFINED_PARAM_ID};

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
 * Base class which extends `CustomControlView` to provide the type (`T`) of the underlying parameter this
 * view is managing.
 *
 * Because the type is known, this class takes care of registering the parameter (using its id given by
 * `getControlTag()`) and automatically handles vst, jmb or no parameter at all (case when the id is undefined).
 *
 * @see `Params::GUIOptionalParam` for more details on optional parameters
 */
template<typename T>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(TCustomControlView, CustomControlView)

  /**
   * Returns the value of the managed parameter (properly typed) */
  T getControlValue() const;

  /**
   * Sets the value of the managed parameter to the provided value */
  virtual void setControlValue(T const &iControlValue);

  /**
   * Registers the optional parameter using `getControlTag()` as its id */
  void registerParameters() override;

protected:
  // the gui parameter tied to the control (handle vst/jmb or simple value)
  GUIOptionalParam<T> fControlParameter{};

public:
  using Creator = CustomViewCreator<TCustomControlView<T>, CustomControlView>;
};

/**
 * Specialization for raw parameter (`ParamValue` / no conversion).
 *
 * @note Roughly speaking, this view is essentially the Jamba equivalent of `CControl` with all the bells and whistles
 *       added.
 */
using RawCustomControlView = TCustomControlView<ParamValue>;

/**
 * Specialization of `TCustomControlView` for discrete values.
 *
 * This type of view is designed to work with any parameter (both Vst and Jmb) that is (or can be interpreted as)
 * a discrete parameter which means:
 *
 * - for Vst parameters
 *   - a discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` > 0). Note that in this case
 *     the attribute `step-count` is ignored (and should be set to its default `-1`).
 *   - a non discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` = 0) can be interpreted
 *     as a discrete parameter by defining the number of steps `step-count`. For example, setting `step-count` to `10`
 *     will "split" the continuous range into 11 values `(0.0, 0.1, 0.2, 0.3, 0.4, ..., 1.0)`.
 * - for Jmb parameters
 *   - a discrete parameter (which is a parameter where `IGUIParameter::getStepCount()` > 0, which is the case if
 *     a discrete converter is defined, like for `DiscreteTypeParamSerializer` that handle enums). Note that in this
 *     case the attribute `step-count` is  ignored (and should be set to its default `-1`).
 *   - a parameter whose underlying type (`T`) is convertible (both ways) to an `int32` can be interpreted as a
 *     discrete parameter by defining the number of steps `step-count`.
 *
 * In addition to the attributes exposed by `CustomControlView`, this class exposes the following attributes:
 *
 * Attribute    | Description
 * ---------    | -----------
 * `step-count` | @copydoc getStepCount()
 *
 * @see IGUIParameter::asDiscreteParameter()
 */
class CustomDiscreteControlView : public TCustomControlView<int32>
{
public:
  // CustomDiscreteControlView
  explicit CustomDiscreteControlView(const CRect &iSize) : TCustomControlView<int32>(iSize) {}

  /**
   * The number of steps of the managed discrete parameter as specified by this view.
   *
   * @note This value is **ignored** if the managed parameter is already a discrete parameter, thus providing
   *       its own step count. It will only be used in the event the managed parameter is not a discrete parameter
   *       but can be *interpreted* as one (IGUIParameter::asDiscreteParameter())
   */
  int32 getStepCount() const { return fStepCount; }

  //! @see getStepCount()
  void setStepCount(int32 iStepCount) { fStepCount = iStepCount; }

public:
  CLASS_METHODS_NOCOPY(CustomDiscreteControlView, TCustomControlView<int32>)

  //! @copydoc pongasoft::VST::GUI::Views::TCustomControlView::registerParameters()
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
  fControlParameter = registerOptionalParam<T>(getControlTag());
}

}
