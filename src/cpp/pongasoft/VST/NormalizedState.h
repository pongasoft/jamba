/*
 * Copyright (c) 2018-2020 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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

#include <pluginterfaces/vst/vsttypes.h>
#include <base/source/fstreamer.h>
#include <pongasoft/logging/logging.h>

#include <string>
#include <vector>

namespace pongasoft::VST {

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
    int16 fVersion{0};
    std::vector<ParamID> fOrder{};
    inline int getCount() const { return static_cast<int>(fOrder.size()); }
  };

  // Constructor
  explicit NormalizedState(SaveOrder const *iSaveOrder);

  // Destructor
  ~NormalizedState();

  // Copy constructor
  NormalizedState(NormalizedState const &other);

  // Copies the content of the array ONLY when same SaveOrder (which is the case internally)
  NormalizedState& operator=(NormalizedState const &other);

  /**
   * Copy values from `this` to the destination (`oDestination`) state. This call will only copies the values that
   * `oDestination` state can handle.
   *
   * @param oDestination the destination to copy the values to
   * @return the number of values actually copied */
  int copyValuesTo(NormalizedState &oDestination) const;

  /**
   * Copy values from the source (`iSource`) state to `this` state. This call will only copies the values that `this`
   * state can handle.
   *
   * @param iSource the source to copy the values from
   * @return the number of values actually copied */
  inline int copyValuesFrom(NormalizedState const &iSource) { return iSource.copyValuesTo(*this); }

  // getCount
  inline int getCount() const { return fSaveOrder->getCount(); }

  // getVersion
  inline int16 getVersion() const { return fSaveOrder->fVersion; }

  /**
   * The index for the given param
   *
   * @return the index or `-1` if not found */
  int findParamIndex(ParamID iParamID) const;

  //! Sets the param value
  inline void set(int iIdx, ParamValue iParamValue)
  {
    DCHECK_F(iIdx >= 0 && iIdx < fSaveOrder->getCount());
    fValues[iIdx] = iParamValue;
  }

  //! Gets the param value
  inline ParamValue get(int iIdx) const
  {
    DCHECK_F(iIdx >= 0 && iIdx < fSaveOrder->getCount());
    return fValues[iIdx];
  }

  // readFromStream
  virtual tresult readFromStream(Parameters const *iParameters, IBStreamer &iStreamer);

  // writeToStream
  virtual tresult writeToStream(Parameters const *iParameters, IBStreamer &oStreamer) const;

  /**
   * Returns the normalized value for the given param id if it exists.
   *
   * @return `kResultTrue` if the value existed and `oValue` is populated, `kResultFalse` otherwise */
  tresult getNormalizedValue(ParamID iParamID, ParamValue &oValue) const;

  /**
   * Sets the normalized value for the given param id if it exists.
   *
   * @return `kResultTrue` if the param existed, `kResultFalse` otherwise */
  tresult setNormalizedValue(ParamID iParamID, ParamValue iValue);


  // toString
  std::string toString() const;

public:
  SaveOrder const *fSaveOrder;
  ParamValue *fValues{nullptr};
};


}

