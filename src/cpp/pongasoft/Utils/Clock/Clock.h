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