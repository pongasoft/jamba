#include <vstgui4/vstgui/lib/cview.h>
#include "ViewCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {


//------------------------------------------------------------------------
// ViewCxMgr::registerConnection
//------------------------------------------------------------------------
void ViewCxMgr::registerConnection(CView *iView, ViewCxMgr::FObjectCxUPtr iFObjectCx)
{
  if(iView == nullptr || iFObjectCx == nullptr)
    return;

  auto iter = fViewConnections.find(iView);

  if(iter == fViewConnections.end())
  {
    iView->registerViewListener(this);
    fViewConnections[iView] = GUIParamCxVector{};
  }

  fViewConnections[iView].emplace_back(std::move(iFObjectCx));
}

//------------------------------------------------------------------------
// ViewCxMgr::closeAll
//------------------------------------------------------------------------
void ViewCxMgr::closeAll()
{
  fViewConnections.clear();
}

//------------------------------------------------------------------------
// ViewCxMgr::viewWillDelete
//------------------------------------------------------------------------
void ViewCxMgr::viewWillDelete(CView *iView)
{
  auto iter = fViewConnections.find(iView);

  if(iter != fViewConnections.end())
  {
    iView->unregisterViewListener(this);
    fViewConnections.erase(iter);
  }
}

}
}
}