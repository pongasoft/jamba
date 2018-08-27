//------------------------------------------------------------------------------------------------------------
// This file contains the standard boilerplate code that VST2 requires to instantiate the plugin
// components: it creates an adapter which wraps the VST3 plugin to adapt its API to the VST2 world.
//
// Note that VST2 requires a 4 letter code that needs to be registered with Steinberg =>
// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
//
// Steinberg is planning to officially kill VST2 in October 2018 =>
// https://sdk.steinberg.net/viewtopic.php?f=6&t=557
//------------------------------------------------------------------------------------------------------------
#include "public.sdk/source/vst/vst2wrapper/vst2wrapper.h"
#include "[-name-]CIDs.h"

//------------------------------------------------------------------------
::AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
  return Steinberg::Vst::Vst2Wrapper::create(GetPluginFactory(),
                                             [-namespace-]::[-name-]ProcessorUID,
                                             'TBDx',
                                             audioMaster);
}
