#include "NormalizedState.h"

#include <sstream>

namespace pongasoft {
namespace VST {

//------------------------------------------------------------------------
// NormalizedState::NormalizedState
//------------------------------------------------------------------------
NormalizedState::NormalizedState(int iCount)
{
  DCHECK_F(iCount >= 0);
  if(iCount > 0)
  {
    fCount = iCount;
    fValues = new ParamValue[fCount];

    for(int i = 0; i < fCount; i++)
    {
      fValues[i] = 0.0;
    }
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
// NormalizedState::NormalizedState(&&) - Move constructor
//------------------------------------------------------------------------
NormalizedState::NormalizedState(NormalizedState &&other) noexcept
{
  fCount = other.fCount;
  fValues = other.fValues;

  other.fCount = 0;
  other.fValues = nullptr;
}

//------------------------------------------------------------------------
// NormalizedState::operator=
//------------------------------------------------------------------------
NormalizedState &NormalizedState::operator=(const NormalizedState &other)
{
  if(&other == this)
    return *this;

  // should not happen but sanity check!
  if(fCount != other.fCount)
    ABORT_F("no memory allocation allowed => aborting");
  else
  {
    std::copy(other.fValues, other.fValues + fCount, fValues);
  }

  return *this;
}

//------------------------------------------------------------------------
// NormalizedState::toString -- only for debug
//------------------------------------------------------------------------
std::string NormalizedState::toString(ParamID const *iParamIDs) const
{
  std::ostringstream s;
  s << "{";
  for(int i = 0; i < fCount; i++)
  {
    if(i > 0)
      s << ", ";
    s << iParamIDs[i] << "=" << fValues[i];
  }
  s << "}";
  return s.str();
}


}
}