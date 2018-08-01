#ifndef __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
#define __PONGASOFT_VST_GUI_PARAM_CX_MGR_H__

#include <pongasoft/VST/GUI/GUIState.h>

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
  inline bool existsVst(ParamID iParamID) const { return fGUIState->existsVst(iParamID); }

  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawVstParameter> registerRawVstParam(ParamID iParamID,
                                                          Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Generic register with any kind of conversion
   */
  template<typename T>
  std::unique_ptr<T> registerVstParam(ParamID iParamID,
                                      Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return std::make_unique<T>(registerRawVstParam(iParamID, iChangeListener));
  }

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   */
  template<typename ParamConverter>
  GUIVstParam<ParamConverter> registerVstParam(VstParam<ParamConverter> iParamDef,
                                               Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return std::make_unique<GUIVstParameter<ParamConverter>>(registerRawVstParam(iParamDef->fParamID,
                                                                                 iChangeListener));
  }

  /**
   * Returns the plugin parameters downcasting it to the proper subclass for direct access to the parameters by name.
   * Note that it will return nullptr if the plugin parameters are not of the proper type.
   */
  template<typename TParameters>
  TParameters const *getPluginParameters() const
  {
    return dynamic_cast<TParameters const *>(&fGUIState->getPluginParameters());
  }

  friend class GUI::GUIState;

protected:
  explicit GUIParamCxMgr(GUIState *iGUIState) : fGUIState{iGUIState}
  {
    DCHECK_F(fGUIState != nullptr);
  }

private:
  // the gui state
  GUIState *fGUIState;

  // Maintains the connections for the listeners... will be automatically discarded in the destructor
  std::map<ParamID, std::unique_ptr<GUIRawVstParameter::Connection>> fVstParamCxs;
};


}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
