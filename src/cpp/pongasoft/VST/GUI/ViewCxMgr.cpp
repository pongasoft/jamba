#include <vstgui4/vstgui/lib/cview.h>
#include "ViewCxMgr.h"

namespace pongasoft {
namespace VST {
namespace GUI {


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