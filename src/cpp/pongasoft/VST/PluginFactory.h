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

#pragma once

#include <string>
#include <functional>
#include <vector>

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/base/ipluginbase.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <public.sdk/source/main/pluginfactory.h>

namespace pongasoft::VST {

/**
 * The purpose of this class is to make it easier and safer to create the plugin factory. The implementation
 * provided with the VST3 SDK relies on macros, comments (like "do not changed this" or "not used here") and
 * ends up creating `static` instances which are never destroyed until the DAW quits (which is the normal behavior
 * for static variables defined inside functions...) even if there are no more instances of the plugin (although
 * it is not a big "leak", it is nonetheless one!).
 *
 * You use this class in the following manner:
 *
 * ```
 * // this is the main entry point that every plugin must define
 * EXPORT_FACTORY Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
 * {
 *   return JambaPluginFactory::GetVST3PluginFactory<
 *     pongasoft::test::jamba::RT::JambaTestPluginProcessor,  // processor class (Real Time)
 *     pongasoft::test::jamba::GUI::JambaTestPluginController // controller class (GUI)
 *   >("pongasoft",                     // vendor
 *     "https://www.pongasoft.com",     // url
 *     "support@pongasoft.com",         // email
 *     stringPluginName,                // plugin name
 *     FULL_VERSION_STR,                // plugin version
 *     Vst::PlugType::kFx               // plugin category (can be changed to other like kInstrument, etc...)
 *   );
 * }
 * ```
 *
 * \note 1. The `GetPluginFactory()` method takes an additional (optional) parameter not shown in this example
 *          which is provided to the factory method of the controller and processor (`iContext`) and can be used
 *          to provide additional information like a constructor argument for the parameters class or the parameters
 *          themselves or whatever else you want.
 *
 * \note 2. If you do not want to use this class, you can continue to use the macros provided with the SDK.
 */
class JambaPluginFactory
{
public:
  /**
   * Main method to create the factory for the plugin. See example in the class comment. The optional `iContext`
   * last arguments will be provided to the controller and processor factory methods (the macros provided
   * with the SDK do not let you set this parameter).
   *
   * @tparam RTProcessorClass the class for the processor (assumes existence of `UUID()` and
   *                          `createInstance(void *)` static methods)
   * @tparam GUIControllerClass the class for the controller (assumes existence of `UUID()` and
   *                            `createInstance(void *)` static methods)
   */
  template<typename RTProcessorClass, typename GUIControllerClass>
  static Steinberg::IPluginFactory* GetVST3PluginFactory(const std::string& iVendor,
                                                         const std::string& iURL,
                                                         const std::string& iEmail,
                                                         const std::string& iPluginName,
                                                         const std::string& iPluginVersion,
                                                         const std::string& iSubCategories,
                                                         void *iContext = nullptr);
};

//------------------------------------------------------------------------
// JambaPluginFactory::GetVST3PluginFactory
//------------------------------------------------------------------------
template<typename RTClass, typename GUIClass>
Steinberg::IPluginFactory *JambaPluginFactory::GetVST3PluginFactory(std::string const &iVendor,
                                                                    std::string const &iURL,
                                                                    std::string const &iEmail,
                                                                    std::string const &iPluginName,
                                                                    std::string const &iPluginVersion,
                                                                    std::string const &iSubCategories,
                                                                    void *iContext)
{
  // implementation note: this code was essentially copied from the macros coming from the SDKs and adapted this way:
  // 1) do not use Steinberg::gPluginFactory global variable
  // 2) do not use any static variables (which end up being small memory leaks)
  Steinberg::PFactoryInfo factoryInfo(iVendor.c_str(),
                                      iURL.c_str(),
                                      iEmail.c_str(),
                                      Steinberg::Vst::kDefaultFactoryFlags);

  // wrapping in a unique_ptr to make sure it gets destroyed if the method does not end properly
  auto factory = std::make_unique<Steinberg::CPluginFactory>(factoryInfo);

  // processor
  {
    Steinberg::TUID lcid = INLINE_UID_FROM_FUID(RTClass::UUID());
    Steinberg::PClassInfo2 component{lcid,
                                     Steinberg::PClassInfo::kManyInstances,
                                     kVstAudioEffectClass,
                                     iPluginName.c_str(),
                                     static_cast<Steinberg::int32>(Steinberg::Vst::kDistributable),
                                     iSubCategories.c_str(),
                                     nullptr,
                                     iPluginVersion.c_str(),
                                     kVstVersionString};

    factory->registerClass(&component, RTClass::createInstance, iContext);
  }

  // controller
  {
    Steinberg::TUID lcid = INLINE_UID_FROM_FUID(GUIClass::UUID());
    Steinberg::PClassInfo2 component{lcid,
                                     Steinberg::PClassInfo::kManyInstances,
                                     kVstComponentControllerClass,
                                     (iPluginName + "Controller").c_str(),
                                     0,
                                     nullptr,
                                     nullptr,
                                     iPluginVersion.c_str(),
                                     kVstVersionString};

    factory->registerClass(&component, GUIClass::createInstance, iContext);
  }

  // here we can release the pointer as it becomes the responsibility of the caller to manage its
  // lifecycle
  return factory.release();
}


}