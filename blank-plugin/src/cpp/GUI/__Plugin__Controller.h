#pragma once

#include <pongasoft/VST/GUI/GUIController.h>
#include "../[-name-].h"

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
  static FUnknown *createInstance(void *iContext) {
    return (IEditController *) new [-name-]Controller(*reinterpret_cast<[-name-]Parameters *>(iContext));
  }

public:
  // Constructor
  explicit [-name-]Controller([-name-]Parameters const &iParams);

  // Destructor -- overridden for debugging purposes only
  ~[-name-]Controller() override;

  // getGUIState
  GUIState *getGUIState() override { return &fState; }

protected:
  // initialize
  tresult PLUGIN_API initialize(FUnknown *context) override;

private:
  // The controller gets its own copy of the parameters (defined in Plugin.h)
  [-name-]Parameters const &fParams;

  // The state accessible in the controller and views
  [-name-]GUIState fState;
};

}

