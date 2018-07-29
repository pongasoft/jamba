#ifndef __PONGASOFT_UTILS_CONCURRENT_CONCURRENT_H__
#define __PONGASOFT_UTILS_CONCURRENT_CONCURRENT_H__

#include <atomic>
#include <memory>

namespace pongasoft {
namespace Utils {
namespace Concurrent {

/*
 * One of the big issue in the VST world comes from the fact that the processing thread maintains the state but it
 * is being accessed/changed by the UI thread (AudioEffect::setState and AudioEffect::getState). Moreover if the
 * processing thread needs to send a message to the UI thread this must happen in a timer (running in the UI thread)
 * meaning the processing thread needs to have a way to communicate the content of the message to the timer in a
 * thread safe way. See thread https://sdk.steinberg.net/viewtopic.php?f=4&t=516 for discussion.
 *
 * The 2 primitives required are an atomic value (for getState) and a queue with one element (where the element in the
 * queue can be updated if not popped yet) (for setState / timer message)
 *
 * One of the golden rules of real time audio programming is not to use locks. Implementing those primitives without
 * using any locks requires memory allocation which is also another no-no for real time audio programming which then
 * constrains memory allocation to the UI thread only. This makes the code very challenging to write in a thread
 * safe fashion and no solution is provided part of the SDK.
 *
 * As a trade-off, I decided to go for a much simpler solution which does use a very lightweight lock: a user space
 * spin lock. The implementation is not allocating any memory in any thread and is relying on the std::atomic_flag
 * concept which is guaranteed to be lock free. It also does not make any system calls.
 *
 * SingleElementQueue and AtomicValue both use a spin lock and keep the lock only for the duration of copying
 * the Element/Value that each concept encapsulates (of type T). So the worst case scenario from the real time
 * processing should be to wait for the other thread to copy T which seems like a pretty fair trade-off as it is quite
 * constrained (of course the real time processing could theoretically "spin" for a long time but in practice the
 * UI thread is not "banging" on the lock to create this scenario).
 */

/**
 * A simple implementation of a spin lock using the std::atomic_flag which is guaranteed to be atomic and lock free.
 * The usage is the following:
 *
 * auto lock = spinLock.acquire();
 * ...
 *
 * the lock is released automatically when it exits the scope.
 */

class SpinLock
{
public:

  class Lock
  {
  public:
    /**
     * This will automatically release the lock
     */
    inline ~Lock()
    {
      if(fSpinLock != nullptr)
        fSpinLock->unlock();
    }

    inline Lock(Lock &&iLock) noexcept : fSpinLock{iLock.fSpinLock}
    {
      iLock.fSpinLock = nullptr;
    }

    Lock(Lock const &) = delete;
    Lock& operator=(Lock const &) = delete;

  private:
    friend class SpinLock;

    explicit Lock(SpinLock *iSpinLock) : fSpinLock{iSpinLock}
    {
    }


    SpinLock *fSpinLock;
  };

  SpinLock() : fFlag{false}
  {
  }

  /**
   * @return the lock that will be released when it goes out of scope
   */
  inline Lock acquire()
  {
    while(fFlag.test_and_set(std::memory_order_acquire))
    {
      // nothing to do => spin
    }

    return Lock(this);
  }


  SpinLock(SpinLock const &) = delete;

  SpinLock &operator=(SpinLock const &) = delete;

private:
  friend class Lock;

  inline void unlock()
  {
    fFlag.clear(std::memory_order_release);
  }

  std::atomic_flag fFlag;
};

/**
 * The purpose of this namespace is to emphasize the fact that the implementation is using a spinlock
 */
namespace WithSpinLock {

/**
 * This class implements a queue which has at most 1 element (0 or 1). If there is already an element and the
 * push method is called again, it will replace the current element. This implementation uses a SpinLock and is
 * thread safe to be called by any number of threads. This implementation does not allocate any memory and is expecting
 * the T type to have an empty constructor and operator=.
 */
template<typename T>
class SingleElementQueue
{
public:
  SingleElementQueue() : fSingleElement{std::make_unique<T>()}, fIsEmpty{true}, fSpinLock{}
  {}

