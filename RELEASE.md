Release Notes {#release-notes}
-------------

## [2019-10-28 - v4.0.0](https://github.com/pongasoft/jamba/tree/v4.0.0)

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
* Major renaming: introduced `ParamAware` (formerly `GUIParamCxAware`) and `StateAware` (formerly `PluginAccessor`) (added deprecation warnings 
  for old names) and related.
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
* `StepButtonView` and `DiscreteButtonView` now use discrete values only (no longer `double`). Check the documentation
  on how to use it instead (ex: an increment of `0.1` can be converted to an increment of `1` 
  with a step count of `10`).
* Changed `TagID` to be an unsigned int (`ParamID` aka `uint32`)
* Changed default template type for `DiscreteValueParamConverter` to be an `int32`
* Although care has been taken with renaming/adding deprecation warnings for old names, there is a chance (mostly 
  with templated parameters) that it may impact compilation.

## [2019-08-03 - v3.2.4](https://github.com/pongasoft/jamba/tree/v3.2.4)

* fixed blank plugin `initialize` API to match superclass



## [2019-08-01 - v3.2.3](https://github.com/pongasoft/jamba/tree/v3.2.3)

* Implemented [#6](https://github.com/pongasoft/jamba/issues/6): added cmake targets `build_vst3`, `test_vst3`, `install_vst3`, `install_vst2`, `build_au` and `install_au`
* This change allows those targets to be used directly in an IDE and for example use them for debugging (ex: attach a DAW executable to `install_vst2` or `install_vst3`)
* Modified `jamba.sh` and `jamba.bat` to use the new cmake targets (making the scripts more generic)
* Added ability to customize (extend or completely override) the following targets:
  * `build_vst3` by setting an optional `BUILD_VST3_TARGET` variable
  * `test_vst3` by setting an optional `TEST_VST3_TARGET` variable
  * `build_au` by setting an optional `BUILD_AU_TARGET` variable



## [2019-07-23 - v3.2.2](https://github.com/pongasoft/jamba/tree/v3.2.2)

* released [Jamba](https://jamba.dev/) website with [Quickstart](https://jamba.dev/quickstart/) section to make it very easy to generate a blank plugin (answer a few questions in the browser)
* added (optional) VST3 SDK automatic download to the blank plugin (web and command line)
* fixed blank plugin creation issue when no company was provided



## [2019-05-19 - v3.2.1](https://github.com/pongasoft/jamba/tree/v3.2.1)

* fixed `jamba.sh` script to handle filename with white spaces and install audio unit before validation (validation tool unfortunately works from an installed component, not a file...)



## [2019-05-19 - v3.2.0](https://github.com/pongasoft/jamba/tree/v3.2.0)

* added audio unit validation to `jamba.sh` script (`jamba.sh validate-au`) which runs the `auvaltool` utility on the audio unit plugin
* added switch container view to handle tab like switching (check `Views::SwitchContainerView`)
* added (optional) image for text buttons (`Views::TextButtonView`)
* added (optional) disabled state for momentary buttons (`Views::MomentaryButtonView`)
* added discrete button view (a button which is "on" only when the backing discrete parameter matches the `step` value). This button can be used for radio groups, tabs, etc... (check `Views::DiscreteButtonView`)

<h3 class="release-note notice warning">WARNING</h3>

#### The following changes are potentially breaking changes

* There is a small potentially breaking API change with function `Views::createCustomView` (defined in `Views/CustomViewCreator.h`). Check [commit](https://github.com/pongasoft/jamba/commit/08d06ceda9b936b92c45b8bd9f22a93acfeb3995). The function takes an additional 2 parameters which can be safely ignored if you don't need them.

## [2019-02-10 - v3.1.0](https://github.com/pongasoft/jamba/tree/v3.1.0)

* added ability to conveniently register a global key hook (check `Views::registerGlobalKeyboardHook`)
* parameters now copy the provided strings (title, short tile and units) so that they no longer have to be static and for example can be computed
* added convenient methods to `AudioBuffers` and `Channel` (`copyFrom` and `forEachSample`). Defined `const` and non `const` versions.
* added Step Button view which is a button that increment/decrement a parameter by a fixed amount when clicked. Another amount (tied to shift click) can be provided.



## [2019-01-06 - v3.0.2](https://github.com/pongasoft/jamba/tree/v3.0.2)

* fixed minor bugs



## [2019-01-04 - v3.0.1](https://github.com/pongasoft/jamba/tree/v3.0.1)

* Use shallow cloning to avoid downloading the (unused) project history



## [2019-01-03 - v3.0.0](https://github.com/pongasoft/jamba/tree/v3.0.0)

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


## [2018-10-11 - v2.1.2](https://github.com/pongasoft/jamba/tree/v2.1.2)

* `AudioBuffers` now properly handles null buffers
* Fixed issue with non Xcode build (Xcode generator is only required for audio unit building) and allow building with CLion



## [2018-10-06 - v2.1.1](https://github.com/pongasoft/jamba/tree/v2.1.1)

* Fixed #4: Generate AU Plugin Manufacturer code from name instead of company



## [2018-10-06 - v2.1.0](https://github.com/pongasoft/jamba/tree/v2.1.0)

* Implemented #1: _Eliminate resource declaration duplication_
* Implemented #2: _Optionally generate audio unit wrapper_
* Implemented #3: _Make VST2 support truly optional_
* Upgraded to cmake 3.12
* Consolidated dev scripts into a single `jamba.sh` (resp `jamba.bat`) script
* Uses `Xcode` as the cmake generator on macOS



## [2018-09-02 - v2.0.2](https://github.com/pongasoft/jamba/tree/v2.0.2)

* Updated license terms



## [2018-08-28 - v2.0.1](https://github.com/pongasoft/jamba/tree/v2.0.1)

* Added ability to create a jamba enabled blank plugin: the plugin simply copies the input to the output and offers all the capabilities of jamba (build, test, edit, validate, install, etc...).



## [2018-08-25 - v2.0.0](https://github.com/pongasoft/jamba/tree/v2.0.0)

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



## [2018-08-05 - v1.0.0](https://github.com/pongasoft/jamba/tree/v1.0.0)

* first public release / free / open source
