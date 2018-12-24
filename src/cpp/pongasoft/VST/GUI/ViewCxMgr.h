/*
 * Copyright (c) 2018 pongasoft
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

#include <unordered_map>
#include <vector>
#include <pongasoft/VST/FObjectCx.h>
#include <vstgui4/vstgui/lib/iviewlistener.h>
#include <pongasoft/VST/GUI/Params/GUIJmbParameter.h>
#include <pongasoft/VST/GUI/Params/GUIVstParameter.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;
using namespace Params;

/**
 * This class will manage the callbacks registered against a view (CView). The idea is to be able to register callbacks
 * for the life of a view without having to inherit from the view to implement a similar behavior (which is much more
 * involved).
 *
 * Example usage:
 *
 * ViewCxMgr *mgr = ...;
 * TextButtonView *button = ....;
 * mgr->registerConnectionFor(button).callback<int>(fState->fMyParam,
 *   [] (TextButtonView *iButton, GUIVstParam<int> &iParam) {
 *   iButton->setMouseEnabled(iParam > 3);
 * });
 *
 * This examples sets up a callback on the button view, so that when the (Vst) parameter "my param"
 * changes, the callback is invoked. Since the view and the parameter are part of the callback API, it is easy to
 * implement simple behaviors like this. Usually this can be done from the main controller (or sub controller) in the
 * verifyView method.
 *
 * An alternative would be to inherit from the view which is more work (especially if the only purpose is add a
 * simple callback like behavior):
 *
 * class MyView : public TextButtonView, PluginAccessor<MyGUIState>
 * {
 *   public:
 *     // constructor
 *     MyView(...);
 *
 *     void registerParameters() override
 *     {
 *        registerVstCallback(fState->fMyParam, [this] (GUIVstParam<int> &iParam) {
 *          setMouseEnabled(iParam > 3);
 *        });
 *     }
 * };
 */
class ViewCxMgr : private IViewListenerAdapter
{
public:
  /**
   * Builder pattern to add multiple callbacks to a view
   */
  template<typename TView>
  struct ViewCallbackBuilder
  {
    /**
     * Adds a callback to handle changes on a Vst parameter
     *
     * @param iInvokeCallback whether to invoke the callback right away or not
     */
    template<typename T>
    inline void callback(VstParam<T> const &iParamDef,
                         Parameters::ChangeCallback2<TView, GUIVstParam<T>> iChangeCallback,
                         bool iInvokeCallback = false)
    {
      if(iChangeCallback)
      {
        auto param = fMgr->fGUIVstParameterMgr->getGUIVstParameter(iParamDef);
        if(param)
        {
          // Implementation note:
          // 1) using std::make_unique results in an error on std::move(callback) because unique_ptr
          // cannot be copied...
          // 2) we need to access the ptr afterwards to call connect
          auto ptr = std::make_shared<GUIVstParam<T>>(std::move(param));
          auto callback = [view = this->fView, ptr, cb2 = std::move(iChangeCallback)] () {
            cb2(view, *ptr);
          };

          if(iInvokeCallback)
            callback();

          fMgr->registerConnection(fView, ptr->connect(std::move(callback)));
        }
      }
    }

    /**
     * Adds a callback to handle changes on a Jmb parameter
     *
     * @param iInvokeCallback whether to invoke the callback right away or not
     */
    template<typename T>
    inline void callback(GUIJmbParam<T> &iParam,
                         Parameters::ChangeCallback2<TView, GUIJmbParam<T>> iChangeCallback,
                         bool iInvokeCallback = false)
    {
      if(iChangeCallback)
      {
        auto callback = [view = this->fView, &iParam, cb2 = std::move(iChangeCallback)] () {
          cb2(view, iParam);
        };

        if(iInvokeCallback)
          callback();

        fMgr->registerConnection(fView, std::move(iParam.connect(std::move(callback))));
      }
    }

    friend class ViewCxMgr;

  private:
    // Constructor
    ViewCallbackBuilder(TView *iView, ViewCxMgr *iMgr) : fView{iView}, fMgr{iMgr} {}

    TView *fView;
    ViewCxMgr *fMgr;
  };

public:
  // Constructor
  explicit ViewCxMgr(GUIVstParameterMgr *iMgr) : fGUIVstParameterMgr{iMgr} {};

  /**
   * @param TView should be a subclass of VSTGUI::CView
   * @return a builder class to add callback for this view (see comment at the top for usages)
   */
  template<typename TView>
  inline ViewCallbackBuilder<TView> registerConnectionFor(TView *iView) { return {iView, this};}

  /**
   * Close all previously established connections
   */
  void closeAll();

private:
  using FObjectCxUPtr = std::unique_ptr<FObjectCx>;
  using GUIParamCxVector = std::vector<FObjectCxUPtr>;

  /**
   * Called by the builder class to add a connection for this view
   */
  void registerConnection(CView *iView, FObjectCxUPtr iFObjectCx);

  /**
   * Called by VSTGUI when a view that was previously registered is being deleted => all its connections will be closed
   */
  void viewWillDelete(CView *iView) override;

private:
  GUIVstParameterMgr *fGUIVstParameterMgr;
  std::unordered_map<CView *, GUIParamCxVector> fViewConnections{};
};

}
}
}