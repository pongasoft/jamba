#ifndef __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
#define __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__

#include "GUIParameters.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

class GUIParamCxMgr
{
public:
  /**
   * @return true if the param actually exists
   */
  bool exists(ParamID iParamID) const
  {
    return fParameters->exists(iParamID);
  }

  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawParameter> registerGUIRawParam(ParamID iParamID,
                                                       GUIRawParameter::IChangeListener *iChangeListener = nullptr);

  /**
   * Generic register with any kind of conversion
   */
  template<typename T>
  std::unique_ptr<T> registerGUIParam(ParamID iParamID,
                                      GUIRawParameter::IChangeListener *iChangeListener = nullptr)
  {
    return std::make_unique<T>(registerGUIRawParam(iParamID, iChangeListener));
  }

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   */
  template<typename ParamConverter>
  GUIParamUPtr<ParamConverter> registerGUIParam(ParamDefSPtr<ParamConverter> iParamDef,
                                                GUIRawParameter::IChangeListener *iChangeListener = nullptr)
  {
    return std::make_unique<GUIParameter<ParamConverter>>(registerGUIRawParam(iParamDef->fParamID, iChangeListener));
  }

  /**
   * Returns the plugin parameters downcasting it to the proper subclass for direct access to the parameters by name.
   * Note that it will return nullptr if the plugin parameters are not of the proper type.
   */
  template<typename TParameters>
  TParameters const *getPluginParameters() const
  {
    return dynamic_cast<TParameters const *>(&fParameters->getPluginParameters());
  }

  friend class GUIParameters;

protected:
  explicit GUIParamCxMgr(std::shared_ptr<const GUIParameters> iParameters) :
    fParameters{std::move(iParameters)}
  {}

private:
  // the parameters
  std::shared_ptr<const GUIParameters> fParameters;

  // Maintains the connections for the listeners... will be automatically discarded in the destructor
  std::map<ParamID, std::unique_ptr<GUIRawParameter::Connection>> fParameterConnections;
};


}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
