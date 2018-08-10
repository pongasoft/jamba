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

#pragma once

#include <memory>
#include <pluginterfaces/vst/ivstmessage.h>

namespace pongasoft {
namespace VST {

using namespace Steinberg;
using namespace Steinberg::Vst;

/**
 * Abstraction for allocating and sending a message
 */
class IMessageProducer
{
public:
  virtual ~IMessageProducer() = default;

  /** Allocates a message instance */
  virtual IPtr<IMessage> allocateMessage() = 0;

  /** Sends the given message to the peer. */
  virtual tresult sendMessage(IPtr<IMessage> iMessage) = 0;
};

}
}