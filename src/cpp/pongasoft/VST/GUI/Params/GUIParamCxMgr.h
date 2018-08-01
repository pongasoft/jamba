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
  GUIVstParam<ParamConverter> registerVstParam(VstParam<ParamConverter> const &iParamDef,
                                               Parameters::IChangeListener *iChangeListener = nullptr)
  {
    return std::make_unique<GUIVstParameter<ParamConverter>>(registerRawVstParam(iParamDef->fParamID,
                                                                                 iChangeListener));
  }

  /**
   * This method registers the listener to be notified of the GUISerParam changes. Note that GUISerParam is already
   * a wrapper directly accessible from the view so there is no need to return something from this method. As a result
   * there is no need to call this method unless a listener is provided, hence the listener is required.
   */
  template<typename ParamSerializer>
  void registerSerParam(GUISerParam<ParamSerializer> const &iParamDef, Parameters::IChangeListener *iChangeListener)
  {
    DCHECK_F(iChangeListener != nullptr);
    fParamCxs[iParamDef.getParamID()] = std::move(iParamDef.connect(iChangeListener));
  }

  /**
   * Registers the ser param only given its id and return the associated GUISerParameterSPtr
   */
  template<typename ParamSerializer>
  GUISerParameterSPtr<ParamSerializer> registerSerParam(ParamID iParamID,
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
// GUIParamCxMgr::registerSerParam
//------------------------------------------------------------------------
template<typename ParamSerializer>
GUISerParameterSPtr<ParamSerializer> GUIParamCxMgr::registerSerParam(ParamID iParamID,
                                                                     Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getSerParameter(iParamID);

  DCHECK_F(param != nullptr, "param [%d] not found", iParamID);

  auto res = std::dynamic_pointer_cast<GUISerParameter<ParamSerializer>>(param);
  DCHECK_F(res != nullptr, "param [%d] is not of expected type", iParamID);
  if(iChangeListener)
    fParamCxs[iParamID] = std::move(res->connect(iChangeListener));

  return res;
}

}
}
}
}

#endif //__PONGASOFT_VST_GUI_PARAM_CX_MGR_H__
