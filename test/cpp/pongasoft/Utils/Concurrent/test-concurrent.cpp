#include <pongasoft/Utils/Concurrent/Concurrent.h>
#include <gtest/gtest.h>
#include <thread>

namespace pongasoft {
namespace VST {
namespace Test {

using namespace pongasoft::Utils::Concurrent::WithSpinLock;

struct MyTestValue
{
  explicit MyTestValue(int iValue = 0) : fValue(iValue), fOperatorEqualCounter{0}
  {
    instanceCounter++;
//    std::cout << "MyTestValue() = " << instanceCounter.load() << std::endl;
  }

  MyTestValue(MyTestValue const &other) : fValue{other.fValue}, fOperatorEqualCounter{other.fOperatorEqualCounter}
  {
    instanceCounter++;
//    std::cout << "MyTestValue(cc) = " << instanceCounter.load() << std::endl;
  }

  void operator=(MyTestValue const &other)
  {
    fValue = other.fValue;
    fOperatorEqualCounter++;
    //std::cout << "MyTestValue::operator= = " << operatorEqualCounter << std::endl;
  }

  ~MyTestValue()
  {
    instanceCounter--;
//    std::cout << "~MyTestValue = " << instanceCounter.load() << std::endl;
  }

  int fValue;
  // this is on purpose not a thread safe counter to test that the ++ operation happens in a thread safe manner
  int fOperatorEqualCounter{0};

  static std::atomic<int> instanceCounter;
};

std::atomic<int> MyTestValue::instanceCounter{0};

///////////////////////////////////////////
// AtomicValue tests
///////////////////////////////////////////

// AtomicValueTest - SingleThreadCorrectBehavior
TEST(AtomicValueTest, SingleThreadCorrectBehavior)
{
  {
    AtomicValue<MyTestValue> value{MyTestValue{3}};

    // AtomicValue.fValue
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    // value should be 3
    ASSERT_EQ(3, value.get().fValue);

    // AtomicValue.fValue
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    // calling again should return the same value!
    ASSERT_EQ(3, value.get().fValue);

    // AtomicValue.fValue
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    // setting 4
    value.set(MyTestValue{4});

    // AtomicValue.fValue
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    // value should now be 4
    ASSERT_EQ(4, value.get().fValue);

    // AtomicValue.fValue
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());
  }

