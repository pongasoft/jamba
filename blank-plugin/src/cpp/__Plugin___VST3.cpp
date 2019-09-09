//------------------------------------------------------------------------------------------------------------
// This file contains the standard boilerplate code that VST3 sdk requires to instantiate the plugin
// components
//------------------------------------------------------------------------------------------------------------
#include "[-name-]CIDs.h"

#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <public.sdk/source/main/pluginfactoryvst3.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>

#include "version.h"
#include "RT/[-name-]Processor.h"
#include "GUI/[-name-]Controller.h"

using namespace Steinberg::Vst;

#ifndef NDEBUG
#define stringPluginName "[-name-]_Debug"
#else
#define stringPluginName "[-name-]"
#endif


//------------------------------------------------------------------------
//  Module init/exit
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// called after library was loaded
bool InitModule()
{
  return true;
}

//------------------------------------------------------------------------
// called after library is unloaded
bool DeinitModule()
{
  return true;
}

//------------------------------------------------------------------------
//  VST Plug-in Entry
//------------------------------------------------------------------------
BEGIN_FACTORY_DEF ("[-company-]",
                   "[-company_url-]",
                   "[-company_email-]")

    // [-name-]Processor processor
    DEF_CLASS2 (INLINE_UID_FROM_FUID([-namespace-]::[-name-]ProcessorUID),
                PClassInfo::kManyInstances,  // cardinality
                kVstAudioEffectClass,        // the component category (do not changed this)
                stringPluginName,            // here the Plug-in name (to be changed)
                Vst::kDistributable,         // means that component and controller could be distributed on different computers
                Vst::PlugType::kFx,          // Subcategory for this Plug-in (to be changed)
                FULL_VERSION_STR,            // Plug-in version (to be changed)
                kVstVersionString,           // the VST 3 SDK version (do not changed this, use always this define)
                [-namespace-]::RT::[-name-]Processor::createInstance)  // function pointer called when this component should be instantiated

    // [-name-]Controller controller
    DEF_CLASS2 (INLINE_UID_FROM_FUID([-namespace-]::[-name-]ControllerUID),
                PClassInfo::kManyInstances,  // cardinality
                kVstComponentControllerClass,// the Controller category (do not changed this)
                stringPluginName
                "Controller",  // controller name (could be the same than component name)
                0,            // not used here
                "",            // not used here
                FULL_VERSION_STR,    // Plug-in version (to be changed)
                kVstVersionString,    // the VST 3 SDK version (do not changed this, use always this define)
                [-namespace-]::GUI::[-name-]Controller::createInstance)// function pointer called when this component should be instantiated

END_FACTORY
