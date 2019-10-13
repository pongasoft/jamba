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

#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/base/ftypes.h>
#include <memory>
#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/FObjectCx.h>

namespace pongasoft::VST::GUI::Params {

using namespace Steinberg;
using namespace Steinberg::Vst;

// forward declaration required for compilation
template<typename T> class ITGUIParameter;

/**
 * A discrete parameter is defined by a parameter whose underlying backing type is an `int32` and whose number
 * of steps is `> 0`.
 */
using GUIDiscreteParameter = ITGUIParameter<int32>;

/**
 * This is the base class of all %GUI parameters. The API defined by this class is fairly limited since the
 * underlying type `T` is not known or exposed by this generic API.
 *
 * \note %GUI parameters are usually "shared" and subclasses will return shared pointers, that may potentially
 * be shared pointers on `this`.
 */
class IGUIParameter : public std::enable_shared_from_this<IGUIParameter>
{
public:
  /**
   * Defines the basic and common API of all parameter editors (allow to commit/rollback)
   */
  class Editor
  {
  public:
    /**
     * Commits all the changes applied to the parameter.
     *
     * \note If `commit()` or `rollback()` has already been called this method does nothing. */
    virtual tresult commit() = 0;

    /**
     * Rollback all the changes that were made to this parameter (since this editor was created).
     *
     * \note If `commit()` or `rollback()` has already been called this method does nothing. */
    virtual tresult rollback() = 0;

    /**
     * Technically the destructor must only call `rollback()`, but due to the fact that it is a virtual
     * method, it needs to be implemented by each subclass...
     */
    virtual ~Editor() = default;
  };

public:
  /**
   * Each parameter has a unique ID returned by this method.
   */
  virtual ParamID getParamID() const = 0;

  /**
   * When a parameter is a discrete parameter (which means its underlying backing type is an `int32` with values
   * in the discrete range `[0, getStepCount()]`), this method will return the number of steps (`> 0`).
   *
   * \note Although a parameter may not be a discrete parameter itself, the method `asDiscreteParameter(int32)` may
   * be able to convert/adapt it to one.
   *
   * @return the number of steps which is `> 0` if and only if this parameter is a discrete parameter.
   */
  virtual int32 getStepCount() const = 0;

  /**
   * Return the current value of the parameter as a string (which is properly UTF-8 encoded).
   *
   * @param iPrecision if `iPrecision < 0` the parameter is free to use whichever precision is tied to the parameter
   *                   otherwise it should use the one provided
   */
  virtual std::string toUTF8String(int32 iPrecision) const = 0;

  /**
   * Creates a connection between this parameter and the change listener: whenever the parameter changes, the
   * listener will be notified of the changes (`Parameters::IChangeListener::onParameterChange(ParamID)`).
   *
   * \note This method is usually invoked by the framework but it may be used in the rare cases when the caller
   * requires to handle the duration of the connection in a more granular fashion
   *
   * @param iChangeListener the listener that will be notified on parameter changes. `nullptr` is allowed and will be a noop.
   * @return the connection between this parameter and the listener and is maintained for as long as this pointer exists
   */
  virtual std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const = 0;

  /**
   * Creates a connection between this parameter and the callback: whenever the parameter changes, the
   * callback will be invoked (`std::function<void()>`).
   *
   * \note This method is usually invoked by the framework but it may be used in the rare cases when the caller
   * requires to handle the duration of the connection in a more granular fashion
   *
   * @param iChangeCallback the callback that will be invoked on parameter changes.
   * @return the connection between this parameter and the callback and is maintained for as long as this pointer exists
   */
  virtual std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const = 0;

public:
  /**
   * Downcasts this parameter into a typed version.
   *
   * @tparam T the underlying backing type of the parameter
   * @return the downcasted parameter of `nullptr` if the cast is not possible
   */
  template<typename T>
  std::shared_ptr<ITGUIParameter<T>> cast();