  ASSERT_EQ(0, MyTestValue::instanceCounter.load());
}

// AtomicValueTest - MultiThreadSafe
TEST(AtomicValueTest, MultiThreadSafe)
{
  // sanity check to make sure the test is not impacted!
  ASSERT_TRUE(MyTestValue::instanceCounter.is_lock_free());

  constexpr int N = 10000;

  int gets[N];
  int sets[N];

  for(int i = 0; i < N; i++)
  {
    gets[i] = 0;
    sets[i] = 0;
  }

  {
    AtomicValue<MyTestValue> value{MyTestValue{}};

    auto ui = [&] {
      for(int i = 0; i < N; i++)
        gets[value.get().fValue]++;
    };

    auto processing = [&] {
      for(int i = 0; i < N; i++)
      {
        value.set(MyTestValue{i});
        sets[i]++;
      }
    };

    std::thread uiThread(ui);
    std::thread processingThread(processing);

    uiThread.join();
    processingThread.join();

    for(int i = 0; i < N; i++)
    {
      ASSERT_EQ(1, sets[i]);
    }
  }

  ASSERT_EQ(0, MyTestValue::instanceCounter.load());

}

// AtomicValueTest - MultiThreadSafe2
TEST(AtomicValueTest, MultiThreadSafe2)
{
  // sanity check to make sure the test is not impacted!
  ASSERT_TRUE(MyTestValue::instanceCounter.is_lock_free());

  // the purpose of this test is to bang on the AtomicValue using many threads on the get and set side and make sure
  // that accessing the value is properly protected via lock => the fOperatorEqualCounter is incremented by the
  // operator= which should take place under lock
  constexpr int N = 1000;
  constexpr int M = 10;

  {
    AtomicValue<MyTestValue> value{MyTestValue{}};

    ASSERT_EQ(0, value.get().fOperatorEqualCounter);
    value.set(MyTestValue{3});
    ASSERT_EQ(1, value.get().fOperatorEqualCounter);
    value.set(MyTestValue{9});
    ASSERT_EQ(2, value.get().fOperatorEqualCounter);

    auto ui = [&] {
      for(int i = 0; i < N; i++)
        value.get();
    };

    auto processing = [&] {
      for(int i = 0; i < N; i++)
      {
        value.set(MyTestValue{i});
      }
    };

    auto processingThreads = new std::thread *[M];
    auto uiThreads = new std::thread *[M];

    for(int i = 0; i < M; i++)
    {
      processingThreads[i] = new std::thread(processing);
      uiThreads[i] = new std::thread(ui);
    }


    for(int i = 0; i < M; i++)
    {
      processingThreads[i]->join();
      delete processingThreads[i];
      uiThreads[i]->join();
      delete uiThreads[i];
    }

    delete[] uiThreads;
    delete[] processingThreads;

    ASSERT_EQ(N * M + 2, value.get().fOperatorEqualCounter);
  }

  ASSERT_EQ(0, MyTestValue::instanceCounter.load());

}

/**
 * the purpose of this test is to bang on the AtomicValue using many threads on the get and set side and make sure
 * that the state is always consistent meaning get and set are actually atomic.
 */
// AtomicValueTest - Atomic
TEST(AtomicValueTest, Atomic)
{
  constexpr int SIZE = 2000;

  struct State
  {
    int fValue{0};
    double fArray[SIZE]{};

    void init(int iValue)
    {
      fValue = iValue;
      for(int i = 0; i < SIZE; i++)
        fArray[i] = 1.0 / (fValue + 1.0);
    }

    void check()
    {
      for(int i = 0; i < SIZE; i++)
        ASSERT_EQ(fArray[i], 1.0 / (fValue + 1.0));
    }
  };

  constexpr int N = 1000;
  constexpr int M = 10;

  std::atomic<int> stateChecked[N];
  for(int i = 0; i < N; i++)
    stateChecked[i].store(0);

  State state;
  state.init(0);

  AtomicValue<State> atomicState{state};

  auto ui = [&] {
    for(int i = 0; i < N; i++)
    {
      auto s = atomicState.get();
      s.check();
      stateChecked[i]++;
    }

  };

  auto processing = [&] {
    for(int i = 0; i < N; i++)
    {
      State s;
      s.init(i);
      atomicState.set(s);
    }
  };

  auto processingThreads = new std::thread *[M];
  auto uiThreads = new std::thread *[M];

  for(int i = 0; i < M; i++)
  {
    processingThreads[i] = new std::thread(processing);
    uiThreads[i] = new std::thread(ui);
  }


  for(int i = 0; i < M; i++)
  {
    processingThreads[i]->join();
    delete processingThreads[i];
    uiThreads[i]->join();
    delete uiThreads[i];
  }

  delete[] uiThreads;
  delete[] processingThreads;

  int count = 0;
  for(int i = 0; i < N; i++)
    count += stateChecked[i].load();

  ASSERT_EQ(N*M, count);

}

///////////////////////////////////////////
// SingleElementQueue tests
///////////////////////////////////////////
// SingleElementQueueTest - SingleThreadCorrectBehavior
TEST(SingleElementQueueTest, SingleThreadCorrectBehavior)
{
  {
    SingleElementQueue<MyTestValue> queue{};

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    {
      MyTestValue v{3};
      ASSERT_EQ(false, queue.pop(v));
      // we make sure that when pop returns false, the original value is left untouched
      ASSERT_EQ(3, v.fValue);
    }

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    queue.push(MyTestValue{4});

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    {
      MyTestValue v{};
      ASSERT_EQ(true, queue.pop(v));

      // SingleElementQueue.fSingleElement + v
      ASSERT_EQ(2, MyTestValue::instanceCounter.load());

      ASSERT_EQ(4, v.fValue);
      ASSERT_EQ(false, queue.pop(v));

      // SingleElementQueue.fSingleElement + v
      ASSERT_EQ(2, MyTestValue::instanceCounter.load());
    }

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    queue.push(MyTestValue{5});

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    queue.push(MyTestValue{6});

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());

    {
      MyTestValue v{};
      ASSERT_EQ(true, queue.pop(v));
      // SingleElementQueue.fSingleElement + v
      ASSERT_EQ(2, MyTestValue::instanceCounter.load());

      ASSERT_EQ(6, v.fValue);
      ASSERT_EQ(false, queue.pop(v));

      // SingleElementQueue.fSingleElement + v
      ASSERT_EQ(2, MyTestValue::instanceCounter.load());
    }

    // SingleElementQueue.fSingleElement
    ASSERT_EQ(1, MyTestValue::instanceCounter.load());
  }

