/*
 * Copyright (c) 2018-2019 pongasoft
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
#pragma once

#include <memory>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <vstgui4/vstgui/lib/cframe.h>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include <pongasoft/VST/GUI/GUIState.h>
#include <pongasoft/VST/MessageProducer.h>
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.h>
#include <pongasoft/VST/MessageHandler.h>
#include <vstgui4/vstgui/plugin-bindings/vst3editor.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

/**
 * Base class from which the actual controller inherits from. Handles most of the "framework" logic,
 * (state loading/saving in a thread safe manner, registering VST parameters...) so
 * that the actual controller code deals mostly with business logic.
 */
class GUIController : public EditController, public VSTGUI::VST3EditorDelegate, public IMessageProducer
{
public:
  // Constructor
  explicit GUIController(char const *iXmlFileName = "Views.uidesc",
                         char const *iMainViewName = "view");

  // Destructor
  ~GUIController() override;

  /**
   * Subclasses must implement this method to return the state
   */
  virtual GUIState *getGUIState() = 0;

  /**
   * Subclasses should override this method to return the custom controller or nullptr if doesn't match the name
   */
  virtual IController *createCustomController(UTF8StringPtr iName,
                                              IUIDescription const *iDescription,
                                              IController *iBaseController) { return nullptr; };

protected:
  /** Called at first after constructor */
  tresult PLUGIN_API initialize(FUnknown *context) override;

  /** Called at the end before destructor */
  tresult PLUGIN_API terminate() override;

  /** Create the view */
  IPlugView *PLUGIN_API createView(const char *name) override;

  // didOpen -> track lifecycle of editor (open)
  void didOpen(VST3Editor *editor) override;

  // willClose -> track lifecycle of editor (close)
  void willClose(VST3Editor *editor) override;

protected:
  /** Sets the component state (after setting the processor) or after restore */
  tresult PLUGIN_API setComponentState(IBStream *state) override;

  /** Restore the state (UI only!) (ex: after loading preset or project) */
  tresult PLUGIN_API setState(IBStream *state) override;

  /** Called to save the state (UI only!) (before saving a preset or project) */
  tresult PLUGIN_API getState(IBStream *state) override;

  /** Called to handle a message (coming from RT) */
  tresult PLUGIN_API notify(IMessage *message) SMTG_OVERRIDE;

  /** Called when a sub controller needs to be created */
  IController *createSubController(UTF8StringPtr iName,
                                   const IUIDescription *iDescription,
                                   VST3Editor *iEditor) override;

  // registerParameters (if not nullptr)
  void registerParameters(GUIParamCxAware *iGUIParamCxAware);

  /**
   * This method should be called to display a totally different (root) view.
   *
   * Example: with `xxx.uidesc` like this one, you would call `switchToView("compact_view")`;
   *
   *     <vstgui-ui-description version="1">
   *       <!-- Main view opened on creation -->
	 *       <template class="CViewContainer" name="view" ...>
   *       </template>
   *       <!-- Secondary view you can switch to -->
	 *       <template class="CViewContainer" name="compact_view" ...>
   *       </template>
   *     <vstgui-ui-description
   *
   * @return `true` if switching worked
   */
  virtual bool switchToView(char const *iViewName);

  /**
   * Switch back to the main view
   */
  virtual bool switchToMainView() { return switchToView(fMainViewName.c_str()) ; }

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

  // The name of the main view
  std::string fMainViewName;

  // The name of the current view
  std::string fCurrentViewName;

  // we keep a reference to the editor to be able to switch views
  VSTGUI::VST3Editor *fVST3Editor{};

};

}
}
}