  /**
   * This constructor can be used to add one element to the queue right away or when there is no empty constructor
   * (required by the no argument constructor).
   */
  explicit SingleElementQueue(std::unique_ptr<T> iFirstElement,
                              bool iIsEmpty = false) :
    fSingleElement{std::move(iFirstElement)},
    fIsEmpty{iIsEmpty},
    fSpinLock{}
  {}

  /**
   * Returns the single element in the queue if there is one
   *
   * @param oElement this will be populated with the value of the element in the queue or left untouched if there
   *                 isn't one
   * @return true if there was one element in the queue, false otherwise
   */
  bool pop(T &oElement)
  {
    auto lock = fSpinLock.acquire();
    if(fIsEmpty)
      return false;

    oElement = *fSingleElement;
    fIsEmpty = true;

    return true;
  };

  /**
   * Returns the single element in the queue if there is one
   *
   * @param oElement this will be populated with the value of the element in the queue or left untouched if there
   *                 isn't one
   * @return true if there was one element in the queue, false otherwise
   */
  bool pop(T *oElement)
  {
    auto lock = fSpinLock.acquire();
    if(fIsEmpty)
      return false;

    *oElement = *fSingleElement;
    fIsEmpty = true;

    return true;
  };


  /**
   * Pushes one element in the queue. If the queue already had an element it will be replaced.
   * @param iElement the element to push (clearly not modified by the call)
   */
  void push(T const &iElement)
  {
    auto lock = fSpinLock.acquire();
    *fSingleElement = iElement;
    fIsEmpty = false;
  }

  /**
   * Pushes one element in the queue. If the queue already had an element it will be replaced.
   * @param iElement the element to push (clearly not modified by the call)
   */
  void push(T const *iElement)
  {
    auto lock = fSpinLock.acquire();
    *fSingleElement = *iElement;
    fIsEmpty = false;
  }

private:
  std::unique_ptr<T> fSingleElement;
  bool fIsEmpty;
  SpinLock fSpinLock;
};

/**
 * This class encapsulates a single atomic value. This implementation uses a SpinLock and is
 * thread safe to be called by any number of threads. This implementation does not allocate any memory and is
 * expecting the T type to have an empty constructor, copy constructor and operator=.
 */
template<typename T>
class AtomicValue
{
public:
  explicit AtomicValue(std::unique_ptr<T> iValue) : fValue{std::move(iValue)}, fSpinLock{} {}

  explicit AtomicValue(T const &iValue) : fValue{std::make_unique<T>(iValue)}, fSpinLock{} {}

  /**
   * Returns the "current" value. Note that this method should be called by one thread at a time (it is ok to call
   * "set" at the same time with another thread). The return value is copied in the oElement (copy vs creation).
   */
  T get()
  {
    auto lock = fSpinLock.acquire();
    return *fValue;
  };

  /**
 * Returns the "current" value. Note that this method should be called by one thread at a time (it is ok to call
 * "set" at the same time with another thread). The return value is copied in the oElement (copy vs creation).
 */
  void get(T &oElement)
  {
    auto lock = fSpinLock.acquire();
    oElement = *fValue;
  };

  /**
   * Returns the "current" value. Note that this method should be called by one thread at a time (it is ok to call
   * "set" at the same time with another thread). The return value is copied in the oElement (copy vs creation).
   */
  void get(T *oElement)
  {
    auto lock = fSpinLock.acquire();
    *oElement = *fValue;
  };

  /**
   * Updates the current value with the provided one.
   */
  void set(T const &iValue)
  {
    auto lock = fSpinLock.acquire();
    *fValue = iValue;
  }

  /**
   * Updates the current value with the provided one.
   */
  void set(T const *iValue)
  {
    auto lock = fSpinLock.acquire();
    *fValue = *iValue;
  }

private:
  std::unique_ptr<T> fValue;
  SpinLock fSpinLock;
};


}
}
}
}

#endif // __PONGASOFT_UTILS_CONCURRENT_CONCURRENT_H__