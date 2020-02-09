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
#include <pongasoft/VST/GUI/Views/SelfContainedViewListener.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

/**
 * MyListener
 */
struct MyViewListener : public SelfContainedViewListener
{
  explicit MyViewListener(int iValue) : fValue(iValue)
  {
    instanceCounter++;
  }

protected:
  void viewTookFocus(CView *view) override
  {
    fValue += 10;
  }

  void viewWillDelete(CView *iView) override
  {
    fValue *= 20;
    SelfContainedViewListener::viewWillDelete(iView);
  }

public:

  ~MyViewListener() override
  {
    instanceCounter--;
  }

  long getUseCount() const
  {
    return fThis ? fThis.use_count() : 0;
  }

  int fValue;

  static std::atomic<int> instanceCounter;
};

std::atomic<int> MyViewListener::instanceCounter{0};

/**
 * CView destructor is protected => subclassing
 */
struct MyView : public CView
{
  MyView() : CView{CRect{0,0,100,100}} {}
  ~MyView() override = default;
};

// SelfContainedViewListener - testScenario1
TEST(SelfContainedViewListener, testScenario1) {
  MyView view{};
  MyViewListener *listener{nullptr};

  // making sure there is no instances
  ASSERT_EQ(0, MyViewListener::instanceCounter.load());

  {
    // create an instance of the listener and keep a reference to it via the shared pointer
    auto sptr = SelfContainedViewListener::create<MyViewListener>(&view, 100);

    // we also keep a reference to the pointer for when we exit this block
    listener = sptr.get();

    ASSERT_EQ(1, MyViewListener::instanceCounter.load()); // instance created
    ASSERT_EQ(2, sptr.use_count()); // 2 shared pointers (sptr + MyViewListener::fThis)
    ASSERT_EQ(100, sptr->fValue); // constructor argument was properly propagated

    // make sure that the listener was registered properly
    view.takeFocus();
    ASSERT_EQ(110, sptr->fValue); // viewTookFocus increases fValue by 10
  }

  // after exiting the block, sptr is gone, but because of MyViewListener::fThis, the listener still exists

  ASSERT_EQ(1, MyViewListener::instanceCounter.load()); // instance still exists
  ASSERT_EQ(1, listener->getUseCount()); // only MyViewListener::fThis reference

  // this removes the listener which ends up destroying the instance
  view.beforeDelete();

  ASSERT_EQ(0, MyViewListener::instanceCounter.load()); // instance destroyed
}

// SelfContainedViewListener - testScenario2
TEST(SelfContainedViewListener, testScenario2) {
  MyView view{};

  // making sure there is no instances
  ASSERT_EQ(0, MyViewListener::instanceCounter.load());

  {
    // create an instance of the listener and keep a reference to it via the shared pointer
    auto sptr = SelfContainedViewListener::create<MyViewListener>(&view, 100);

    ASSERT_EQ(1, MyViewListener::instanceCounter.load()); // instance created
    ASSERT_EQ(2, sptr.use_count()); // 2 shared pointers (sptr + MyViewListener::fThis)
    ASSERT_EQ(100, sptr->fValue); // constructor argument was properly propagated

    // make sure that the listener was registered properly
    view.takeFocus();
    ASSERT_EQ(110, sptr->fValue); // viewTookFocus increases fValue by 10

    // now we prematurely removes the listener
    sptr->unregister();
    ASSERT_EQ(1, MyViewListener::instanceCounter.load()); // instance still exists (because of sptr)
    ASSERT_EQ(1, sptr.use_count()); // only sptr reference (MyViewListener::fThis) was deleted

    view.takeFocus();
    ASSERT_EQ(110, sptr->fValue); // listener has been removed => callback NOT called
  }

  // sptr is gone => instance is gone
  ASSERT_EQ(0, MyViewListener::instanceCounter.load()); // instance destroyed
}

}
}
}
}