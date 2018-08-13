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

#include "GUIJmbParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

//------------------------------------------------------------------------
// IGUIJmbParameter::broadcast
//------------------------------------------------------------------------
tresult IGUIJmbParameter::broadcast() const
{
  if(!fMessageProducer)
  {
    DLOG_F(WARNING, "broadcast ignored: no message producer assigned");
    return kResultFalse;
  }

  if(!getParamDef()->fShared)
  {
    DLOG_F(WARNING, "broadcast ignored: parameter [%d] is not marked shared", getParamID());
    return kResultFalse;
  }

  tresult res = kResultOk;

  auto message = fMessageProducer->allocateMessage();

  if(message)
  {
    Message m{message.get()};

    // sets the message ID
    m.setMessageID(getParamID());

    // serialize the content
    if(writeToMessage(m) == kResultOk)
      res |= fMessageProducer->sendMessage(message);
    else
      res = kResultFalse;
  }
  else
    res = kResultFalse;

  return res;
}

}
}
}
}