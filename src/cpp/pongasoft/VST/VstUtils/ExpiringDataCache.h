/*
 * Copyright (c) 2020 pongasoft
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

#ifndef JAMBA_EXPIRINGDATACACHE_H
#define JAMBA_EXPIRINGDATACACHE_H

#include <pongasoft/VST/Timer.h>
#include <functional>

#include <pongasoft/logging/logging.h>

namespace pongasoft::VST::VstUtils {

using namespace Steinberg;

/**
 * The purpose of this class is to implement a short live cache (time to live (or TTL) being a constructor parameter)
 * for data. This class uses the `Timer` class from the VST3 SDK to implement the timeout so that it can safely be used
 * by all classes living in the plugin. This class is **not** thread safe but is intended to be used in the VST3 plugin
 * environment where the timer (`ITimerCallback::onTimer`) and caller (`ExpiringDataCache::getData`) are part of the
 * event loop and thus never called by 2 threads at the same time.
 *
 * \note This class offers copy/move constructors and copy/move assignment operators but care must be taken that
 *       the `Loader` can safely be moved/copied if you use them. A **bad** example would be a lambda capturing `this`
 *
 * ```
 * struct MyClass
 * {
 *    std::shared_ptr<MyData> getMyData() const { return fMyDataCache.getData(); }
 *
 *    // this is good because it is capturing a shared pointer independent of "this"
 *    MyClass(std::shared_ptr<MyStorage> iStorage) : fStorage{iStorage},
 *      fMyDataCache{[iStorage]() -> auto { return iStorage->loadMyData(); }, myTTL} {}
 *
 *    // This is terrible:
 *    // MyClass(std::shared_ptr<MyStorage> iStorage) : fStorage{iStorage},
 *    //  fMyDataCache{[this]() -> auto { return fStorage->loadMyData(); }, myTTL} {}
 *    //
 *    // And will fail with code like this:
 *    // MyClass a1{myStorage};
 *    // MyClass a2{a1}; // this will lead to failure if a1 goes away because Loader captures the address of a1!!!!
 *
 *    std::shared_ptr<MyStorage> fStorage{};
 *    mutable ExpiringDataCache<MyData> fMyDataCache{}; // mutable allows to declare `getMyData()` const!
 * }
 * ```
 *
 * @tparam T the type of the data that is being cached
 * @tparam Ptr the (optional) type for the pointer. By default it is `std::shared_ptr<T>` because this makes the most
 *             sense for data that needs to be shared (clearly this class keeps it and then the caller via
 *             `ExpiringDataCache::getData()`). You can use `T*` if you prefer/need to, but is not recommended.
 * @tparam Loader the (optional) type for the loader "function" which must be a "callable" object (`iDataLoader()`
 *                returns a `Ptr`) and is by default `std::function<Ptr()>`.
 */
template<typename T, typename Ptr = std::shared_ptr<T>, typename Loader = std::function<Ptr()>>
class ExpiringDataCache : ITimerCallback
{
public:
  using value_type = T;
  using pointer = Ptr;
  using loader = Loader;

public:
  //! Default empty constructor => `getData()` always return `nullptr`
  ExpiringDataCache() = default;

  //! Main constructor with loader and TTL
  ExpiringDataCache(Loader iDataLoader, uint32 iTimeToLiveMilliseconds) :
    fDataLoader{std::move(iDataLoader)}, fTimeToLiveMilliseconds{iTimeToLiveMilliseconds} {}

  //! Copy constructor
  ExpiringDataCache(ExpiringDataCache const &iOther) :
    fDataLoader{iOther.fDataLoader}, fTimeToLiveMilliseconds{iOther.fTimeToLiveMilliseconds} {}

  //! Move constructor
  ExpiringDataCache(ExpiringDataCache &&iOther) noexcept :
    fDataLoader{std::move(iOther.fDataLoader)}, fTimeToLiveMilliseconds{iOther.fTimeToLiveMilliseconds} {}

  //! Copy assignment operator
  ExpiringDataCache &operator=(ExpiringDataCache const &iOther) noexcept
  {
    fDataLoader = iOther.fDataLoader;
    fTimeToLiveMilliseconds = iOther.fTimeToLiveMilliseconds;
    fCachedData = nullptr;
    fTimer = nullptr;

    return *this;
  }

  //! Move assignment operator
  ExpiringDataCache &operator=(ExpiringDataCache &&iOther) noexcept
  {
    fDataLoader = std::move(iOther.fDataLoader);
    fTimeToLiveMilliseconds = iOther.fTimeToLiveMilliseconds;
    fCachedData = nullptr;
    fTimer = nullptr;

    return *this;
  }

  //! Allow to write `if(cache)` to check if the cache is properly initialized
  explicit operator bool() const noexcept { return fDataLoader ? true : false; }

  /**
   * Main api to retrieve the data. If the data is in the cache, it simply returns it (and extends the duration to
   * remain in the cache by TTL). If it is not in the cache, it delegates to the loader to get it. Note that it is ok
   * for the loader to return `nullptr`, but this result will not be cached (meaning, the loader will be called
   * every time).
   *
   * @return the data (which can be `nullptr`) */
  Ptr getData()
  {
    if(fCachedData)
    {
      // resets the timer
      fTimer = AutoReleaseTimer::create(this, fTimeToLiveMilliseconds);
      return fCachedData;
    }

    fCachedData = fDataLoader ? fDataLoader() : nullptr;

    if(fCachedData)
      fTimer = AutoReleaseTimer::create(this, fTimeToLiveMilliseconds);

    return fCachedData;
  }

private:
  // Callback when the timer expires => remove cached data
  void onTimer(Timer *timer) override
  {
    fCachedData = nullptr;
    fTimer = nullptr;
  }

private:
  Loader fDataLoader{};
  uint32 fTimeToLiveMilliseconds{};

  Ptr fCachedData{};
  std::unique_ptr<AutoReleaseTimer> fTimer{};
};

}

#endif //JAMBA_EXPIRINGDATACACHE_H