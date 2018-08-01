#pragma once

#include <vstgui4/vstgui/uidescription/uiviewfactory.h>
#include <pongasoft/VST/GUI/GUIState.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace Params;

/**
 * interface to access gui state
 */
class GUIStateProvider
{
public:
  virtual GUIState *getGUIState() const = 0;
};

/**
 * Custom view factory to give access to vst parameters
 */
class CustomUIViewFactory : public VSTGUI::UIViewFactory, public GUIStateProvider
{
public:
  explicit CustomUIViewFactory(GUIState *iGUIState) : fGUIState{iGUIState}
  {
  }

  ~CustomUIViewFactory() override = default;

  GUIState *getGUIState() const override
  {
    return fGUIState;
  }

private:
  GUIState *fGUIState{};
};


}
}
}
}