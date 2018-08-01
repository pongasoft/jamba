#pragma once

#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/GUI/Params/VstParameters.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>
#include <pongasoft/VST/GUI/Params/GUISerParameter.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

namespace Params {
class GUIParamCxMgr;
}

class GUIState
{
public:
  explicit GUIState(Parameters const &iPluginParameters) :
    fPluginParameters{iPluginParameters}
  {};

  /**
   * Called by the GUIController. */
  virtual tresult init(VstParametersSPtr iVstParameters);

  // getPluginParameters
  Parameters const &getPluginParameters() const { return fPluginParameters; }

  /**
   * This method is called for each parameter managed by the GUIState that is not a regular VST parameter
   */
  template<typename ParamSerializer>
  GUISerParam<ParamSerializer> add(SerParam<ParamSerializer> iParamDef);

  /**
   * @return true if there is a vst param with the provided ID
   */
  inline bool existsVst(ParamID iParamID) const { return fVstParameters->exists(iParamID); }

  /**
   * @return true if there is a ser param with the provided ID
   */
  inline bool existsSer(ParamID iParamID) const { return fSerParams.find(iParamID) != fSerParams.cend(); }

  /**
   * @return the raw parameter given its id
   */
  std::unique_ptr<GUIRawVstParameter> getRawVstParameter(ParamID iParamID) const
  {
    return std::make_unique<GUIRawVstParameter>(iParamID, fVstParameters);
  }

  /**
   * @return the ser parameter given its id (nullptr if not found)
   */
  std::shared_ptr<IGUISerParameter> getSerParameter(ParamID iParamID) const;

  /**
   * This method is called from the GUI controller setComponentState method and reads the state coming from RT
   * and initializes the vst host parameters accordingly
   */
  virtual tresult readRTState(IBStreamer &iStreamer);

  /**
   * This method is called from the GUI controller setState method and reads the state previously saved by the
   * GUI only (parameters that are ui only) and initializes the vst host parameters accordingly
   */
  virtual tresult readGUIState(IBStreamer &iStreamer);

  /**
   * This method is called from the GUI controller getState method and writes the state specific to the
   * GUI only (parameters that are ui only), reading the values from the vst host parameters
   */
  virtual tresult writeGUIState(IBStreamer &oStreamer) const;

  /**
   * The CustomView class automatically calls this method to get a handle of a ParamCxMgr used to register for interest
   * and obtain GUIParam instances. See CustomView::registerXXX methods.
   */
  std::unique_ptr<GUIParamCxMgr> createParamCxMgr();

protected:
  // the parameters
  Parameters const &fPluginParameters;

  // setParamNormalized
  tresult setParamNormalized(NormalizedState const *iNormalizedState);

protected:
  VstParametersSPtr fVstParameters{};

  // contains all the (serializable) registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<IGUISerParameter>> fSerParams{};

  // add serializable parameter to the structures
  void addSerParam(std::shared_ptr<IGUISerParameter> const &iParameter);
};

/**
 * Simple templated extension to expose the plugin parameters as its real type
 *
 * @tparam TPluginParameters the type of the plugin parameters (subclass of Parameters)
 */
template<typename TPluginParameters>
class GUIPluginState : public GUIState
{
public:
  using PluginParameters = TPluginParameters;

public:
  explicit GUIPluginState(PluginParameters const &iPluginParameters) :
    GUIState{iPluginParameters},
    fParams{iPluginParameters}
  { }

public:
  PluginParameters const &fParams;
};

//------------------------------------------------------------------------
// GUIState::add
//------------------------------------------------------------------------
template<typename ParamSerializer>
GUISerParam<ParamSerializer> GUIState::add(SerParam<ParamSerializer> iParamDef)
{
  auto guiParam = std::make_shared<GUISerParameter<ParamSerializer>>(iParamDef);
  addSerParam(guiParam);
  return guiParam;
}

}
}
}

