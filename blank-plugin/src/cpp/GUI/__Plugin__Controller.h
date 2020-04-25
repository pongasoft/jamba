#ifndef VST3_JAMBA_[-name-]_GUI_CONTROLLER_H
#define VST3_JAMBA_[-name-]_GUI_CONTROLLER_H

#include <pongasoft/VST/GUI/GUIController.h>
#include "../Plugin.h"

namespace [-namespace-]::GUI {

using namespace pongasoft::VST::GUI;

//------------------------------------------------------------------------
// [-name-]Controller - Main GUI Controller
//------------------------------------------------------------------------
class [-name-]Controller : public GUIController
{
public:
  //------------------------------------------------------------------------
  // UUID() method used to create the controller
  //------------------------------------------------------------------------
  static inline ::Steinberg::FUID UUID() { return [-name-]ControllerUID; };

  //------------------------------------------------------------------------
  // Factory method used to create the controller
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void * /*iContext */) { return (IEditController *) new [-name-]Controller(); }

public:
  // Constructor
  explicit [-name-]Controller();

  // Destructor -- overridden for debugging purposes only
  ~[-name-]Controller() override;

  // getGUIState
  GUIState *getGUIState() override { return &fState; }

protected:
  // initialize
  tresult PLUGIN_API initialize(FUnknown *context) override;

private:
  // The controller gets a copy of the parameters (defined in Plugin.h)
  [-name-]Parameters fParams;

  // The state accessible in the controller and views
  [-name-]GUIState fState;
};

}

#endif // VST3_JAMBA_[-name-]_GUI_CONTROLLER_H

