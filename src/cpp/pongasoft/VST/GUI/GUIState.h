#pragma once

#include <pongasoft/VST/Parameters.h>
#include <pongasoft/VST/GUI/Params/HostParameters.h>
#include <pongasoft/VST/GUI/Params/GUIParameter.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

class GUIState
{
public:
  explicit GUIState(Parameters const &iParameters) :
    fPluginParameters{iParameters}
  {};

  /**
   * Called by the GUIController. */
  tresult init(HostParameters const &iHostParameters);

  // getPluginParameters
  Parameters const &getPluginParameters() const { return fPluginParameters; }

  /**
   * This method is called for each parameter managed by the GUIState that is not a regular VST parameter
   */
  template<typename ParamSerializer>
  GUIParam<ParamSerializer> add(AnyParamDefSPtr<ParamSerializer> iParamDef);

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

protected:
  // setParamNormalized
  tresult setParamNormalized(NormalizedState const *iNormalizedState);

protected:
  std::unique_ptr<HostParameters> fHostParameters{};
  Parameters const &fPluginParameters;

  // contains all the (serializable) registered parameters (unique ID, will be checked on add)
  std::map<ParamID, std::shared_ptr<GUISerializableParameter>> fSerializableParameters{};

  // add serializable parameter to the structures
  void addSerializableParameter(std::shared_ptr<GUISerializableParameter> const &iParameter);
};

//------------------------------------------------------------------------
// GUIState::add
//------------------------------------------------------------------------
template<typename ParamSerializer>
GUIParam<ParamSerializer> GUIState::add(AnyParamDefSPtr<ParamSerializer> iParamDef)
{
  auto guiParam = std::make_shared<AnyParamDef<ParamSerializer>>(iParamDef);
  addSerializableParameter(guiParam);
  return guiParam;
}

}
}
}

