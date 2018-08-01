#include "GUIRawVstParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

///////////////////////////////////////////
// GUIRawVstParameter::Editor::Editor
///////////////////////////////////////////
GUIRawVstParameter::Editor::Editor(ParamID iParamID, VstParametersSPtr iVstParameters) :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)}
{
// DLOG_F(INFO, "GUIRawVstParameter::Editor(%d)", fParamID);
  fVstParameters->beginEdit(fParamID);
  fIsEditing = true;
  fInitialParamValue = fVstParameters->getParamNormalized(fParamID);
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::setValue
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::setValue(ParamValue iValue)
{
  tresult res = kResultFalse;
  if(fIsEditing)
  {
    res = fVstParameters->setParamNormalized(fParamID, iValue);
    if(res == kResultOk)
      fVstParameters->performEdit(fParamID, fVstParameters->getParamNormalized(fParamID));
  }
  return res;
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::commit
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::commit()
{
  if(fIsEditing)
  {
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawVstParameter::Editor::rollback
///////////////////////////////////////////
tresult GUIRawVstParameter::Editor::rollback()
{
  if(fIsEditing)
  {
    setValue(fInitialParamValue);
    fIsEditing = false;
    fVstParameters->endEdit(fParamID);
    return kResultOk;
  }
  return kResultFalse;
}

///////////////////////////////////////////
// GUIRawVstParameter::GUIRawVstParameter
///////////////////////////////////////////
GUIRawVstParameter::GUIRawVstParameter(ParamID iParamID, VstParametersSPtr iVstParameters)  :
  fParamID{iParamID},
  fVstParameters{std::move(iVstParameters)}
{
  // DLOG_F(INFO, "GUIRawVstParameter::GUIRawVstParameter(%d)", fParamID);
  DCHECK_F(fVstParameters->exists(fParamID), "Missing parameter [%d]", iParamID);
}

}
}
}
}