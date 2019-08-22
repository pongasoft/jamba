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

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

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
  template<typename T>
  std::shared_ptr<ITGUIParameter<T>> cast();
  virtual std::unique_ptr<FObjectCx> connect(Parameters::IChangeListener *iChangeListener) const = 0;
  virtual std::unique_ptr<FObjectCx> connect(Parameters::ChangeCallback iChangeCallback) const = 0;
};

template<typename T>
class ITGUIParameter : public IGUIParameter
{
public:
  using ParamType = T;

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
  virtual ParamType const &getValue() const = 0;

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
  explicit DefaultEditorImpl(ITGUIParameter<T> *iGUIParameter) :
    fGUIParameter{iGUIParameter},
    fInitialValue{iGUIParameter->getValue()}
  {
  }

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
}
}
}