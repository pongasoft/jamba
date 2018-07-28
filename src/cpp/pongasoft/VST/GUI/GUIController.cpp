#include "GUIController.h"
#include <vstgui4/vstgui/plugin-bindings/vst3editor.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Views;
using namespace VSTGUI;

//------------------------------------------------------------------------
// GUIController::GUIController
//------------------------------------------------------------------------
GUIController::GUIController(const char *iXmlFileName) :
  EditController(),
  fXmlFileName{iXmlFileName}
{

}

//------------------------------------------------------------------------
// GUIController::~GUIController
//------------------------------------------------------------------------
GUIController::~GUIController()
{
  delete fViewFactory;
}

//------------------------------------------------------------------------
// GUIController::initialize
//------------------------------------------------------------------------
tresult GUIController::initialize(FUnknown *context)
{
#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIController::initialize()");
#endif

  tresult result = EditController::initialize(context);
  if(result != kResultOk)
  {
    return result;
  }

  // making sure that the knob mode is set to the default specified
  CFrame::kDefaultKnobMode = fDefaultKnobMode;
  setKnobMode(fDefaultKnobMode);

  fGUIParameters = std::make_shared<GUIParameters>(HostParameters(this), *getPluginParameters());
  fGUIParameters->registerVstParameters(parameters);

  fViewFactory = new CustomUIViewFactory(fGUIParameters);

  for(const auto &viewState : fViewStates)
  {
    viewState->initParameters(fGUIParameters);
    viewState->registerParameters();
  }

  return result;
}

//------------------------------------------------------------------------
// GUIController::terminate
//------------------------------------------------------------------------
tresult GUIController::terminate()
{
#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIController::terminate()");
#endif

  tresult res = EditController::terminate();

  delete fViewFactory;
  fViewFactory = nullptr;

  return res;
}

//------------------------------------------------------------------------
// GUIController::createView
//------------------------------------------------------------------------
IPlugView *GUIController::createView(const char *name)
{
  if(name && strcmp(name, ViewType::kEditor) == 0)
  {
    UIDescription *uiDescription = new UIDescription(fXmlFileName, fViewFactory);
    return new VSTGUI::VST3Editor(uiDescription, this, "view", fXmlFileName);
  }
  return nullptr;
}

//------------------------------------------------------------------------
// VAC6Controller::setComponentState
//------------------------------------------------------------------------
tresult GUIController::setComponentState(IBStream *state)
{
  // we receive the current state of the component (processor part)
  if(state == nullptr)
    return kResultFalse;

  std::unique_ptr<NormalizedState> normalizedState = nullptr;

#ifdef JAMBA_DEBUG_LOGGING
  auto const &order = fGUIParameters->getPluginParameters().getRTSaveStateOrder();
  normalizedState = std::make_unique<NormalizedState>(order.getParamCount());
#endif

  // using helper to read the stream
  IBStreamer streamer(state, kLittleEndian);
  
  tresult res = fGUIParameters->readRTState(streamer, normalizedState.get());

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIController::readRTState - v=%d, %s", order.fVersion, normalizedState->toString(order.fOrder.data()).c_str());
#endif

  return res;
}

//------------------------------------------------------------------------
// VAC6Controller::setState
//------------------------------------------------------------------------
tresult GUIController::setState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  std::unique_ptr<NormalizedState> normalizedState = nullptr;

#ifdef JAMBA_DEBUG_LOGGING
  auto const &order = fGUIParameters->getPluginParameters().getGUISaveStateOrder();
  normalizedState = std::make_unique<NormalizedState>(order.getParamCount());
#endif

  IBStreamer streamer(state, kLittleEndian);
  tresult res = fGUIParameters->readGUIState(streamer, normalizedState.get());

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIController::readGUIState - v=%d, %s", order.fVersion, normalizedState->toString(order.fOrder.data()).c_str());
#endif

  return res;
}

//------------------------------------------------------------------------
// VAC6Controller::getState
//------------------------------------------------------------------------
tresult GUIController::getState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  std::unique_ptr<NormalizedState> normalizedState = nullptr;

#ifdef JAMBA_DEBUG_LOGGING
  auto const &order = fGUIParameters->getPluginParameters().getGUISaveStateOrder();
  normalizedState = std::make_unique<NormalizedState>(order.getParamCount());
#endif

  IBStreamer streamer(state, kLittleEndian);
  tresult res = fGUIParameters->writeGUIState(streamer, normalizedState.get());

#ifdef JAMBA_DEBUG_LOGGING
  DLOG_F(INFO, "GUIController::writeGUIState - v=%d, %s", order.fVersion, normalizedState->toString(order.fOrder.data()).c_str());
#endif

  return res;
}

//------------------------------------------------------------------------
// VAC6Controller::getState
//------------------------------------------------------------------------
void GUIController::registerViewState(std::shared_ptr<GUIViewState> iViewState)
{
  if(std::find(fViewStates.cbegin(), fViewStates.cend(), iViewState) != fViewStates.cend())
  {
    DLOG_F(WARNING, "Registering same GUIViewState multiple times (ignored");
  }
  else
  {
    fViewStates.emplace_back(std::move(iViewState));
  }
}

}
}
}