  /**
   * Converts this parameter into a discrete parameter. The discrete parameter returned is a parameter with an
   * underlying backing type `int32` and values in the discrete range `[0, getStepCount()]` or
   * `[0, iStepCount]` if `getStepCount() <= 0`.
   *
   * @param iStepCount if this parameter already defines a step count (`getStepCount() > 0`), then this parameter
   *                   is ignored, otherwise it is used to convert this parameter into a discrete parameter
   *                   with `iStepCount` steps.
   * @return the discrete parameter or `nullptr` if this parameter cannot be converted to a discrete parameter.
   */
  virtual std::shared_ptr<GUIDiscreteParameter> asDiscreteParameter(int32 iStepCount) = 0;
};

template<typename T>
class ITGUIParameter : public IGUIParameter
{
public:
  using ParamType = T;
  using ValueAccessor = std::function<void(T const &)>;

public:

public:
  class ITEditor : public IGUIParameter::Editor
  {
  public:
    virtual tresult setValue(ParamType const &iValue) = 0;

    virtual bool updateValue(ParamType const &iValue) = 0;

    /**
     * Importing other commit method from superclass */
    using IGUIParameter::Editor::commit;

    virtual tresult commit(ParamType const &iValue)
    {
      auto res = setValue(iValue);
      if(res == kResultOk)
        return commit();
      else
        return res;
    };
  };

public:
  /**
   * `getValue()` has a different api depending on the type of param (Vst and Jmb). As a result this interface
   * only provides a common way to access it in both case via an accessor which allows to:
   *
   * - avoid copy in the case of Jmb
   * - avoid duplicating values in the case of Vst
   *
   * @return `kResultOk` if the getter is called, and `kResultFalse` if the getter is not called for some reason
   */
  virtual tresult accessValue(ValueAccessor const &iGetter) const = 0;

  virtual bool update(ParamType const &iValue) = 0;

  virtual tresult setValue(ParamType const &iValue) = 0;

  virtual std::unique_ptr<ITEditor> edit() = 0;

  virtual std::unique_ptr<ITEditor> edit(ParamType const &iValue)
  {
    auto editor = edit();
    editor->setValue(iValue);
    return editor;
  }
};

template<typename T>
class DefaultEditorImpl : public ITGUIParameter<T>::ITEditor
{
public:
  explicit DefaultEditorImpl(ITGUIParameter<T> *iGUIParameter, T const &iDefaultValue) :
    fGUIParameter{iGUIParameter},
    fInitialValue{iDefaultValue}
  {
  }

  ~DefaultEditorImpl() override { rollback(); }

  bool updateValue(T const &iValue) override
  {
    if(fDoneEditing)
      return false;
    return fGUIParameter->update(iValue);
  }

  tresult setValue(T const &iValue) override
  {
    if(fDoneEditing)
      return kResultFalse;
    fGUIParameter->update(iValue);
    return kResultOk;
  }

  /**
   * Importing other commit method from superclass */
  using ITGUIParameter<T>::ITEditor::commit;

  tresult commit() override
  {
    if(fDoneEditing)
      return kResultFalse;
    fDoneEditing = true;
    return kResultOk;
  }

  tresult rollback() override
  {
    auto res = setValue(fInitialValue);
    fDoneEditing = true;
    return res;
  }

private:
  ITGUIParameter<T> *fGUIParameter;
  T fInitialValue;
  bool fDoneEditing{false};
};

//------------------------------------------------------------------------
// IGUIParam - wrapper to make writing the code much simpler and natural
//------------------------------------------------------------------------
/**
 * This is the main class that the plugin should use as it exposes only the necessary methods of the param
 * as well as redefine a couple of operators which helps in writing simpler and natural code.
 */
class IGUIParam
{
public:
  IGUIParam(std::shared_ptr<IGUIParameter> iPtr = nullptr) : // NOLINT (not marked explicit on purpose)
    fPtr{std::move(iPtr)}
  {}

  // exists
  inline bool exists() const { return (bool) fPtr; }

  // getParamID
  inline ParamID getParamID() const { return fPtr->getParamID(); }

  // getStepCount
  inline int32 getStepCount() const { return fPtr->getStepCount(); }

  /**
   * Return the current value of the parameter as a string (which is properly UTF-8 encoded).
   *
   * @param iPrecision if `iPrecision` < 0 the parameter is free to use whichever precision is tied to the parameter
   *                   otherwise it should use the one provided
   */
  inline std::string toUTF8String(int32 iPrecision) const { return fPtr->toUTF8String(iPrecision); }

  /**
   * @return an object maintaining the connection between the parameter and the listener
   */
  inline std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const { return fPtr->connect(iChangeListener); }

  /**
   * @return an object maintaining the connection between the parameter and the callback
   */
  std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const { return fPtr->connect(std::move(iChangeCallback)); }

private:
  std::shared_ptr<IGUIParameter> fPtr;
};


}