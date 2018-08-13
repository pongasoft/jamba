/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
#pragma once

#include <memory>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <vstgui4/vstgui/lib/cframe.h>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/MessageProducer.h>
#include "pongasoft/VST/MessageHandler.h"

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

/**
 * Base class from which the actual controller inherits from. Handles most of the "framework" logic,
 * (state loading/saving in a thread safe manner, registering VST parameters...) so
 * that the actual controller code deals mostly with business logic.
 */
class GUIController : public EditController, public IMessageProducer
{
public:
  // Constructor
  explicit GUIController(char const *iXmlFileName = "Views.uidesc");

  // Destructor
  ~GUIController() override;

  /**
   * Subclasses must implement this method to return the state
   */
  virtual GUIState *getGUIState() = 0;

protected:
  /** Called at first after constructor */
  tresult PLUGIN_API initialize(FUnknown *context) override;

  /** Called at the end before destructor */
  tresult PLUGIN_API terminate() override;

  /** Create the view */
  IPlugView *PLUGIN_API createView(const char *name) override;

  /** Sets the component state (after setting the processor) or after restore */
  tresult PLUGIN_API setComponentState(IBStream *state) override;

  /** Restore the state (UI only!) (ex: after loading preset or project) */
  tresult PLUGIN_API setState(IBStream *state) override;

  /** Called to save the state (UI only!) (before saving a preset or project) */
  tresult PLUGIN_API getState(IBStream *state) override;

  /** Called to handle a message (coming from RT) */
  tresult PLUGIN_API notify(IMessage *message) SMTG_OVERRIDE;

public:
  // allocateMessage - API adapter
  IPtr<IMessage> allocateMessage() override;

  // sendMessage - API adapter
  tresult sendMessage(IPtr<IMessage> iMessage) override;

protected:
  // the name of the xml file (relative) which contains the ui description
  char const *const fXmlFileName;

  // the default knob mode to use (you can override it in your controller)
  VSTGUI::CKnobMode fDefaultKnobMode{VSTGUI::CKnobMode::kLinearMode};

private:
  // view factory used to give access to GUIState to views
  Views::CustomUIViewFactory *fViewFactory{nullptr};

};

}
}
}