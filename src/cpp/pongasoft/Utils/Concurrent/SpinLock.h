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

#include <atomic>

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
