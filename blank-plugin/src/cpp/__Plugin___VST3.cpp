//------------------------------------------------------------------------------------------------------------
// This file contains the standard boilerplate code that VST3 sdk requires to instantiate the plugin
// components
//------------------------------------------------------------------------------------------------------------
#include "[-name-]CIDs.h"

#include "version.h"
#include "RT/[-name-]Processor.h"
#include "GUI/[-name-]Controller.h"

#include <pongasoft/VST/PluginFactory.h>

using namespace pongasoft::VST;

//------------------------------------------------------------------------
//  VST3 Plugin Main entry point
//------------------------------------------------------------------------
SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
{
  return JambaPluginFactory::GetVST3PluginFactory<
    [-namespace-]::RT::[-name-]Processor, // processor class (Real Time)
    [-namespace-]::GUI::[-name-]Controller // controller class (GUI)
  >("[-company-]", // company/vendor
    "[-company_url-]", // url
    "[-company_email-]", // email
    stringPluginName, // plugin name
    FULL_VERSION_STR, // plugin version
    Vst::PlugType::kFx // plugin category (can be changed to other like kInstrument, etc...)
   );
}
