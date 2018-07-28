#pragma once

#include <pluginterfaces/vst/vsttypes.h>
#include <pongasoft/logging/logging.h>

#include <string>

namespace pongasoft {
namespace VST {

using namespace Steinberg::Vst;

// Internal structure used to transfer the state between the GUI thread and the RT processing
// (readNewState/writeLatestState)
struct NormalizedState
{
  explicit NormalizedState(int iCount);

  // Destructor
  ~NormalizedState();

  // No copy constructor allowed (use move constructor instead)
  NormalizedState(NormalizedState const &other) = delete;

  // Move constructor => transfer the memory to this object and cleans the other (no memory allocation!)
  NormalizedState(NormalizedState &&other) noexcept;

  // Copies the content of the array ONLY when size match (which is the case internally)
  NormalizedState& operator=(NormalizedState const &other);

  // set the param value (check for index bounds)
  inline void set(int iIdx, ParamValue iParamValue)
  {
    DCHECK_F(iIdx >= 0 && iIdx < fCount);
    fValues[iIdx] = iParamValue;
  }

  // toString
  std::string toString(ParamID const *iParamIDs) const;

  ParamValue *fValues{nullptr};
  int fCount{0};
};


}
}

