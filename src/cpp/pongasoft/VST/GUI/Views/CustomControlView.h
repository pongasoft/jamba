#pragma once

#include "CustomView.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

/**
 * Base class for custom views providing one parameter only (similar to CControl)
 */
class CustomControlView : public CustomView
{
public:
  explicit CustomControlView(const CRect &iSize) : CustomView(iSize) {}

  // get/setControlTag
  void setControlTag (int32_t iTag) { fControlTag = iTag; };
  int32_t getControlTag () const { return fControlTag; }

public:
  CLASS_METHODS_NOCOPY(CustomControlView, CustomView)

protected:
  int32_t fControlTag{-1};

public:
  class Creator : public CustomViewCreator<CustomControlView, CustomView>
  {
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreator(iViewName, iDisplayName)
    {
      registerTagAttribute("control-tag", &CustomControlView::getControlTag, &CustomControlView::setControlTag);
    }
  };
};

/**
 * Base class for custom views providing one parameter only (similar to CControl)
 * This base class automatically registers the custom control and also keeps a control value for the case when
 * the control does not exist (for example in editor the control tag may not be defined).
 */
template<typename ParamConverter>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(CustomControlView, TCustomControlView)

  // set/getControlValue
  typename ParamConverter::ParamType getControlValue() const;
  void setControlValue(typename ParamConverter::ParamType const &iControlValue);

  // registerParameters
  void registerParameters() override;

protected:
  // the gui parameter tied to the control
  GUIVstParam<ParamConverter> fControlParameter{nullptr};

#if EDITOR_MODE
  // the value (in sync with control parameter but may exist on its own in editor mode)
  typename ParamConverter::ParamType fControlValue{};
#endif

public:
  class Creator : public CustomViewCreator<TCustomControlView<ParamConverter>, CustomControlView>
  {
  private:
    using CustomViewCreatorT = CustomViewCreator<TCustomControlView<ParamConverter>, CustomControlView>;
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreatorT(iViewName, iDisplayName)
    {
    }
  };
};

///////////////////////////////////////////
// TCustomControlView<ParamConverter>::getControlValue
///////////////////////////////////////////
template<typename ParamConverter>
typename ParamConverter::ParamType TCustomControlView<ParamConverter>::getControlValue() const
{
#if EDITOR_MODE
  if(fControlParameter)
    return fControlParameter->getValue();
  else
    return fControlValue;
#else
  return fControlParameter->getValue();
#endif
}

///////////////////////////////////////////
// TCustomControlView<ParamConverter>::setControlValue
///////////////////////////////////////////
template<typename ParamConverter>
void TCustomControlView<ParamConverter>::setControlValue(typename ParamConverter::ParamType const &iControlValue)
{
#if EDITOR_MODE
  fControlValue = iControlValue;
  if(fControlParameter)
    fControlParameter->setValue(fControlValue);
#else
  fControlParameter->setValue(iControlValue);
#endif
}

///////////////////////////////////////////
// TCustomControlView<ParamConverter>::registerParameters
///////////////////////////////////////////
template<typename ParamConverter>
void TCustomControlView<ParamConverter>::registerParameters()
{
  CustomControlView::registerParameters();
  if(!fParamCxMgr)
    ABORT_F("fParamCxMgr should have been registered");

#if EDITOR_MODE
  if(getControlTag() >= 0)
  {
    auto paramID = static_cast<ParamID>(getControlTag());
    if(fParamCxMgr->existsVst(paramID))
    {
      fControlParameter = registerVstParam<ParamConverter>(paramID);
      fControlValue = fControlParameter->getValue();
    }
    else
    {
      DLOG_F(WARNING, "Parameter[%d] does not exist", paramID);
      fControlParameter = nullptr;
    }
  }
#else
  auto paramID = static_cast<ParamID>(getControlTag());
  if(fParamCxMgr->exists(paramID))
    fControlParameter = registerGUIParam<ParamConverter>(paramID);
  else
    ABORT_F("Could not find parameter for control tag [%d]", paramID);
#endif
}

}
}
}
}
