#ifndef VST3_JAMBA_[-name-]_CIDS_H
#define VST3_JAMBA_[-name-]_CIDS_H

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/vsttypes.h>

namespace [-namespace-] {

//------------------------------------------------------------------------
// These 2 IDs are used in [-name-]_VST3.cpp to create
// the processor (RT) and controller (GUI). Those IDs are unique and have
// been generated automatically. Using different ids for Debug/Release
// targets so that both plugins can live side by side.
//------------------------------------------------------------------------
#ifndef NDEBUG
static const ::Steinberg::FUID [-name-]ProcessorUID([-debug_processor_uuid-]);
static const ::Steinberg::FUID [-name-]ControllerUID([-debug_controller_uuid-]);
#else
static const ::Steinberg::FUID [-name-]ProcessorUID([-processor_uuid-]);
static const ::Steinberg::FUID [-name-]ControllerUID([-controller_uuid-]);
#endif

//------------------------------------------------------------------------
// Parameters and Custom view ids
//------------------------------------------------------------------------
enum E[-name-]ParamID : Steinberg::Vst::ParamID
{
  // although NOT a requirement, I like to start at 1000 so that they are all 4 digits.
  // the grouping and numbering is arbitrary and you can use whatever makes sense for your case.

  // the bypass parameter which has a special meaning to the host
  kBypass = 1000,
};

}

#endif // VST3_JAMBA_[-name-]_CIDS_H