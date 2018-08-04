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
  std::shared_ptr<GUIRawVstParameter> registerRawVstParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for BooleanParameter
  GUIVstParam<bool> registerVstBooleanParam(ParamID iParamID, bool iSubscribeToChanges = true);

  // shortcut for PercentParameter
  GUIVstParam<Percent> registerVstPercentParam(ParamID iParamID, bool iSubscribeToChanges = true);

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID, bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
    else
      return nullptr;
  }

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(VstParam<T> const &iParamDef, bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerVstParam(iParamDef, iSubscribeToChanges ? this : nullptr);
    else
      return nullptr;
  }

  /**
   * This method registers this class to be notified of the GUISerParam changes. Note that GUISerParam is already
   * a wrapper directly accessible from the view so there is no need to return something from this method.
   */
  template<typename T>
  void registerSerParam(GUISerParam<T> const &iParamDef)
  {
    if(fParamCxMgr)
      fParamCxMgr->registerSerParam(iParamDef, this);
  }

  /**
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUISerParam<T> registerSerParam(ParamID iParamID,
                                  bool iSubscribeToChanges = true)
  {
    if(fParamCxMgr)
      return fParamCxMgr->registerSerParam<T>(iParamID, iSubscribeToChanges ? this : nullptr);
    else
      return GUISerParam<T>{};
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
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return true if the param was present, false otherwise
   */
  bool unregisterParam(ParamID iParamID) { return fParamCxMgr->unregisterParam(iParamID); }

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