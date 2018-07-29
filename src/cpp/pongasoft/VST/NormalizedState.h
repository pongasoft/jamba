#pragma once

#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>
#include <pongasoft/logging/logging.h>

#include <string>
#include <vector>

namespace pongasoft {
namespace VST {

using namespace Steinberg::Vst;
using namespace Steinberg;

class Parameters;

/**
 * Used to communicate the state between the UI and the RT and read/write to stream
 */
class NormalizedState
{
public:
  /**
   * Maintains the order used to save/restore the RT and GUI state
   */
  struct SaveOrder
  {
    uint16 fVersion{0};
    std::vector<ParamID> fOrder{};
    inline int getCount() const { return static_cast<int>(fOrder.size()); }
  };

  // Constructor
  explicit NormalizedState(SaveOrder const *iSaveOrder);

  // Destructor
  ~NormalizedState();

  // No copy constructor allowed
  NormalizedState(NormalizedState const &other) = delete;

  // Copies the content of the array ONLY when same SaveOrder (which is the case internally)
  NormalizedState& operator=(NormalizedState const &other);

  // getCount
  inline int getCount() const { return fSaveOrder->getCount(); }

  // set the param value (check for index bounds)
  inline void set(int iIdx, ParamValue iParamValue)
  {
    DCHECK_F(iIdx >= 0 && iIdx < fSaveOrder->getCount());
    fValues[iIdx] = iParamValue;
  }

  // readFromStream
  virtual tresult readFromStream(Parameters const *iParameters, IBStreamer &iStreamer);

  // writeToStream
  virtual tresult writeToStream(Parameters const *iParameters, IBStreamer &oStreamer) const;

  // toString
  std::string toString() const;


public:
  SaveOrder const *fSaveOrder;
  ParamValue *fValues{nullptr};
};


}
}

