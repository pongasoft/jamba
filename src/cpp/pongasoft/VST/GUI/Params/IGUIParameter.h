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

template<typename T> class ITGUIParameter;

class IGUIParameter : public std::enable_shared_from_this<IGUIParameter>
{
public:
  class Editor
  {
  public:
    virtual tresult commit() = 0;
    virtual tresult rollback() = 0;
    virtual ~Editor() = default;
  };

public:
  virtual ParamID getParamID() const = 0;
  virtual int32 getStepCount() const = 0;

  /**
   * Return the current value of the parameter as a string (which is properly UTF-8 encoded).
   *
   * @param iPrecision if `iPrecision` < 0 the parameter is free to use whichever precision is tied to the parameter
   *                   otherwise it should use the one provided
   */
  virtual std::string toUTF8String(int32 iPrecision) const = 0;

  virtual std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const = 0;
  virtual std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const = 0;

public:
  template<typename T>
  std::shared_ptr<ITGUIParameter<T>> cast();

  virtual std::shared_ptr<ITGUIParameter<int32>> asDiscreteParameter(int32 iStepCount) = 0;
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
  using ITGUIParameter<T>::Editor::commit;

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

}