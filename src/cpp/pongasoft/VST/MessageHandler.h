/*
 * Copyright (c) 2018 pongasoft
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

#include "Messaging.h"

#include <map>

namespace pongasoft {
namespace VST {

/**
 * Interface defining a message handler */
class IMessageHandler
{
public:
  virtual ~IMessageHandler() = default;
  virtual tresult handleMessage(Message const &iMessage) = 0;
};

/**
 * Simple implementation of IMessageHandler which will delegate the message handling based on MessageID */
class MessageHandler : public IMessageHandler
{
public:
  // handleMessage
  tresult handleMessage(Message const &iMessage) override;

  // registerHandler
  void registerHandler(MessageID iMessageID, IMessageHandler *iMessageHandler);

private:
  // map of messageID to a handler
  std::map<MessageID, IMessageHandler *> fHandlers{};
};

}
}

