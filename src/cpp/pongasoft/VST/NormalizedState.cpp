/*
 * Copyright (c) 2018 pongasoft
 *
 * Licensed under the General Public License (GPL) Version 3; you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */
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
  if(fSaveOrder->fVersion >= 0)
    oStreamer.writeInt16u(static_cast<uint16>(fSaveOrder->fVersion));

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