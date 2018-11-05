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
#ifndef __PONGASOFT_UTILS_CONCURRENT_CONCURRENT_H__
#define __PONGASOFT_UTILS_CONCURRENT_CONCURRENT_H__

#include "SpinLock.h"

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
 * The golden rules of real time audio programming are not to use locks or memory allocation. Here are 2
 * implementations with different tradeoffs.
 *
 * The LockFree namespace implements a version that does not use locks or allocate memory at runtime (only when
 * the classes are created). The tradeoff is that it uses more memory (3 instances of T) and it is only thread
 * safe when there is a single thread calling 'get' (resp 'pop') and another single thread calling 'set' (rep 'push').
 *
 * The WithSpinLock namespace a version which uses a very lightweight lock: a user space spin lock. The SpinLock
 * implementation is not allocating any memory in any thread and is relying on the std::atomic_flag concept which is
 * guaranteed to be lock free. It also does not make any system calls. The tradeoff is that the queue and atomic
 * value do lock for the duration of the copy of T. The advantages are less memory use and fully multi thread safe.
 */

//------------------------------------------------------------------------
// Lock Free Implementation of AtomicValue and SingleQueueElement
//------------------------------------------------------------------------
namespace LockFree {
/**
 * This (internal) class stores a single element. This implementation is NOT fully thread safe. It is only thread
 * safe as long as load is called by a single thread and store is called by another single thread as well (if load
 * and store are called by the same thread it is obviously thread safe since it would be mono thread...)
 *
 * TODO: the implementation somehow assumes that T is a real type, not a primitive.. maybe there is a way to write
 * a primitive version (if that becomes a necessity)
 */
template<typename T>
class SingleElementStorage
{
public:
  // wraps a unique pointer of type T and whether it is a new value or not
  struct Element
  {
    Element(std::unique_ptr<T> iElement, bool iNew) noexcept : fElement{std::move(iElement)}, fNew{iNew} {}

    std::unique_ptr<T> fElement;
    bool fNew;
  };

public:
  // Constructor
  SingleElementStorage(std::unique_ptr<T> iElement, bool iIsEmpty) noexcept :
    fSingleElement{new Element(std::move(iElement), !iIsEmpty)}
  {}

  // Destructor - Deletes the element created in the constructor
  ~SingleElementStorage()
  {
    delete fSingleElement.exchange(nullptr);
  }

  // isEmpty
  inline bool isEmpty() const
  {
    return !fSingleElement.load()->fNew;
  }

  /**
   * Used (from test) to make sure that it is a lock free implementation. Relies on an atomic pointer and in
   * general the implementation should be lock free but we make sure here.
   */
  bool __isLockFree() const { return fSingleElement.is_lock_free(); }

protected:
  /**
   * Stores an element in the storage. Replaces the current one if there is one. In order to avoid copy and memory
   * allocation, the storage assumes ownership of the element provided and as result returns the one it replaces.
   */
  std::unique_ptr<Element> store(std::unique_ptr<Element> iElement)
  {
    iElement->fNew = true;
    iElement.reset(fSingleElement.exchange(iElement.release()));
    return std::move(iElement);
  }

  /**
   * Loads an element from storage. In order to avoid copy and memory allocation, the api actually takes an element
   * to replace the one that is returned. The correct usage pattern should be:
   *
   * if(!isEmpty())
   *   myPtr = std::move(load(std::move(myPtr)));
   *
   * That way you are guaranteed that what load returns will have a fNew flag set to true...
   *
   * @return element with flag fNew set to true if there was a new element, false otherwise
   */
  std::unique_ptr<Element> load(std::unique_ptr<Element> iElement)
  {
    iElement->fNew = false;
    iElement.reset(fSingleElement.exchange(iElement.release()));
    return std::move(iElement);
  }

  // __newT => create a new T by using copy constructor
  std::unique_ptr<T> __newT() const { return std::make_unique<T>(*(fSingleElement.load()->fElement)); }

