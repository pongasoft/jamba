/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
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
#include <pongasoft/VST/GUI/Params/GUIParamCxAware.h>

namespace pongasoft {
namespace VST {
namespace GUI {

using namespace VSTGUI;
using namespace Params;

/**
 * The idea is that an instance of this class is being owned by the main controller (thus its lifespan is the entire
 * life of the controller) but the views assigned to it can come and go (as the user opens/closes the UI of the plugin)
 */
class GUIViewState : public IViewListenerAdapter, public GUIParamCxAware
{
public:
  /**
   * Called by the view when it is interested in updates from this state
   *
   * @param iView
   */
  void registerForUpdate(CView *iView)
  {
    DCHECK_NOTNULL_F(iView, "assign should not receive null pointer");

    if(std::find(fViews.cbegin(), fViews.cend(), iView) != fViews.cend())
    {
      DLOG_F(WARNING, "view already registered.. ignoring...");
    }
    else
    {
      fViews.emplace_back(iView);
      iView->registerViewListener(this);
    }
  }

  /**
   * By default sets the view dirty
   */
  virtual void updateViews()
  {
    for(auto view : fViews)
      view->setDirty(true);
  }

protected:
  /**
   * Callback that is called when the host is about to delete the view and as a result it needs to be unassigned from
   * this instance
   */
  void viewWillDelete(CView *iView) override
  {
    auto iter = std::find(fViews.cbegin(), fViews.cend(), iView);
    DCHECK_F(iter != fViews.cend(), "views should have been registered before!");
    fViews.erase(iter);
    iView->unregisterViewListener(this);
  };

protected:
  std::vector<CView *> fViews{};
};

/**
 * This subclass gives access to plugin parameters with the fParams variable (similar to PluginCustomView)
 *
 * @tparam TGUIPluginState type of the plugin parameters class (should be a subclass of GUIPluginState<>)
*/
template<typename TGUIPluginState>
class PluginGUIViewState : public GUIViewState
{
public:
  void initState(GUIState *iGUIState) override
  {
    GUIViewState::initState(iGUIState);
    if(fParamCxMgr)
    {
      fState = dynamic_cast<TGUIPluginState *>(fParamCxMgr->getGUIState());
      fParams = &fState->fParams;
    }
  }

public:
  // direct access to state (ex: fParams->fBypassParam)
  TGUIPluginState const *fState{};

  // direct access to parameters (ex: fParams->fBypassParam)
  typename TGUIPluginState::PluginParameters const *fParams{};
};

}
}
}