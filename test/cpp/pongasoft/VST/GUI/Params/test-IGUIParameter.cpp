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

#include <gtest/gtest.h>
#include <pongasoft/VST/GUI/Params/IGUIParameter.hpp>
#include <pongasoft/VST/GUI/Params/GUIValParameter.h>
#include <memory>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Params {
namespace TestIGUIParameter {

// IGUIParameter - testCast
TEST(IGUIParameter, testCast) {

  auto ref = std::make_shared<GUIValParameter<bool>>(1, false);

  std::shared_ptr<IGUIParameter> ptr = ref;

  ASSERT_EQ(1, ptr->getParamID());

  auto pb = ptr->cast<bool>();

  // cast should work!
  ASSERT_TRUE(pb.get() != nullptr);
  ASSERT_EQ(1, pb->getParamID());
  ASSERT_EQ(false, pb->getValue());

  // making sure that we get a pointer to the SAME object (ref)
  pb->setValue(true);
  ASSERT_EQ(true, pb->getValue());
  ASSERT_EQ(true, ref->getValue());
  ASSERT_EQ(ref.get(), pb.get()); // clearly shows it is the same pointer

  auto pi = ptr->cast<int>();
  // cast should NOT work
  ASSERT_TRUE(pi.get() == nullptr);
}

}
}
}
}
}