  // __newElement
  std::unique_ptr<Element> __newElement() const { return std::make_unique<Element>(std::move(__newT()), false); }

private:
  // using a std::atomic on a pointer which should be lock free (check __isLockFree for sanity check!)
  std::atomic<Element *> fSingleElement;
};

/**
 * This is the lock free version of the SingleElementQueue. Internally it uses a different pointer for push and pop.
 * As described in the comment for SingleElementStorage, all methods related to 'pop' can be called in one thread
 * while all methods related to 'push' can be called by another.
 */
template<typename T>
class SingleElementQueue : public SingleElementStorage<T>
{
public:
  // Constructor
  SingleElementQueue() :
    SingleElementStorage<T>{std::make_unique<T>(), true},
    fPopValue{std::move(SingleElementStorage<T>::__newElement())},
    fPushValue{std::move(SingleElementStorage<T>::__newElement())}
  {}

  /**
   * This constructor should be used if T does not provide an empty constructor */
  explicit SingleElementQueue(std::unique_ptr<T> iElement, bool iIsEmpty = false) :
    SingleElementStorage<T>{std::move(iElement), iIsEmpty},
    fPopValue{std::move(SingleElementStorage<T>::__newElement())},
    fPushValue{std::move(SingleElementStorage<T>::__newElement())}
  {
  }

  //------------------------------------------------------------------------------------------------------------
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // All the following methods (pop and last) should be called in a single thread
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //------------------------------------------------------------------------------------------------------------
  /**
   * @return the value popped or nullptr if nothing to pop
   */
  T *pop()
  {
    if(!this->isEmpty())
    {
      fPopValue = std::move(SingleElementStorage<T>::load(std::move(fPopValue)));
    }

    if(fPopValue->fNew)
    {
      fPopValue->fNew = false;
      return fPopValue->fElement.get();
    }

    return nullptr;
  };

  /**
   * Copy the popped value to oElement and return true when there is a new value otherwise do nothing and return false.
   */
  bool pop(T &oElement)
  {
    auto element = pop();
    if(element)
    {
      oElement = *element;
      return true;
    }

    return false;
  }

  /**
   * @return returns the last value that was popped (never nullptr). Does NOT check for new value
   */
  T const *last() const
  {
    return fPopValue->fElement.get();
  };


  /**
   * Copy the last value that was popped to oElement. Does NOT check for new value
   */
  void last(T &oElement) const
  {
    oElement = *last();
  }

  /**
 * @return if there is a new value to pop, returns it otherwise return the last value that was popped (never nullptr)
 */
  T const *popOrLast()
  {
    auto element = pop();

    if(element)
      return element;
    else
      return fPopValue->fElement.get();
  };


  /**
   * Copy either the new value (if there is one) or the last value that was popped to oElement
   */
  void popOrLast(T &oElement)
  {
    oElement = *popOrLast();
  }

  //------------------------------------------------------------------------------------------------------------
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // All the following methods (push and updateAndPush) should be called in a single thread
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //------------------------------------------------------------------------------------------------------------

  /**
   * Pushes (a copy of) iElement in the queue.
   */
  void push(T const &iElement)
  {
    *(fPushValue->fElement.get()) = iElement;
    pushValue();
  }

  /**
   * Pushes (a copy of) *iElement in the queue.
   */
  void push(T const *iElement)
  {
    *(fPushValue->fElement.get()) = *iElement;
    pushValue();
  }

  /**
   * Use this flavor of push to avoid copy. ElementModifier will be called back with the internal pointer to
   * update it.
   */
  template<class ElementModifier>
  void updateAndPush(ElementModifier const &iElementModifier)
  {
    iElementModifier(fPushValue->fElement.get());
    pushValue();
  }

