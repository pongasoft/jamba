#include "NormalizedState.h"
#include "Parameters.h"

#include <sstream>

namespace pongasoft {
namespace VST {

//------------------------------------------------------------------------
// NormalizedState::NormalizedState
//------------------------------------------------------------------------
NormalizedState::NormalizedState(SaveOrder const *iSaveOrder) : fSaveOrder{iSaveOrder}
{
  if(getCount() > 0)
  {
    // () initializes the element to their default (0.0)
    fValues = new ParamValue[getCount()]();
  }
}

//------------------------------------------------------------------------
// NormalizedState::~NormalizedState
//------------------------------------------------------------------------
NormalizedState::~NormalizedState()
{
  delete[] fValues; // ok to delete nullptr
}

//------------------------------------------------------------------------
// NormalizedState::operator=
//------------------------------------------------------------------------
NormalizedState &NormalizedState::operator=(const NormalizedState &other)
{
  if(&other == this)
    return *this;

  // should not happen but sanity check!
  if(fSaveOrder != other.fSaveOrder)
    ABORT_F("no memory allocation allowed => aborting");
  else
  {
    std::copy(other.fValues, other.fValues + getCount(), fValues);
  }

  return *this;
}

//------------------------------------------------------------------------
// NormalizedState::readFromStream
//------------------------------------------------------------------------
tresult NormalizedState::readFromStream(const Parameters *iParameters, IBStreamer &iStreamer)
{
  // Skipping version reading on purpose: needs to be read before in order to handle upgrade/multiple versions

  for(int i = 0; i < getCount(); i++)
  {
    auto paramID = fSaveOrder->fOrder[i];
    fValues[i] = iParameters->readNormalizedValue(paramID, iStreamer);
  }

  return kResultOk;
}

//------------------------------------------------------------------------
// NormalizedState::writeToStream
//------------------------------------------------------------------------
tresult NormalizedState::writeToStream(Parameters const * /* iParameters */, IBStreamer &oStreamer) const
{
  // write version for later upgrade
  oStreamer.writeInt16u(fSaveOrder->fVersion);

  for(int i = 0; i < getCount(); i ++)
  {
    oStreamer.writeDouble(fValues[i]);
  }

  return kResultOk;
}

//------------------------------------------------------------------------
// NormalizedState::toString -- only for debug
//------------------------------------------------------------------------
std::string NormalizedState::toString() const
{
  std::ostringstream s;
  s << "{v=" << fSaveOrder->fVersion;
  for(int i = 0; i < fSaveOrder->getCount(); i++)
  {
    s << ", " << fSaveOrder->fOrder[i] << "=" << fValues[i];
  }
  s << "}";
  return s.str();
}



}
}