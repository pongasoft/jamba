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
#include <pongasoft/VST/FObjectCx.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

using namespace Steinberg::Vst;
using namespace Steinberg;

/**
 * Wrapper class which maintains the connection between a parameter and its listener. The connection will be
 * terminated if close() is called or automatically when the destructor is called.
 */
class GUIParamCx : public FObjectCx
{
public:
  // Constructor with listener
  GUIParamCx(ParamID iParamID, FObject *iParameter, Parameters::IChangeListener *iChangeListener);

  /**
   * Call to stop listening for changes. Also called automatically from the destructor.
   */
  void close() override;

  // onTargetChange
  void onTargetChange() override;

  // disabling copy
  GUIParamCx(GUIParamCx const &) = delete;
  GUIParamCx& operator=(GUIParamCx const &) = delete;

protected:
  ParamID fParamID;
  Parameters::IChangeListener *fChangeListener;
};

}
}
}
}