  /**
   * Use this flavor of push to avoid copy. ElementModifier will be called back with the internal pointer to
   * update it. This flavor uses a callback that returns true when the push should happen and false otherwise.
   */
  template<class ElementModifier>
  bool updateAndPushIf(ElementModifier const &iElementModifier)
  {
    if(iElementModifier(fPushValue->fElement.get()))
    {
      pushValue();
      return true;
    }
    return false;
  }
private:
  void pushValue()
  {
    fPushValue = std::move(SingleElementStorage<T>::store(std::move(fPushValue)));
  }

private:
  using Element = typename SingleElementStorage<T>::Element;

  std::unique_ptr<Element> fPopValue;
  std::unique_ptr<Element> fPushValue;
};

/**
 * This is the lock free version of the AtomicValue. Internally it uses a different pointer for get and set.
 * As described in the comment for SingleElementStorage, all methods related to 'get' can be called in one thread
 * while all methods related to 'set' can be called by another.
 */
template<typename T>
class AtomicValue : public SingleElementStorage<T>
{
public:
  // Constructor - needs a value for initalizing the object
  explicit AtomicValue(std::unique_ptr<T> iValue) :
    SingleElementStorage<T>{std::move(iValue), false},
    fGetValue{std::move(SingleElementStorage<T>::__newElement())},
    fSetValue{std::move(SingleElementStorage<T>::__newElement())}
  {}

  //------------------------------------------------------------------------------------------------------------
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // All the following methods (get) should be called in a single thread
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //------------------------------------------------------------------------------------------------------------

  /**
   * @return the value
   */
  T const *get()
  {
    if(!this->isEmpty())
    {
      fGetValue = std::move(SingleElementStorage<T>::load(std::move(fGetValue)));
    }

    return fGetValue->fElement.get();
  };

  /**
   * @return a copy of the value
   */
  T getCopy()
  {
    return *get();
  }

  /**
   * Copy the value to oElement
   */
  void get(T &oElement)
  {
    oElement = *get();
  };

  /**
   * Copy the value to *oElement
   */
  void get(T *oElement)
  {
    *oElement = *get();
  };

  //------------------------------------------------------------------------------------------------------------
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // All the following methods (set / update) should be called in a single thread
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //------------------------------------------------------------------------------------------------------------

  /**
   * Copy the value to make it accessible to get
   */
  void set(T const &iValue)
  {
    *(fSetValue->fElement.get()) = iValue;
    fSetValue = std::move(SingleElementStorage<T>::store(std::move(fSetValue)));
  }

  /**
   * Copy the value to make it accessible to get
   */
  void set(T const *iValue)
  {
    *(fSetValue->fElement.get()) = *iValue;
    fSetValue = std::move(SingleElementStorage<T>::store(std::move(fSetValue)));
  }

  /**
   * Use this flavor to avoid copy. ElementModifier will be called back with the internal pointer to
   * update it.
   */
  template<class ElementModifier>
  void update(ElementModifier const &iElementModifier)
  {
    iElementModifier(fSetValue->fElement.get());
    fSetValue = std::move(SingleElementStorage<T>::store(std::move(fSetValue)));
  }

  /**
   * Use this flavor to avoid copy. ElementModifier will be called back with the internal pointer to
   * update it.This flavor uses a callback that returns true when the update should happen and false otherwise.
   */
  template<class ElementModifier>
  bool updateIf(ElementModifier const &iElementModifier)
  {
    if(iElementModifier(fSetValue->fElement.get()))
    {
      fSetValue = std::move(SingleElementStorage<T>::store(std::move(fSetValue)));
      return true;
    }

    return false;
  }

private:
  using Element = typename SingleElementStorage<T>::Element;

  std::unique_ptr<Element> fGetValue;
  std::unique_ptr<Element> fSetValue;
};
}

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
   * Note that although this api is thread safe, it will only report the state of the queue at the moment it is called.
   * You should not assume that because isEmpty() returns false, then pop will return true!
   *
   * @return true if the queue is empty
   */
  bool isEmpty() const
  {
    auto &spinLock = const_cast<SpinLock &>(fSpinLock);
    auto lock = spinLock.acquire();
    return fIsEmpty;
  }

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