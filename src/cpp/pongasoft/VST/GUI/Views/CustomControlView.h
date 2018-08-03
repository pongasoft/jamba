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
template<typename T>
class TCustomControlView : public CustomControlView
{
public:
  // TCustomControlView
  explicit TCustomControlView(const CRect &iSize) : CustomControlView(iSize) {}

public:
  CLASS_METHODS_NOCOPY(CustomControlView, TCustomControlView)

  // set/getControlValue
  T getControlValue() const;
  void setControlValue(T const &iControlValue);

  // registerParameters
  void registerParameters() override;

protected:
  // the gui parameter tied to the control
  GUIVstParam<T> fControlParameter{nullptr};

#if EDITOR_MODE
  // the value (in sync with control parameter but may exist on its own in editor mode)
  T fControlValue{};
#endif

public:
  class Creator : public CustomViewCreator<TCustomControlView<T>, CustomControlView>
  {
  private:
    using CustomViewCreatorT = CustomViewCreator<TCustomControlView<T>, CustomControlView>;
  public:
    explicit Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
      CustomViewCreatorT(iViewName, iDisplayName)
    {
    }
  };
};

///////////////////////////////////////////
// TCustomControlView<T>::getControlValue
///////////////////////////////////////////
template<typename T>
T TCustomControlView<T>::getControlValue() const
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
// TCustomControlView<T>::setControlValue
///////////////////////////////////////////
template<typename T>
void TCustomControlView<T>::setControlValue(T const &iControlValue)
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
// TCustomControlView<T>::registerParameters
///////////////////////////////////////////
template<typename T>
void TCustomControlView<T>::registerParameters()
{
  CustomControlView::registerParameters();

  if(!fParamCxMgr || getControlTag() < 0)
    return; // not set yet

  auto paramID = static_cast<ParamID>(getControlTag());
  fControlParameter = registerVstParam<T>(paramID);

#if EDITOR_MODE
  if(fControlParameter)
    fControlValue = fControlParameter->getValue();
#else
  if(!fControlParameter)
    ABORT_F("Could not find parameter for control tag [%d]", paramID);
#endif
}

}
}
}
}
