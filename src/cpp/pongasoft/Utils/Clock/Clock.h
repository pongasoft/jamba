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

#include <chrono>

namespace pongasoft {
namespace VST {
namespace Clock {

//------------------------------------------------------------------------
// getCurrentTimeMillis()
//------------------------------------------------------------------------
inline long getCurrentTimeMillis()
{
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

}
}
}