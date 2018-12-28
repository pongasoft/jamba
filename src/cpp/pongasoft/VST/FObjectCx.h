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

#include <base/source/fobject.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <pongasoft/VST/Parameters.h>

namespace pongasoft {
namespace VST {

using namespace Steinberg::Vst;
using namespace Steinberg;

/**
 * Wrapper class which maintains a connection between the target and this object. The connection will be
 * terminated if close() is called or automatically when the destructor is called. The main point of this class
 * is to turn FObject.addRef/addDependent into an RAII concept (Resource Acquisition Is Initialization).
 */
class FObjectCx : protected FObject
{
public:
  // Constructor
  explicit FObjectCx(FObject *iTarget);

  /**
   * Call to stop listening for changes. Also called automatically from the destructor.
   */
  virtual void close();

  /**
   * Called when the target changes (by default does nothing)
   */
  virtual void onTargetChange() {};

  /**
   * Automatically closes the connection and stops listening */
  inline ~FObjectCx() override { close(); }

  // disabling copy
  FObjectCx(FObjectCx const &) = delete;
  FObjectCx& operator=(FObjectCx const &) = delete;

protected:
  /**
   * This is being called when fTarget sends a message to this object
   */
  void PLUGIN_API update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage) SMTG_OVERRIDE;

protected:
  FObject *fTarget;
  bool fIsConnected;
};

/**
 * Wrapper class which will invoke the callback when the target is changed
 */
class FObjectCxCallback : public FObjectCx
{
public:
  using ChangeCallback = std::function<void()>;

  // Constructor
  FObjectCxCallback(FObject *iTarget, ChangeCallback iChangeCallback);

  // close
  void close() override;

  // onTargetChange
  void onTargetChange() override;

  // disabling copy
  FObjectCxCallback(FObjectCxCallback const &) = delete;
  FObjectCxCallback& operator=(FObjectCxCallback const &) = delete;

protected:
  ChangeCallback fChangeCallback;
};

}
}