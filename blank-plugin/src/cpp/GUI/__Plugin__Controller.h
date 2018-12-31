#pragma once

#include <pongasoft/VST/GUI/GUIController.h>
#include "../[-name-].h"

[-namespace_start-]
namespace GUI {

using namespace pongasoft::VST::GUI;

//------------------------------------------------------------------------
// [-name-]Controller - Main GUI Controller
//------------------------------------------------------------------------
class [-name-]Controller : public GUIController
{
public:
  //------------------------------------------------------------------------
  // Factory method used in [-name-]_VST3.cpp to create the controller
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void * /*context*/) { return (IEditController *) new [-name-]Controller(); }

public:
  // Constructor
  [-name-]Controller();

  // Destructor -- overridden for debugging purposes only
  ~[-name-]Controller() override;

  // getGUIState
  GUIState *getGUIState() override { return &fState; }

protected:
  // initialize
  tresult initialize(FUnknown *context) override;

private:
  // The controller gets its own copy of the parameters (defined in Plugin.h)
  [-name-]Parameters fParams;

  // The state accessible in the controller and views
  [-name-]GUIState fState;
};

}
[-namespace_end-]

