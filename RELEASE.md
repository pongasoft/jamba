Release Notes {#release-notes}
-------------

Check the [Requirements](https://jamba.dev/requirements/) documentation for information about which VST3 SDK Jamba supports and how to get it.

## [6.2.0 | 2023-04-19](https://github.com/pongasoft/jamba/tree/v6.2.0)

* Enhanced `AudioBuffer` API

## [6.1.0 | 2023-04-15](https://github.com/pongasoft/jamba/tree/v6.1.0)

* Added support for arrays of properties (see [Plugin.h](https://github.com/pongasoft/jamba/blob/master/jamba-test-plugin/src/cpp/Plugin.h) for a full example). For example, you can now write:
  ```cpp
  // Parameters
  VstParams<bool, 2> fArrayVst;
  
  // State
  RTVstParams<bool, 2> fArrayVst;
  ```
* Upgraded GoogleTest to 1.13.0 (and download a zip instead of cloning the repo)  

## [6.0.1 | 2022-06-02](https://github.com/pongasoft/jamba/tree/v6.0.1)

* Fixed gtest crash on Apple M1 platform

## [6.0.0 | 2022-05-30](https://github.com/pongasoft/jamba/tree/v6.0.0)

* Migrated Jamba to use VST3 SDK 3.7.5 (introduces breaking changes)
* Requires CMake minimum version 3.19 (SDK requirement)
* Added `inspect` command to run the `VST3Inspector` tool
* Added `info` command to run the `moduleinfotool` tool (new since SDK 3.7.5)
* Generate and bundle `moduleinfo.json` (new since SDK 3.7.5)
* On Windows, build a module (`.vst3` folder) instead of a DLL when `VST2` is disabled
* Use json format instead of xml (new since SDK 3.7.2)

> #### Migration required
>
> * `InitModule()` (resp. `DeinitModule()`) cannot be used anymore (since the SDK now implements them).
>   Instead, you need to use `Steinberg::ModuleInitializer` (resp. `Steinberg::ModuleTerminator`) concept.
>   If your code simply has empty implementations for these functions, you can safely delete them.
> * You should provide the version of the plugin in the project definition (required by `moduleinfotool`)
>  ```
>  # From CMakeLists.txt
>  project(MyPlugin VERSION "${PLUGIN_VERSION}")
>  ```
> * The `GlobalKeyboardHook::onKeyDown` and `GlobalKeyboardHook::onKeyUp` methods have been replaced by
>   a single `GlobalKeyboardHook::onKeyboardEvent` (due to changes in the SDK)

## [5.1.5 | 2021-09-13](https://github.com/pongasoft/jamba/tree/v5.1.5)

* Added support for snapshot
* Processes `Info.plist` through CMake replacement, thus allowing to inject dates, versions, etc...
* Processes `Info.plist` through Xcode preprocessing when using Xcode generator
* Fixed issue when plugin was generated without Xcode generator (macOS only) which lead to an `Info.plist` without 
  proper version
* Use latest version of GoogleTest (get rid of CMake warnings)
* Use `gtest_discover_tests` instead of `gtest_add_tests` to minimize CMake invocation when building tests
* Make sure that `.rc` file gets regenerated properly when resource files change (Windows)
* Fixed test issue

> #### Potential breaking changes
>
> * Due to the change in `Info.plist` processing, you should replace:
>    ```xml
>    <-- File mac/Info.plist -->
>    <-- Replace this (processed only via Xcode preprocessing) -->
>    <key>CFBundleExecutable</key>
>    <string>$(EXECUTABLE_NAME)</string>
>    
>    <-- With this (processed with all generators) -->
>    <key>CFBundleExecutable</key>
>    <string>@MACOSX_BUNDLE_EXECUTABLE_NAME@</string>
>    ```
>
> * Note that since Audio Unit plugins are only generated with the Xcode generator, this change is not necessary for 
  `audio-unit/Info.plist`

## [5.1.4 | 2021-02-06](https://github.com/pongasoft/jamba/tree/v5.1.4)

* Fixed issue #11: _create-plugin.py instructions point to wrong configure script_
* Fixed issue #10: _jamba is for python 3.7+_

## [5.1.3 | 2021-01-04](https://github.com/pongasoft/jamba/tree/v5.1.3)

* Fixed compilation issue on Windows

## [5.1.2 | 2021-01-02](https://github.com/pongasoft/jamba/tree/v5.1.2)

* Added support for Apple Silicon / universal build and macOS target deployment (no code change)
* Added (optional) `ARCHIVE_FILENAME` and `ARCHIVE_ARCHITECTURE` to the main `jamba_add_vst_plugin` CMake api

#### Migration optional

* Apple Silicon support is optional but if you want to benefit from it you need to change your `CMakeLists.txt` 
    * You can check the [migration guide](https://jamba.dev/releases/migration) for more details and help.
    * You can also go to [Jamba Web Quickstart](https://jamba.dev/quickstart/web/), generate a blank plugin and use the generated `CMakeLists.txt` as an example.
    * You can also check the [documentation plugin](https://github.com/pongasoft/jamba-sample-gain) for another example.

## [5.1.1 | 2020-12-07](https://github.com/pongasoft/jamba/tree/v5.1.1)

* Added `JambaPluginFactory::GetNonDistributableVST3PluginFactory` function to create a plugin that would not be distributable
* Added default implementation to `IParamSerializer` methods
* Fixed Audio Unit build (changes in VST 3.7.0)
* Added missing `uninstall-au` command to `jamba.py`

## [5.1.0 | 2020-11-23](https://github.com/pongasoft/jamba/tree/v5.1.0)

* Migrated Jamba to use VST3 SDK 3.7.1

#### Potential breaking changes

* SDK 3.7.1 made a number of changes in the cmake based build files (mostly renaming) and it is all covered by Jamba unless you have been using those symbols directly in your own cmake build files
* SDK 3.7.1 renamed the define `EXPORT_FACTORY` into `SMTG_EXPORT_SYMBOL`. Jamba is adding it back for backward compatibility (in `PluginFactory.h`) but you may want to update it.

## [5.0.0 | 2020-08-26](https://github.com/pongasoft/jamba/tree/v5.0.0)

* Migrated Jamba to use VST3 SDK 3.7.0
* Major refactoring of CMake files to make it easier to use and extend/customize (check [CMake Build](https://jamba.dev/documentation/cmake))
* Replaced main dev script implementation with a python script (with the added benefit that the Windows version is now on par with the macOS one). Note that the wrapper shell scripts (`jamba.sh` and `jamba.bat`) are still the official way to invoke it and `jamba.py` should be treated as an implementation detail.
* Improved main script to run multiple commands (ex: `jamba.sh test validate`)
* Replaced configure script with a python script (blank plugin now uses it)
* Added many new (cmake) targets so that you no longer have to rely on the script. For example running the editor can now simply be achieved by building the `jmb_run_editor` target.
* Jamba itself has not changed (no new API) and only very few tweaks were required to migrate to 3.7.0

<h3 class="release-note notice warning">WARNING</h3>

#### Migration required

* You will need to migrate your main `CMakeLists.txt` to invoke `jamba_add_vst_plugin` which is the new (cmake) API to add a VST3 plugin (single call).
  * You can check the [migration guide](https://jamba.dev/releases/migration) for more details and help.
  * You can also go to [Jamba Web Quickstart](https://jamba.dev/quickstart/web/), generate a blank plugin and use the generated `CMakeLists.txt` as an example.
  * You can also check the [documentation plugin](https://github.com/pongasoft/jamba-sample-gain) for another example.
* Note that the zip file generated by the archive command / target is slightly different due to the use of `cpack` (but due to the CMake refactoring it is easy to turn it off entirely or implement your own)

#### Potential breaking changes

* Because Jamba now uses VST3 SDK 3.7.0 there might be APIs that you were using from the SDK that have changed (for example Drag'n'Drop is handled differently)
* Because the latest VST3 SDK does not include VST2 anymore, you need to provide it as a separate path (`VST2_SDK_ROOT`)
* Jamba now has a requirement on python (3) for the script (note that you can disable generation of the script by using the CMake option `JAMBA_ENABLE_DEV_SCRIPT` in which case python is not required)
* `install` command no longer deletes the folder before installation. Use `./jamba.sh uninstall install` if you want this behavior.
* `prod` command has been removed from main dev script (equivalent to `jamba.sh -r -b test validate archive` but check migration guide for details on how to add it back as a target if you really need it)
* Requires CMake 3.19
* Tested on macOS 10.14.6 with Xcode 11.3.1 (may work with prior versions but untested). Note that there is no requirement changes for Windows.

#### Misc.

* If you want to benefit from the new configure python script (`configure.py`), simply copy it from the blank plugin (it is 100% generic so it will work for any Jamba project)


## [4.5.0 | 2020-08-03](https://github.com/pongasoft/jamba/tree/v4.5.0)

* Improved messaging (GUI <-> RT) and VST2 performance by using `FastWriteMemoryStream` (a faster implementation of `MemoryStream`)
* Added the ability to show and dismiss a dialog view (for example, an About view or an alert):

      // Example from some controller (aboutButton is a TextButtonView)
      aboutButton->setOnClickListener([this] {
        fState->showDialog("about_dialog");
      });
     
      // From another controller tied to the about_dialog view (set via the sub-controller attribute)
      dismissButton->setOnClickListener([this] {
        fState->dismissDialog();
      });


## [4.4.0 | 2020-04-26](https://github.com/pongasoft/jamba/tree/v4.4.0)

* Updated loguru to latest (as of master on 2020-03-31)
* Added missing `using` types and `toUTF8String()` methods to `XXXParam` classes
* Added `resetToDefault()` method to all parameters (since a default value is provided when the parameter is created, it is a very convenient way to get back to it without having to know about it and using some global constant)
* Added `ExpiringDataCache` concept (using `Timer` from the VST3 SDK)
* Renamed the plugin file from the blank plugin to `Plugin.h` (instead of the name of the plugin)
* Changed the way the plugin gets registered to the VST3 world (implements the _main_ `GetPluginFactory()` function) with an easier and less error prone syntax: 

      EXPORT_FACTORY Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
      {
        return JambaPluginFactory::GetVST3PluginFactory<
          pongasoft::test::jamba::RT::JambaTestPluginProcessor, // processor class (Real Time)
          pongasoft::test::jamba::GUI::JambaTestPluginController // controller class (GUI)
        >("pongasoft",                 // vendor
          "https://www.pongasoft.com", // url
          "support@pongasoft.com",     // email
          stringPluginName,            // plugin name
          FULL_VERSION_STR,            // plugin version
          Vst::PlugType::kFx           // plugin category (can be changed to other like kInstrument, etc...)
         );
      }
          
* This new syntax is optional and the old/macro based syntax is still valid and acceptable (new blank plugins will be created using this new methodology). If you want to migrate your codebase to the new factory you can check this [commit](https://github.com/pongasoft/jamba-sample-gain/commit/36818aaf77ce543cf133ac1e7d1e85aeb71f6b4e) for the `JambaSampleGain` project as an example.

## [4.3.0 | 2020-03-30](https://github.com/pongasoft/jamba/tree/v4.3.0)

* Handle state deprecation which allows a plugin to change the state saved by a previous version in a non backward compatible way (for example removing parameters)
* Added `StepPadView` to allow a parameter value to be changed by dragging the mouse (useful for lcd displays, etc...)
* Fixed `MomentaryButtonView` to decouple the state of the button with the value of the parameter (which caused several rendering issues)

## [4.2.0 | 2020-03-16](https://github.com/pongasoft/jamba/tree/v4.2.0)

* Added `ParamImageView` to render a parameter value as an image (very useful for status LEDs, etc...)
* Added `ImageView` to render an image (useful for static images, logos, decals, stickers, etc...)
* Upgraded `googletest` to `1.10.0` (due to CMake caching, this requires deleting the CMake build folder to take effect on existing projects)

## [4.1.0 | 2020-02-09](https://github.com/pongasoft/jamba/tree/v4.1.0)

* Due to issues with implicit conversion triggering potential hard crash, deprecated implicit conversion to `T` for all params (`RTRawVstParam`, `RTParam`, `GUIRawVstParam`, `GUIVstParam`, `GUIJmbParam`, `GUIOptionalParam`).
* Added convenient dereference operator (`operator *`) to all params as a shortcut to invoke `.value()` (which is the preferred way to fix the deprecated warnings)
* Added convenient equality and relational operators to all params so that the params still continue to behave like the underlying type (`T`) in many instances

* Handle `toUTF8String()` for Jmb parameter when no serializer is provided by checking the availability of `ostream<<` for `T`
* Allow the use of the template name `_` for the switch container view (for example to use as an overlay that is shown/hidden)

## [4.0.1 | 2019-11-29](https://github.com/pongasoft/jamba/tree/v4.0.1)

* Added Visual Studio Code configuration files for Windows 10

## [4.0.0 | 2019-10-28](https://github.com/pongasoft/jamba/tree/v4.0.0)

This release contains a lot of big changes. Although I tried my best to be backward compatible there may be a few changes that might be breaking.

* Build requirements: Jamba now requires C++17 and Visual Studio Build Tools 2019 (for Windows) (note that XCode 9.2 remains the requirement for macOS)
* Major GUI parameter refactoring:
  * Introduced `IGUIParameter` and typed hierarchy (`ITGUIParameter<T>`) as the base of all GUI parameters
  * Introduced `GUIOptionalParam` to handle Vst, Jmb or no parameter in a consistent fashion
  * Introduced the concept of *discrete* parameter (`IGUIParameter::asDiscreteParameter(int32 iStepCount)`) to convert (if possible) any parameter 
    into a discrete parameter (which is a parameter backed by an `int32` with a given number of steps)
  * Refactored most views to use the new optional and discrete parameter concepts, thus making them way more flexible
  * Changed `MomentaryButtonView` and `ToggleButtonView` to use discrete parameter concept (thus allowing other types than `bool`) and allowing
    to override the value of the steps representing *on* and *off*.
  * Changed `SwitchViewContainer` to use discrete parameter concept, thus allowing to use any Vst or Jmb parameter that is 
    (or can be interpreted as) a discrete parameter.
* Major renaming: introduced `ParamAware` (formerly `GUIParamCxAware`) and `StateAware` (formerly `PluginAccessor`) (added deprecation warnings for old names) and related.
* Jmb GUI parameters no longer needed to be explicitly added to `GUIState` (similar to Vst parameters)
* Added `IDiscreteConverter<T>` concept for Jmb parameters to be able to convert a Jmb parameter to a discrete parameter
* Added `ParamDisplayView` to display the value of any parameter (Vst or Jmb)
* Added `DebugParamDisplayView` to display and highlight the value of a parameter (very useful during development)
* Added `ListAttribute` to handle attributes backed by a list of values (for example enumeration)

      // Example (from StepButtonView.h)
      registerListAttribute<EArrowDirection>("arrow-direction",
                                             &StepButtonView::getArrowDirection,
                                             &StepButtonView::setArrowDirection,
                                             {
                                               {"auto", EArrowDirection::kAuto},
                                               {"up", EArrowDirection::kUp},
                                               {"right", EArrowDirection::kRight},
                                               {"down", EArrowDirection::kDown},
                                               {"left", EArrowDirection::kLeft}
                                             }

* Simplified parameter initialization by allowing to use initializer list and added more generic `DiscreteTypeParamConverter`:

      // Example: with DiscreteTypeParamConverter, no need to specify size in advance. Also note the 
      //          use of the initializer list to initialize it.
      fEnumClassVst =
        vst<DiscreteTypeParamConverter<EEnumClass>>(EJambaTestPluginParamID::kEnumClassVst, STR16("EnumClassVst"),
                                                    {
                                                      {EEnumClass::kEnumClass0, STR16("kEC0 (vst)")},
                                                      {EEnumClass::kEnumClass1, STR16("kEC1 (vst)")},
                                                      {EEnumClass::kEnumClass2, STR16("kEC2 (vst)")},
                                                      {EEnumClass::kEnumClass3, STR16("kEC3 (vst)")},
                                                    })
          .add();


* Added `jamba-test-plugin` to the Jamba project as a unit test/development plugin for Jamba (demonstrates ALL available views)
* Added `test.sh` to conveniently run the jamba tests (which are declared by `jamba-test-plugin`) from the command line
* Added `test_jamba` target to run the jamba tests from the IDE
* Added 2 different sets of UUIDs in blank plugin to handle Debug vs Release deployments

<h3 class="release-note notice warning">WARNING</h3>

#### The following changes are potentially breaking changes

* Changed the way Jamba handles parameter registration:
  * Introduced `ICustomViewLifecycle` which is now used instead of `GUIParamCxAware` (in `CustomViewFactory`)
  * The default behavior is now to call `ParamAware::unregisterAll()` followed by `ParamAware::registerParameters()`
  * The consequence is that the `registerParameters` method that you implement in your view is much simpler because 
    it does not have to handle previously registered parameters (which was not trivial to keep an accurate track of)
  * Allow multiple registrations per paramID. Prior to this release, registering multiple callbacks per paramID 
    resulted in only the last one registered being called. Now all of them will be called. 
* `StepButtonView` and `DiscreteButtonView` now use discrete values only (no longer `double`). Check the documentation on how to use it instead (ex: an increment of `0.1` can be converted to an increment of `1` with a step count of `10`).
* Changed `TagID` to be an unsigned int (`ParamID` aka `uint32`)
* Changed default template type for `DiscreteValueParamConverter` to be an `int32`
* Although care has been taken with renaming/adding deprecation warnings for old names, there is a chance (mostly with templated parameters) that it may impact compilation.

## [3.2.4 | 2019-08-03](https://github.com/pongasoft/jamba/tree/v3.2.4)

* fixed blank plugin `initialize` API to match superclass



## [3.2.3 | 2019-08-01](https://github.com/pongasoft/jamba/tree/v3.2.3)

* Implemented [#6](https://github.com/pongasoft/jamba/issues/6): added cmake targets `build_vst3`, `test_vst3`, `install_vst3`, `install_vst2`, `build_au` and `install_au`
* This change allows those targets to be used directly in an IDE and for example use them for debugging (ex: attach a DAW executable to `install_vst2` or `install_vst3`)
* Modified `jamba.sh` and `jamba.bat` to use the new cmake targets (making the scripts more generic)
* Added ability to customize (extend or completely override) the following targets:
  * `build_vst3` by setting an optional `BUILD_VST3_TARGET` variable
  * `test_vst3` by setting an optional `TEST_VST3_TARGET` variable
  * `build_au` by setting an optional `BUILD_AU_TARGET` variable



## [3.2.2 | 2019-07-23](https://github.com/pongasoft/jamba/tree/v3.2.2)

* released [Jamba](https://jamba.dev/) website with [Quickstart](https://jamba.dev/quickstart/) section to make it very easy to generate a blank plugin (answer a few questions in the browser)
* added (optional) VST3 SDK automatic download to the blank plugin (web and command line)
* fixed blank plugin creation issue when no company was provided



## [3.2.1 | 2019-05-19](https://github.com/pongasoft/jamba/tree/v3.2.1)

* fixed `jamba.sh` script to handle filename with white spaces and install audio unit before validation (validation tool unfortunately works from an installed component, not a file...)



## [3.2.0 | 2019-05-19](https://github.com/pongasoft/jamba/tree/v3.2.0)

* added audio unit validation to `jamba.sh` script (`jamba.sh validate-au`) which runs the `auvaltool` utility on the audio unit plugin
* added switch container view to handle tab like switching (check `Views::SwitchContainerView`)
* added (optional) image for text buttons (`Views::TextButtonView`)
* added (optional) disabled state for momentary buttons (`Views::MomentaryButtonView`)
* added discrete button view (a button which is "on" only when the backing discrete parameter matches the `step` value). This button can be used for radio groups, tabs, etc... (check `Views::DiscreteButtonView`)

<h3 class="release-note notice warning">WARNING</h3>

#### The following changes are potentially breaking changes

* There is a small potentially breaking API change with function `Views::createCustomView` (defined in `Views/CustomViewCreator.h`). Check [commit](https://github.com/pongasoft/jamba/commit/08d06ceda9b936b92c45b8bd9f22a93acfeb3995). The function takes an additional 2 parameters which can be safely ignored if you don't need them.

## [3.1.0 | 2019-02-10](https://github.com/pongasoft/jamba/tree/v3.1.0)

* added ability to conveniently register a global key hook (check `Views::registerGlobalKeyboardHook`)
* parameters now copy the provided strings (title, short tile and units) so that they no longer have to be static and for example can be computed
* added convenient methods to `AudioBuffers` and `Channel` (`copyFrom` and `forEachSample`). Defined `const` and non `const` versions.
* added Step Button view which is a button that increment/decrement a parameter by a fixed amount when clicked. Another amount (tied to shift click) can be provided.



## [3.0.2 | 2019-01-06](https://github.com/pongasoft/jamba/tree/v3.0.2)

* fixed minor bugs



## [3.0.1 | 2019-01-04](https://github.com/pongasoft/jamba/tree/v3.0.1)

* Use shallow cloning to avoid downloading the (unused) project history



## [3.0.0 | 2019-01-03](https://github.com/pongasoft/jamba/tree/v3.0.0)

* This is a major release with many changes (a few breaking APIs)
* Added `TextViewButton` on which you can register a `onClickListener` or implement `onClick`. Also handles disabled state.
* Added `ScrollbarView` which implements a scrollbar tied to 2 parameters (offset and zoom)
* Added `CustomController` to implement a custom controller tied into Jamba (access to Vst/Jmb parameters and state)
* Added ability to easily switch to a new view (`GUIController::switchToView`)
* Added `GUIJmbParameter::updateIf` to update the value in place when necessary
* Added callback APIs to `GUIParamCxAware`

        registerCallback<bool>(fParams->fMyParam,
                               [this] (GUIVstParam<bool> &iParam) {
                                 flushCache();
                               });

* Added registering callbacks and parameters on a view without inheriting from it (can be used from controllers `verifyView` method):

        auto button = dynamic_cast<Views::TextButtonView *>(iView);
        if(button) {
          auto callback = [] (Views::TextButtonView *iButton,
                              GUIJmbParam<SampleData> &iParam) {
            iButton->setMouseEnabled(iParam->hasUndoHistory());
          };
          fState->registerConnectionFor(button)
                ->registerCallback<SampleData>(fState->fSampleData,
                                               std::move(callback),
                                               true);
        }

* Added optional arguments to `Parameters::vst<>()` (resp. `Parameters::jmb<>()`) that get passed through the converter (resp. serializer) allowing syntax like

        fPlayModeHold =
          vst<BooleanParamConverter>(1201, // param ID
                                     STR16("Play Mode"), // param title
                                     STR16("Trigger"), STR16("Hold")) // BooleanParamConverter args

* Requires C++14
* Added `EnumParamConverter` for Vst parameters backed by an enum
* Added `Range` concept
* Refactored `CustomViewCreator` code to simplify writing individual attributes. Introduced `MarginAttribute`, `RangeAttribute`, and `GradientAttribute`

<h3 class="release-note notice warning">WARNING</h3>

#### The following changes are potentially breaking changes

* Refactored `Lerp` class to deal with type parameters differently (`TFloat` for math precision, `X` for type of x, `Y` for type of y). Introduced `SPLerp` (single precision) and `DPLerp` (double precision) as well as several convenient methods. Example:

        // this will interpolate (SP=single precision)
        // X -> the time (long) from the range [0, fFadeDuration]
        // Y -> to the alpha (uint8_t) range [255, 0] (opaque -> transparent)
        // Note that although X and Y types are integer flavors, the interpolation will use floats
        // and the value returned will be cast to uint8_t
        fColor.alpha = Utils::mapValueSPXY<long, uint8_t>(iTime, 0, fFadeDuration, 255, 0);

* `GUIParamCxAware` (which is the class used for registering parameters) has been simplified with `registerParam` methods (when the type is known).
* Moved `PluginAccessor` into its own header file
* Removed `CustomViewInitializer`


## [2.1.2 | 2018-10-11](https://github.com/pongasoft/jamba/tree/v2.1.2)

* `AudioBuffers` now properly handles null buffers
* Fixed issue with non Xcode build (Xcode generator is only required for audio unit building) and allow building with CLion



## [2.1.1 | 2018-10-06](https://github.com/pongasoft/jamba/tree/v2.1.1)

* Fixed #4: Generate AU Plugin Manufacturer code from name instead of company



## [2.1.0 | 2018-10-06](https://github.com/pongasoft/jamba/tree/v2.1.0)

* Implemented #1: _Eliminate resource declaration duplication_
* Implemented #2: _Optionally generate audio unit wrapper_
* Implemented #3: _Make VST2 support truly optional_
* Upgraded to cmake 3.12
* Consolidated dev scripts into a single `jamba.sh` (resp `jamba.bat`) script
* Uses `Xcode` as the cmake generator on macOS



## [2.0.2 | 2018-09-02](https://github.com/pongasoft/jamba/tree/v2.0.2)

* Updated license terms



## [2.0.1 | 2018-08-28](https://github.com/pongasoft/jamba/tree/v2.0.1)

* Added ability to create a jamba enabled blank plugin: the plugin simply copies the input to the output and offers all the capabilities of jamba (build, test, edit, validate, install, etc...).



## [2.0.0 | 2018-08-25](https://github.com/pongasoft/jamba/tree/v2.0.0)

* Introduced Jamba parameters to handle non VST parameters and messaging RT <-> GUI
* Added `Debug::ParamTable` and `Debug::ParamLine` to display parameters => example

```text
    | ID   | TITLE      | TYP | OW | TRS | SHA | DEF.N | DEF.S          | STP | FLG   | SHORT  | PRE | UID | UNS |
    --------------------------------------------------------------------------------------------------------------
    | 1000 | Bypass     | vst | rt |     |     | 0.000 | Off            | 1   | 65537 | Bypass | 4   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 2010 | Left Gain  | vst | rt |     |     | 0.700 | +0.00dB        | 0   | 1     | GainL  | 2   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 2011 | Right Gain | vst | rt |     |     | 0.700 | +0.00dB        | 0   | 1     | GainR  | 2   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 2012 | Link       | vst | ui |     |     | 1.000 | On             | 1   | 1     | Link   | 4   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 2020 | Reset Max  | vst | rt |     |     | 0.000 | Off            | 1   | 1     | Reset  | 4   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 2000 | VuPPM      | vst | rt | x   |     | 0.000 | 0.0000         | 0   | 1     | VuPPM  | 4   | 0   |     |
    --------------------------------------------------------------------------------------------------------------
    | 3000 | Stats      | jmb | rt | x   | x   |       | -oo            |     |       |        |     |     |     |
    --------------------------------------------------------------------------------------------------------------
    | 2030 | Input Text | jmb | ui |     |     |       | Hello from GUI |     |       |        |     |     |     |
    --------------------------------------------------------------------------------------------------------------
    | 3010 | UIMessage  | jmb | ui | x   | x   |       |                |     |       |        |     |     |     |
    --------------------------------------------------------------------------------------------------------------
```

* Implemented lock free, memory allocation free and copy free version of SingleElementQueue and AtomicValue
* Generate build, test, validate, edit and install scripts
* Added [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) documentation project



## [1.0.0 | 2018-08-05](https://github.com/pongasoft/jamba/tree/v1.0.0)

* first public release / free / open source
