#include <utility>

#pragma once

#include <vstgui4/vstgui/uidescription/uiviewfactory.h>
#include <pongasoft/VST/GUI/Params/GUIParameters.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace Params;

/**
 * interface to access vst parameters
 */
class GUIParametersProvider
{
public:
  virtual std::shared_ptr<GUIParameters> getGUIParameters() const = 0;
};

/**
 * Custom view factory to give access to vst parameters
 */
class CustomUIViewFactory : public VSTGUI::UIViewFactory, public GUIParametersProvider
{
public:
  explicit CustomUIViewFactory(std::shared_ptr<GUIParameters> iGUIParameters) :
    fGUIParameters{std::move(iGUIParameters)}
  {
  }

  ~CustomUIViewFactory() override = default;

  std::shared_ptr<GUIParameters> getGUIParameters() const override
  {
    return fGUIParameters;
  }

private:
  std::shared_ptr<GUIParameters> fGUIParameters{nullptr};
};


}
}
}
}