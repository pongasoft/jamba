#include "GUIRawParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIRawParameter::Editor::Editor
///////////////////////////////////////////
GUIRawParameter::Editor::Editor(ParamID iParamID, HostParameters const &iHostParameters) :
  fParamID{iParamID},
  fHostParameters{iHostParameters}
{
// DLOG_F(INFO, "GUIRawParameter::Editor(%d)", fParamID);
  fHostParameters.beginEdit(fParamID);
  fIsEditing = true;
  fInitialParamValue = fHostParameters.getParamNormalized(fParamID);
}

///////////////////////////////////////////
// GUIRawParameter::Editor::setValue
///////////////////////////////////////////
tresult GUIRawParameter::Editor::setValue(ParamValue iValue)
{
  tresult res = kResultFalse;
  if(fIsEditing)
  {
    res = fHostParameters.setParamNormalized(fParamID, iValue);
    if(res == kResultOk)
      fHostParameters.performEdit(fParamID, fHostParameters.getParamNormalized(fParamID));
  }
  return res;
}

///////////////////////////////////////////
// GUIRawParameter::Editor::commit
///////////////////////////////////////////
tresult GUIRawParameter::Editor::commit()
{
  if(fIsEditing)
  {
    fIsEditing = false;
    fHostParameters.endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawParameter::Editor::rollback
///////////////////////////////////////////
tresult GUIRawParameter::Editor::rollback()
{
  if(fIsEditing)
  {
    setValue(fInitialParamValue);
    fIsEditing = false;
    fHostParameters.endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawParameter::Connection::Connection
///////////////////////////////////////////
GUIRawParameter::Connection::Connection(ParamID iParamID,
                                        HostParameters const &iHostParameters,
                                        IChangeListener *iChangeListener)  :
  fParamID{iParamID},
  fHostParameters{iHostParameters},
  fChangeListener{iChangeListener}
{
  // DLOG_F(INFO, "GUIRawParameter::Connection(%d)", fParamID);

  fParameter = fHostParameters.getParameterObject(fParamID);

  DCHECK_NOTNULL_F(fParameter);
  DCHECK_NOTNULL_F(fChangeListener);

  fParameter->addRef();
  fParameter->addDependent(this);
  fIsConnected = true;
}

///////////////////////////////////////////
// GUIRawParameter::Connection::close
///////////////////////////////////////////
void GUIRawParameter::Connection::close()
{
  if(fIsConnected)
  {
    fParameter->removeDependent(this);
    fParameter->release();
    fIsConnected = false;
  }
}

///////////////////////////////////////////
// GUIRawParameter::Connection::update
///////////////////////////////////////////
void PLUGIN_API GUIRawParameter::Connection::update(FUnknown *iChangedUnknown, Steinberg::int32 iMessage)
{
  if(iMessage == IDependent::kChanged)
  {
    fChangeListener->onParameterChange(fParamID, fHostParameters.getParamNormalized(fParamID));
  }
}

///////////////////////////////////////////
// GUIRawParameter::GUIRawParameter
///////////////////////////////////////////
GUIRawParameter::GUIRawParameter(ParamID iParamID, HostParameters const &iHostParameters)  :
  fParamID{iParamID},
  fHostParameters{iHostParameters}
{
  // DLOG_F(INFO, "GUIRawParameter::GUIRawParameter(%d)", fParamID);
  DCHECK_F(fHostParameters.exists(fParamID), "Missing parameter [%d]", iParamID);
}

}
}
}
}