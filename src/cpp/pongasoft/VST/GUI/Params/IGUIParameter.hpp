/*
 * Copyright (c) 2019 pongasoft
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

#include "GUIVstParameter.h"

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {

template<typename T>
std::shared_ptr<ITGUIParameter<T>> IGUIParameter::cast()
{
  auto sft = shared_from_this();

  auto rawPtr = std::dynamic_pointer_cast<GUIRawVstParameter>(sft);

  if(rawPtr)
  {
    auto vstPtr = rawPtr->asVstParameter<T>();
    if(vstPtr)
      return vstPtr;
  }

  return std::dynamic_pointer_cast<ITGUIParameter<T>>(sft);
}

}
}
}
}