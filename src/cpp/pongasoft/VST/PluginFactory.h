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
#include <public.sdk/source/main/pluginfactoryvst3.h>

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
 *   return JambaPluginFactory<pongasoft::test::jamba::JambaTestPluginParameters>::GetVST3PluginFactory<
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
 * \note 1. The factory creates and maintain an instance of the `Parameters` class and is provided as the context to both
 *       the processor and controller. Because the factory is the last thing being destroyed, it is guaranteed that
 *       the parameters will outlive the processor and controller.
 *
 * \note 2. If you do not want to use this class, you can continue to use the macros provided with the SDK.
 *
 * @tparam ParametersClass the class of the parameters (if the contructor takes arguments, you provide them
 *                         as extra parameters at the end of `GetVST3PluginFactory`)
 */
template<typename ParametersClass>
class JambaPluginFactory : Steinberg::CPluginFactory
{
public:
  /**
   * Main method to create the factory for the plugin. See usage in the class comment. The optional `iParametersArgs`
   * are arguments that are provided to the `ParametersClass` in the event the constructor requires arguments.
   *
   * @tparam RTProcessorClass the class for the processor (assumes existence of `UUID()` and
   *                          `createInstance(void *)` static methods)
   * @tparam GUIControllerClass the class for the controller (assumes existence of `UUID()` and
   *                            `createInstance(void *)` static methods)
   * @tparam Args the types for the parameters class constructor (usually none, but otherwise deduced by call...)
   */
  template<typename RTProcessorClass, typename GUIControllerClass, typename... Args>
  static Steinberg::IPluginFactory* GetVST3PluginFactory(const std::string& iVendor,
                                                         const std::string& iURL,
                                                         const std::string& iEmail,
                                                         const std::string& iPluginName,
                                                         const std::string& iPluginVersion,
                                                         const std::string& iSubCategories,
                                                         Args&& ...iParametersArgs);

  /**
   * Implementation note: because the implementation of this method uses `delete this` it must be overridden to
   * implement the proper behavior and delete the subclass instead!
   */
  virtual Steinberg::uint32 PLUGIN_API release() SMTG_OVERRIDE
  {
    if(::Steinberg::FUnknownPrivate::atomicAdd(__funknownRefCount, -1) == 0)
    {
      delete this;
      return 0;
    }
    return __funknownRefCount;
  }

protected:
  //! Constructor: called by `GetVST3PluginFactory`
  template<typename... Args>
  explicit JambaPluginFactory(Steinberg::PFactoryInfo const &iFactoryInfo, Args&& ...iParametersArgs) :
    CPluginFactory(iFactoryInfo),
    fParameters{std::forward<Args>(iParametersArgs)...}
  {}

protected:
  ParametersClass fParameters;
};

//------------------------------------------------------------------------
// JambaPluginFactory::GetVST3PluginFactory
//------------------------------------------------------------------------
template<typename PClass>
template<typename RTClass, typename GUIClass, typename... Args>
Steinberg::IPluginFactory *JambaPluginFactory<PClass>::GetVST3PluginFactory(std::string const &iVendor,
                                                                            std::string const &iURL,
                                                                            std::string const &iEmail,
                                                                            std::string const &iPluginName,
                                                                            std::string const &iPluginVersion,
                                                                            std::string const &iSubCategories,
                                                                            Args &&... iParametersArgs)
{
  // implementation note: this code was essentially copied from the macros coming from the SDKs and adapted this way:
  // 1) do not use Steinberg::gPluginFactory global variable
  // 2) do not use any static variables (which end up being small memory leaks)
  Steinberg::PFactoryInfo factoryInfo(iVendor.c_str(),
                                      iURL.c_str(),
                                      iEmail.c_str(),
                                      Steinberg::Vst::kDefaultFactoryFlags);

  // wrapping in a unique_ptr to make sure it gets destroyed if the method does not end properly
  // note: std::make_unique does not work due to protected constructor...
  std::unique_ptr<JambaPluginFactory<PClass>> factory{new JambaPluginFactory<PClass>(factoryInfo,
                                                                                     std::forward<Args>(iParametersArgs)...)};

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

    factory->registerClass(&component, RTClass::createInstance, &factory->fParameters);
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

    factory->registerClass(&component, GUIClass::createInstance, &factory->fParameters);
  }

  // here we can release the pointer as it becomes the responsibility of the caller to manage its
  // lifecycle
  return factory.release();
}


}