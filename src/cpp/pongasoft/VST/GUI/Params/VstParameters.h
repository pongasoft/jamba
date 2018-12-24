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

#include <pongasoft/logging/logging.h>

#include <base/source/fstreamer.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>
#include "GUIParamCx.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * This class acts as a facade/proxy to the vst parameters managed by the host daw
 */
class VstParameters
{
public:
  explicit VstParameters(EditController *const iParametersOwner) : fParametersOwner{iParametersOwner}
  {
    DCHECK_NOTNULL_F(iParametersOwner);
  }

  // getParamNormalized
  inline ParamValue getParamNormalized(ParamID iParamID) const { return fParametersOwner->getParamNormalized(iParamID); }
  inline tresult setParamNormalized(ParamID iParamID, ParamValue iValue) const { return fParametersOwner->setParamNormalized(iParamID, iValue); }
  inline tresult beginEdit(ParamID iParamID) const { return fParametersOwner->beginEdit(iParamID); }
  inline tresult performEdit(ParamID iParamID, ParamValue iValue) const { return fParametersOwner->performEdit(iParamID, iValue); }
  inline tresult endEdit(ParamID iParamID) const { return fParametersOwner->endEdit(iParamID); }
  Vst::Parameter *getParameterObject(ParamID iParamID) const { return fParametersOwner->getParameterObject(iParamID); }


  /**
   * @return a connection that will listen to parameter changes (see FObjectCx)
   */
  std::unique_ptr<FObjectCx> connect(ParamID iParamID, Parameters::IChangeListener *iChangeListener)
  {
    if(exists(iParamID))
      return std::make_unique<GUIParamCx>(iParamID,
                                          getParameterObject(iParamID),
                                          iChangeListener);
    else
      return nullptr;
  }

  /**
   * @return a connection that will listen to parameter changes (see FObjectCx)
   */
  std::unique_ptr<FObjectCx> connect(ParamID iParamID, Parameters::ChangeCallback iChangeCallback) const
  {
    if(exists(iParamID))
      return std::make_unique<FObjectCxCallback>(getParameterObject(iParamID),
                                                 std::move(iChangeCallback));
    else
      return nullptr;
  }

  // exists
  inline bool exists(ParamID iParamID) const { return getParameterObject(iParamID) != nullptr; }

private:
  EditController *const fParametersOwner;
};

using VstParametersSPtr = std::shared_ptr<VstParameters>;

}
}
}
}
