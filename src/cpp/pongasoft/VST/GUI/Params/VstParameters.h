#pragma once

#include <pongasoft/logging/logging.h>

#include <base/source/fstreamer.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

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
