#pragma once

#include "GUIParamCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

/**
 * Encapsulates classes that want to be aware of parameters and be notified when they change
 */
class GUIParamCxAware : public Parameters::IChangeListener
{
public:
  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawVstParameter> registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Generic register with any kind of conversion
   */
  template<typename ParamConverter>
  GUIVstParam<ParamConverter> registerVstParam(ParamID iParamID, bool iSubscribeToChanges = true)
  {
    return std::make_unique<GUIVstParameter<ParamConverter>>(registerRawVstParam(iParamID, iSubscribeToChanges));
  }

  // shortcut for BooleanParameter
  GUIVstBooleanParam registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIVstPercentParam registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Generic register with any kind of conversion using an actual param def (no param id)
   */
  template<typename ParamConverter>
  GUIVstParam<ParamConverter> registerVstParam(VstParam<ParamConverter> iParamDef, bool iSubscribeToChanges = true)
  {
    return std::make_unique<GUIVstParameter<ParamConverter>>(registerRawVstParam(iParamDef->fParamID,
                                                                                 iSubscribeToChanges));
  }

  /**
   * This method registers this class to be notified of the GUISerParam changes. Note that GUISerParam is already
   * a wrapper directly accessible from the view so there is no need to return something from this method.
   */
  template<typename ParamSerializer>
  void registerSerParam(GUISerParam<ParamSerializer> const &iParamDef)
  {
    fParamCxMgr->registerSerParam(iParamDef, this);
  }

  /**
   * Registers the ser param only given its id and return the associated GUISerParameterSPtr
   */
  template<typename ParamSerializer>
  GUISerParameterSPtr<ParamSerializer> registerSerParam(ParamID iParamID,
                                                        bool iSubscribeToChanges = true)
  {
    return fParamCxMgr->registerSerParam<ParamSerializer>(iParamID, iSubscribeToChanges ? this : nullptr);
  }

  /**
   * Called during initialization
   */
  virtual void initState(GUIState *iGUIState)
  {
    fParamCxMgr = iGUIState->createParamCxMgr();
  }

  /**
   * Subclasses should override this method to register each parameter
   */
  virtual void registerParameters()
  {
    // subclasses implements this method
  }

  /**
   * Callback when a parameter changes. Empty default implementation
   */
  void onParameterChange(ParamID iParamID) override {}

protected:
  // Access to parameters
  std::unique_ptr<GUIParamCxMgr> fParamCxMgr;
};


}
}
}
}