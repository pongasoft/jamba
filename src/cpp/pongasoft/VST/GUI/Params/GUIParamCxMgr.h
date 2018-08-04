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
   * @return true if the param actually exists
   */
  inline bool existsSer(ParamID iParamID) const { return fGUIState->existsSer(iParamID); }

  /**
   * Removes the registration of the provided param (closing the connection/stopping to listen)
   *
   * @return true if the param was present, false otherwise
   */
  bool unregisterParam(ParamID iParamID);

  /**
   * Registers a raw parameter (no conversion)
   */
  std::unique_ptr<GUIRawVstParameter> registerRawVstParam(ParamID iParamID,
                                                          Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Register a vst parameter simply given its id
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * Convenient call to register a GUI param simply by using its description. Takes care of the type due to method API
   * @return nullptr if not found or not proper type
   */
  template<typename T>
  GUIVstParam<T> registerVstParam(VstParam<T> iParamDef,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  /**
   * This method registers the listener to be notified of the GUISerParam changes. Note that GUISerParam is already
   * a wrapper directly accessible from the view so there is no need to return something from this method. As a result
   * there is no need to call this method unless a listener is provided, hence the listener is required.
   */
  template<typename T>
  void registerSerParam(GUISerParam<T> const &iParamDef, Parameters::IChangeListener *iChangeListener)
  {
    DCHECK_F(iChangeListener != nullptr);
    fParamCxs[iParamDef.getParamID()] = std::move(iParamDef.connect(iChangeListener));
  }

  /**
   * Registers the ser param only given its id and return the wrapper to the param.
   *
   * @return the wrapper which may be empty if the param does not exists or is of wrong type (use .exists)
   */
  template<typename T>
  GUISerParam<T> registerSerParam(ParamID iParamID,
                                  Parameters::IChangeListener *iChangeListener = nullptr);

  // getGUIState
  GUIState *getGUIState() const { return fGUIState; };

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
  std::map<ParamID, std::unique_ptr<GUIParamCx>> fParamCxs;
};


//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxMgr::registerVstParam(VstParam<T> iParamDef,
                                               Parameters::IChangeListener *iChangeListener)
{
  auto param = registerRawVstParam(iParamDef->fParamID, iChangeListener);

  if(param)
    return std::make_unique<GUIVstParameter<T>>(std::move(param), iParamDef);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxMgr::registerVstParam(ParamID iParamID,
                                               Parameters::IChangeListener *iChangeListener)
{
  auto param = registerRawVstParam(iParamID, iChangeListener);

  if(!param)
  {
    DLOG_F(WARNING, "vst param [%d] not found", iParamID);
    return nullptr;
  }

  auto rawParamDef = fGUIState->getRawVstParamDef(iParamID);

  auto paramDef = std::dynamic_pointer_cast<VstParamDef<T>>(rawParamDef);

  if(paramDef)
  {
    if(iChangeListener)
      fParamCxs[iParamID] = std::move(param->connect(iChangeListener));
    else
      unregisterParam(iParamID);

    return std::make_unique<GUIVstParameter<T>>(std::move(param), paramDef);
  }

  DLOG_F(WARNING, "vst param [%d] is not of the requested type", iParamID);
  unregisterParam(iParamID);
  return nullptr;

}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerSerParam
//------------------------------------------------------------------------
template<typename T>
GUISerParam<T> GUIParamCxMgr::registerSerParam(ParamID iParamID,
                                               Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getSerParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "ser param [%d] not found", iParamID);
    return nullptr;
  }

  auto res = dynamic_cast<GUISerParameter<T> *>(param);
  if(res)
  {
    if(iChangeListener)
      fParamCxs[iParamID] = std::move(res->connect(iChangeListener));
    else
      unregisterParam(iParamID);
  }
  else
  {
    DLOG_F(WARNING, "ser param [%d] is not of the requested type", iParamID);
    unregisterParam(iParamID);
  }

  return res;
}


}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
