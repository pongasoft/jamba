#pragma once

#include "GUIParameters.h"
#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Encapsulates classes that want to be aware of parameters and be notified when they change
 */
class GUIParamCxAware : public GUIRawParameter::IChangeListener
{
public:
  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawParameter> registerGUIRawParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Generic register with any kind of conversion
   */
  template<typename ParamConverter>
  GUIParamUPtr<ParamConverter> registerGUIParam(ParamID iParamID, bool iSubscribeToChanges = true)
  {
    return std::make_unique<GUIParameter<ParamConverter>>(registerGUIRawParam(iParamID, iSubscribeToChanges));
  }

  // shortcut for BooleanParameter
  GUIBooleanParamUPtr registerBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIPercentParamUPtr registerPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Generic register with any kind of conversion using an actual param def (no param id)
   */
  template<typename ParamConverter>
  GUIParamUPtr<ParamConverter> registerGUIParam(ParamDefSPtr<ParamConverter> iParamDef,
                                                bool iSubscribeToChanges = true)
  {
    return std::make_unique<GUIParameter<ParamConverter>>(registerGUIRawParam(iParamDef->fParamID, iSubscribeToChanges));
  }

  /**
   * Gives access to plugin parameters
   */
  template<typename TParameters>
  TParameters const *getPluginParameters() const
  {
    return fParamCxMgr->getPluginParameters<TParameters>();
  }

  /**
   * Called during initialization
   */
  virtual void initParameters(std::shared_ptr<GUIParameters> iParameters)
  {
    fParamCxMgr = iParameters->createParamCxMgr();
  }

  /**
   * Subclasses should override this method to register each parameter
   */
  virtual void registerParameters()
  {
    // subclasses implements this method
  }

  /**
   * Callback when a parameter changes. By default simply marks the view as dirty.
   */
  void onParameterChange(ParamID iParamID, ParamValue iNormalizedValue) override {}

protected:
  // Access to parameters
  std::unique_ptr<GUIParamCxMgr> fParamCxMgr;
};


}
}
}
}