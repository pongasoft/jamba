#include "GUIParamCx.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// GUIParamCx::GUIParamCx
//------------------------------------------------------------------------
GUIParamCx::GUIParamCx(ParamID iParamID, FObject *iParameter, Parameters::IChangeListener *iChangeListener) :
  fParamID{iParamID},
  fParameter{iParameter},
  fChangeListener{iChangeListener}
{
  DCHECK_F(fParameter != nullptr);
  DCHECK_F(fChangeListener != nullptr);

  fParameter->addRef();
  fParameter->addDependent(this);
  fIsConnected = true;
}

//------------------------------------------------------------------------
// GUIParamCx::close
//------------------------------------------------------------------------
void GUIParamCx::close()
{
  if(fIsConnected)
  {
    fParameter->removeDependent(this);
    fParameter->release();
    fIsConnected = false;
  }
}

//------------------------------------------------------------------------
// GUIParamCx::update
//------------------------------------------------------------------------
void GUIParamCx::update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage)
{
  if(iMessage == IDependent::kChanged)
  {
    fChangeListener->onParameterChange(fParamID);
  }
}

}
}
}
}