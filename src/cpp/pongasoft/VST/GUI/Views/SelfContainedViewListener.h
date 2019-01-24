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

#include <vstgui4/vstgui/lib/iviewlistener.h>
#include <vstgui4/vstgui/lib/cview.h>
#include <pongasoft/logging/logging.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

/**
 * The purpose of this class is to implement a view listener that is "self-contained":
 *
 * - this class MUST live at least as long as it is registered with the view
 * - this class offers an easy way to unregister early (if the client code desires to do so) while making sure that
 *   the instance is still valid.
 *
 * The typical code usage would be
 *
 *     // Create your own listener
 *     class MyViewListener : public SelfContainedViewListener
 *     {
 *       public:
 *         // override whichever method from IViewListener that you are interested in
 *         // if you override viewWillDelete, make sure to call SelfContainedViewListener::viewWillDelete
 *     }
 *
 *     // Scenario 1: you only care about creating a listener for the duration of the view
 *     // we assume that it is being called from a custom controller
 *     // (but can be called directly from a view using "this")
 *     CView *verifyView(CView *iView, ...)
 *     {
 *       if(iView ....)
 *       {
 *         // create and "forget"... completely self contained, no need to keep a reference to it
 *         SelfContainedViewListener::create<MyViewListener>(iView);
 *       }
 *     }
 *
 *     // Scenario 2: you want to be able to unregister at a later time
 *     std::shared_ptr<MyViewListener> fListener{};
 *
 *     CView *verifyView(CView *iView, ...)
 *     {
 *       if(iView ....)
 *       {
 *         // keep a reference to it which is guaranteed to remain valid
 *         fListener = SelfContainedViewListener::create<MyViewListener>(iView);
 *       }
 *     }
 *
 *     void someCustomCode()
 *     {
 *      if(fListener)
 *        fListener->unregister();
 *     }
 *
 * Implementation note: internally this class keeps a `std::shared_ptr` to ifself for as long as `this` is registered
 * with the view itself, ensuring that this class is not going to be deleted until the view goes away (or it is
 * unregistered). If an outside piece of code also has a reference to this class which is scenario 2, then the class
 * itself won't be deleted until this other reference goes away.
 */
class SelfContainedViewListener : protected IViewListenerAdapter, public std::enable_shared_from_this<SelfContainedViewListener>
{
public:
  /**
   * This is the main method that should be used to create an instance of this class. It will automatically register
   * `this` class as a view listener to `iView`.
   *
   * @tparam T the actual type (should be a subtype of SelfContainedViewListener)
   * @tparam Args optional arguments templates for the `T` constructor
   * @param iView the view on which this class will be registered as a listener (must not be `nullptr`)
   * @param args optional arguments for the `T` constructor
   * @return the shared pointer which may be kept around (scenario 2) but is NOT required
   */
  template<typename T, typename ...Args>
  static std::shared_ptr<T> create(CView *iView, Args&& ...iArgs)
  {
    auto res = std::make_shared<T>(std::forward<Args>(iArgs)...);
    res->registerView(iView);
    return res;
  }

  /**
   * You can call this method at anytime to unregister `this` class as the listener to the view that was previously
   * registered in SelfContainedViewListener::create. It is not necessary to call it explicitely as it will be
   * automatically unregistered when the view gets deleted.
   */
  virtual void unregister()
  {
    if(fView)
    {
      fView->unregisterViewListener(this);
      fView = nullptr;
      fThis = nullptr;
    }
  }

  /**
   * The constructor which unfortunately has to be declared `public` for the purpose of a creating a shared instance.
   * Do not instantiate directly.
   */
  SelfContainedViewListener() = default;

  /**
   * Registers this class as a view listener. Note that this call requires to be called from a shared pointer. It is
   * called by SelfContainedViewListener::create.
   */
  virtual std::shared_ptr<SelfContainedViewListener> registerView(CView *iView)
  {
    DCHECK_F(iView != nullptr);

    // first we close any prior connection if there was one
    unregister();

    fView = iView;
    fView->registerViewListener(this);
    fThis = shared_from_this();

    return fThis;
  }

protected:

  /**
   * Called by the view itself when it is going to be deleted. It automatically unregisters itself and deletes
   * the `shared_ptr` so that the class can be properly deleted (when all other instances of the `shared_ptr` are gone)
   */
  void viewWillDelete(CView *iView) override
  {
    DCHECK_F(iView == fView);

    unregister();
  }

protected:
  std::shared_ptr<SelfContainedViewListener> fThis{};
  CView *fView{};
};

}
}
}
}