/*
 * Copyright (c) 2019 pongasoft
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

#include <vstgui4/vstgui/lib/cframe.h>
#include <pongasoft/VST/GUI/Types.h>
#include "SelfContainedViewListener.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

/**
 * The `CView` class provides 2 methods to handle keyboard events (`CView::onKeyDown` and `CView::onKeyUp`) which
 * only works as long as the view itself has focus. This helper class is designed to set a keyboard hook which is
 * global so it will work whether the view has the focus or not. This class automatically handles registering
 * the hook to the frame (when it becomes available) and unregistering it automatically as well. As an added bonus
 * it is possible to dynamically change the callbacks after registration.
 *
 * Typical usage would be to use Views::registerGlobalKeyboardHook this way:
 *
 *     // assuming we are writing a custom view
 *     void initState(GUIState *iGUIState)
 *     {
 *       CustomView::initState(iGUIState);
 *       Views::registerGlobalKeyboardHook(this)
 *         ->onKeyDown([this](VstKeyCode const &iKeyCode) -> auto {
 *           if(iKeyCode.character == 'z')
 *           {
 *             // do something...
 *             return CKeyboardEventResult::kKeyboardEventHandled;
 *           }
 *           return CKeyboardEventResult::kKeyboardEventNotHandled;
 *         })
 *         ->onKeyUp([this](VstKeyCode const &iKeyCode) -> auto {
 *           if(iKeyCode.character == 'z')
 *           {
 *             // do something else...
 *             return CKeyboardEventResult::kKeyboardEventHandled;
 *           }
 *           return CKeyboardEventResult::kKeyboardEventNotHandled;
 *         });
 *      }
 */
class GlobalKeyboardHook : public SelfContainedViewListener, protected IKeyboardHook
{
public:
  /**
   * Call this method to register the callback invoked on a key down event. It replaces
   * any other if there was one and is optional.
   *
   * @param iOnKeyDownCallback see GUI::KeyboardEventCallback for the function definition
   * @return itself (as a shared pointer) for chaining calls (see example at the top)
   */
  std::shared_ptr<GlobalKeyboardHook> onKeyDown(KeyboardEventCallback iOnKeyDownCallback)
  {
    fOnKeyDownCallback = std::move(iOnKeyDownCallback);
    return std::dynamic_pointer_cast<GlobalKeyboardHook>(shared_from_this());
  }

  /**
   * Call this method to register the callback invoked on a key up event. It replaces
   * any other if there was one and is optional.
   *
   * @param iOnKeyUpCallback see GUI::KeyboardEventCallback for the function definition
   * @return itself (as a shared pointer) for chaining calls (see example at the top)
   */
  std::shared_ptr<GlobalKeyboardHook> onKeyUp(KeyboardEventCallback iOnKeyUpCallback)
  {
    fOnKeyUpCallback = std::move(iOnKeyUpCallback);
    return std::dynamic_pointer_cast<GlobalKeyboardHook>(shared_from_this());
  }

  /**
   * Factory method to create a global keyboard hook attached to the view. You can use the convenient global function
   * Views::registerGlobalKeyboardHook instead.
   *
   * @param iView the target view of the keyboard events
   * @return the helper class to register callback on
   */
  static std::shared_ptr<GlobalKeyboardHook> create(CView *iView)
  {
    return SelfContainedViewListener::create<GlobalKeyboardHook>(iView);
  }

  /**
   * This method overrides SelfContainedViewListener::registerView to account for the fact that the frame
   * (on which the global keyboard hook is registered) may already be available. We just don't know at which point
   * in the lifecycle of the view, this method is being called.
   */
  std::shared_ptr<SelfContainedViewListener> registerView(CView *iView) override
  {
    auto res = SelfContainedViewListener::registerView(iView);
    maybeRegisterKeyboardHook();
    return res;
  }

protected:
  /**
   * Overridden to handle attaching the keyboard hook to the frame
   */
  void viewAttached(CView *iView) override
  {
    DCHECK_F(iView == fView);
    maybeRegisterKeyboardHook();
  }

  /**
   * Overridden to handle detaching the keyboard hook from the frame
   */
  void viewRemoved(CView *iView) override
  {
    DCHECK_F(iView == fView);
    maybeUnregisterKeyboardHook();
  }

public:
  /**
   * If you need to unregister the keyboard hook early (meaning before the view is deleted), you can do so by
   * calling this method.
   */
  void unregister() override
  {
    maybeUnregisterKeyboardHook();
    SelfContainedViewListener::unregister();
  }

  /**
   * public constructor for the sole purpose of `std::make_shared`. Do not call directly */
  GlobalKeyboardHook() = default;

protected:
  // maybeRegisterKeyboardHook
  void maybeRegisterKeyboardHook()
  {
    if(!fFrame)
    {
      fFrame = fView->getFrame();
      if(fFrame)
      {
        fFrame->registerKeyboardHook(this);
      }
    }
  }

  // maybeUnregisterKeyboardHook
  void maybeUnregisterKeyboardHook()
  {
    if(fFrame)
    {
      fFrame->unregisterKeyboardHook(this);
      fFrame = nullptr;
    }
  }

  /**
   * Delegate to callback
   */
  int32_t onKeyDown(const VstKeyCode &iCode, CFrame *frame) override
  {
    return fOnKeyDownCallback ? fOnKeyDownCallback(iCode): CKeyboardEventResult::kKeyboardEventNotHandled;
  }

  /**
   * Delegate to callback
   */
  int32_t onKeyUp(const VstKeyCode &iCode, CFrame *frame) override
  {
    return fOnKeyUpCallback ? fOnKeyUpCallback(iCode): CKeyboardEventResult::kKeyboardEventNotHandled;
  }

protected:
  CFrame *fFrame{};
  KeyboardEventCallback fOnKeyDownCallback{};
  KeyboardEventCallback fOnKeyUpCallback{};
};

/**
 * This is the main entry point that you should use to register a global keyboard hook. See GlobalKeyboardHook
 * for an example.
 *
 * @param iView the view that becomes the target of key events even if it doesn't have the focus
 * @return the helper class on which you register the `onKeyDown` and `onKeyUp` callbacks
 */
inline std::shared_ptr<GlobalKeyboardHook> registerGlobalKeyboardHook(CView *iView) { return GlobalKeyboardHook::create(iView); }

}
}
}
}