/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
#include "GUIController.h"
#include <vstgui4/vstgui/plugin-bindings/vst3editor.h>
#include <pongasoft/VST/GUI/Views/JambaViews.h>

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
  Views::JambaViews::instance();
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

  // vst parameters
  VstParametersSPtr vstParameters = std::make_shared<VstParameters>(this);

  // initializing the state
  auto guiState = getGUIState();

  guiState->init(vstParameters, this);

  // making sure that the knob mode is set to the default specified
  CFrame::kDefaultKnobMode = fDefaultKnobMode;
  setKnobMode(fDefaultKnobMode);

  guiState->getPluginParameters().registerVstParameters(parameters);

  fViewFactory = new CustomUIViewFactory(guiState);

  registerParameters(dynamic_cast<GUIParamCxAware *>(this));

  return result;
}

//------------------------------------------------------------------------
// GUIController::registerParameters
//------------------------------------------------------------------------
void GUIController::registerParameters(GUIParamCxAware *iGUIParamCxAware)
{
  if(iGUIParamCxAware)
  {
    iGUIParamCxAware->initState(getGUIState());
    iGUIParamCxAware->registerParameters();
  }
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

  auto gpa = dynamic_cast<GUIParamCxAware *>(this);
  if(gpa)
    gpa->unregisterAll();

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
// GUIController::setComponentState
//------------------------------------------------------------------------
tresult GUIController::setComponentState(IBStream *state)
{
  // we receive the current state of the component (processor part)
  if(state == nullptr)
    return kResultFalse;

  // using helper to read the stream
  IBStreamer streamer(state, kLittleEndian);
  return getGUIState()->readRTState(streamer);
}

//------------------------------------------------------------------------
// GUIController::setState
//------------------------------------------------------------------------
tresult GUIController::setState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);
  return getGUIState()->readGUIState(streamer);
}

//------------------------------------------------------------------------
// GUIController::getState
//------------------------------------------------------------------------
tresult GUIController::getState(IBStream *state)
{
  if(state == nullptr)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);
  return getGUIState()->writeGUIState(streamer);
}

//------------------------------------------------------------------------
// GUIController::notify
//------------------------------------------------------------------------
tresult GUIController::notify(IMessage *message)
{
  if(!message)
    return kInvalidArgument;

  Message m{message};

  return getGUIState()->handleMessage(m);
}

//------------------------------------------------------------------------
// GUIController::allocateMessage
//------------------------------------------------------------------------
IPtr<IMessage> GUIController::allocateMessage()
{
  return Steinberg::owned(EditController::allocateMessage());
}

//------------------------------------------------------------------------
// GUIController::sendMessage
//------------------------------------------------------------------------
tresult GUIController::sendMessage(IPtr<IMessage> iMessage)
{
  return EditController::sendMessage(iMessage);
}

//------------------------------------------------------------------------
// GUIController::createSubController
//------------------------------------------------------------------------
IController *GUIController::createSubController(UTF8StringPtr iName,
                                                IUIDescription const *iDescription,
                                                VST3Editor * /* iEditor */)
{
  auto customController = createCustomController(iName, iDescription);
  registerParameters(dynamic_cast<GUIParamCxAware *>(customController));
  return customController;
}


}
}
}