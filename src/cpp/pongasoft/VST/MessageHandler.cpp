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

#include <pongasoft/logging/logging.h>

#include "MessageHandler.h"

namespace pongasoft {
namespace VST {

//------------------------------------------------------------------------
// MessageHandler::handleMessage
//------------------------------------------------------------------------
tresult MessageHandler::handleMessage(Message const &iMessage)
{
  auto iter = fHandlers.find(iMessage.getMessageID());

  if(iter == fHandlers.cend())
    return kResultFalse;

  return iter->second->handleMessage(iMessage);
}

//------------------------------------------------------------------------
// MessageHandler::registerHandler
//------------------------------------------------------------------------
void MessageHandler::registerHandler(MessageID iMessageID, IMessageHandler *iMessageHandler)
{
  DCHECK_F(iMessageHandler != nullptr);

  auto iter = fHandlers.find(iMessageID);

  if(iter != fHandlers.cend())
  {
    DLOG_F(WARNING, "registering message handler for [%d] multiple time", iMessageID);
  }

  fHandlers[iMessageID] = iMessageHandler;
}
}
}