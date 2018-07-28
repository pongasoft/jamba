#pragma once

#include <memory>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <vstgui4/vstgui/lib/cframe.h>
#include <pongasoft/VST/GUI/Params/GUIParameters.h>
#include <pongasoft/VST/GUI/Views/CustomViewFactory.h>
#include <pongasoft/VST/GUI/GUIViewState.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace Params;

/**
 * Base class from which the actual controller inherits from. Handles most of the "framework" logic,
 * (state loading/saving in a thread safe manner, registering VST parameters...) so
 * that the actual controller code deals mostly with business logic.
 */
class GUIController : public EditController
{
public:
  // Constructor
  explicit GUIController(char const *iXmlFileName = "Views.uidesc");

  // Destructor
  ~GUIController() override;

  /**
   * Must be implemented to return the "base" plugin parameter class
   */
  virtual pongasoft::VST::Parameters const *getPluginParameters() const = 0;

  /**
   * Registers the view state used by the (sub) controller so that they get properly initialized  */
  void registerViewState(std::shared_ptr<GUIViewState> iViewState);

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

protected:
  // the name of the xml file (relative) which contains the ui description
  char const *const fXmlFileName;

  // the parameters
  std::shared_ptr<GUIParameters> fGUIParameters{nullptr};

  // the default knob mode to use (you can override it in your controller)
  VSTGUI::CKnobMode fDefaultKnobMode{VSTGUI::CKnobMode::kLinearMode};

private:
  // keeps track of the view states
  std::vector<std::shared_ptr<GUIViewState>> fViewStates{};

  // view factory used to give access to GUIParameters to views
  Views::CustomUIViewFactory *fViewFactory{nullptr};

};

}
}
}