  ASSERT_EQ(0, MyTestValue::instanceCounter.load());
}

// SingleElementQueueTest - MultiThreadSafe
TEST(SingleElementQueueTest, MultiThreadSafe)
{
  // sanity check to make sure the test is not impacted!
  ASSERT_TRUE(MyTestValue::instanceCounter.is_lock_free());

  constexpr int N = 100000;

  int pushs[N];
  int pops[N];

  for(int i = 0; i < N; i++)
  {
    pushs[i] = 0;
    pops[i] = 0;
  }

  {
    SingleElementQueue<MyTestValue> queue{};

    auto ui = [&] {
      for(int i = 0; i < N; i++)
      {
        queue.push(MyTestValue{i});
        pushs[i]++;
      }
    };

    auto processing = [&] {
      for(int i = 0; i < N; i++)
      {
        MyTestValue v{};
        if(queue.pop(v))
        {
          pops[v.fValue]++;
        }
      }
    };

    std::thread uiThread(ui);
    std::thread processingThread(processing);

    uiThread.join();
    processingThread.join();

    for(int i = 0; i < N; i++)
    {
      ASSERT_EQ(1, pushs[i]);
      ASSERT_TRUE(pops[i] == 0 || pops[i] == 1);

      if(pops[i] > 2)
        std::cout << "pops > 1 [" << i << "] = " << pops[i] << std::endl;
    }


    if(pops[N - 1] == 0)
    {
      MyTestValue v{};
      ASSERT_TRUE(queue.pop(v));
      ASSERT_EQ(N - 1, v.fValue);
    }
    else
    {
      MyTestValue v{};
      ASSERT_FALSE(queue.pop(v));
    }
  }

  ASSERT_EQ(0, MyTestValue::instanceCounter.load());

}

/**
 * the purpose of this test is to bang on the SingleElementQueueTest using many threads on the push and pop side and make sure
 * that the state is always consistent meaning get and set are actually atomic.
 */
// AtomicValueTest - Atomic
TEST(SingleElementQueueTest, Atomic)
{
  constexpr int SIZE = 2000;

  struct State
  {
    int fValue{0};
    double fArray[SIZE]{};

    void init(int iValue)
    {
      fValue = iValue;
      for(int i = 0; i < SIZE; i++)
        fArray[i] = 1.0 / (fValue + 1.0);
    }

    void check()
    {
      for(int i = 0; i < SIZE; i++)
        ASSERT_EQ(fArray[i], 1.0 / (fValue + 1.0));
    }
  };

  constexpr int N = 1000;
  constexpr int M = 10;

  std::atomic<int> stateChecked[N];
  for(int i = 0; i < N; i++)
    stateChecked[i].store(0);

  State state;
  state.init(0);

  SingleElementQueue<State> queue{};

  auto ui = [&] {
    for(int i = 0; i < N; i++)
    {
      State s;
      if(queue.pop(s))
      {
        s.check();
        stateChecked[i]++;
      }
    }

  };

  auto processing = [&] {
    for(int i = 0; i < N; i++)
    {
      State s;
      s.init(i);
      queue.push(s);
    }
  };

  auto processingThreads = new std::thread *[M];
  auto uiThreads = new std::thread *[M];

  for(int i = 0; i < M; i++)
  {
    processingThreads[i] = new std::thread(processing);
    uiThreads[i] = new std::thread(ui);
  }


  for(int i = 0; i < M; i++)
  {
    processingThreads[i]->join();
    delete processingThreads[i];
    uiThreads[i]->join();
    delete uiThreads[i];
  }

  delete[] uiThreads;
  delete[] processingThreads;

  int count = 0;

  // it is possible that there is still one element in the queue
  State lastOne;
  if(queue.pop(lastOne))
  {
    lastOne.check();
    count++;
  }
  
  // because push replaces the previous value, it is impossible to know how many values were actually checked

  for(int i = 0; i < N; i++)
  {
    int checked = stateChecked[i].load();
    count += checked;
    ASSERT_TRUE(checked <= N);
    
  }

  ASSERT_TRUE(count <= N*M);

}

}
}
}

