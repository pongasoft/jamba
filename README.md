Jamba - A lightweight VST2/3 framework
======================================

Jamba is a set of helpers (classes, concepts, build files, etc...) built on top of the VST SDK to provide a lightweight framework to build a VST2/3 plugin. Jamba has been designed to help in building VST2/3 plugin, not to replace it: you are still writing a VST2/3 plugin, not a Jamba plugin.

Check the [Jamba](https://jamba.dev/) website for more details.

Features
--------

### Bootstrap

- generate a fully buildable, testable, editable and deployable plugin (see [Quick Starting Guide](https://jamba.dev/quickstart/))
- automatically download dependencies

### Build & Deploy

- simple script to build, test, validate, edit and install the plugin from the command line (see [jamba.sh (.bat)](https://jamba.dev/jamba.sh/)
- build a **self contained** plugin that depends on the VST3 SDK
- optionally builds a VST2 compatible plugin
- optionally wraps the VST3 plugin into an Audio Unit plugin (macOS)
- easily adds resources (images) in a cross platform way
- easily write unit tests for your project
- build on macOS and Windows 10
- package your project as an archive (zip file) for production release


### C++ classes & concepts
- pick and choose which feature is useful to you (lots of options to override/change the default behavior)
- define your VST parameters in one location (inherit from `Parameters`) and use both in real time processing (RT) and GUI code
- define typed VST parameters (ex: a boolean (`bool`) parameter, a gain (`Gain`) parameter, etc...)
- use typed VST parameters directly in RT processing code as well as GUI code
- use Jamba parameters for handling non VST parameters (ex: a label/text string cannot be represented by a VST parameters). Jamba parameters can be automatically saved/restored part of the state
- use Jamba parameters to easily exchange messages between RT and GUI (both directions) in a thread safe fashion
- easily create custom views with their creators (so that they appear in the GUI editor)
- easily use multiple parameters in a custom view (ex: a custom display which displays a gain value (parameter 1) in a color depending on parameter 2)
- store/read state in a thread safe fashion (includes version)
- [Jamba Views](https://jamba.dev/views/)

Getting Started
---------------

Check the [Quickstart](https://jamba.dev/quickstart/) section to getting started.

Latest Release Notes
--------------------
### 2019-08-03 - `v3.2.4`
* fixed blank plugin `initialize` API to match superclass

### 2019-08-01 - `v3.2.3`
* Implemented [#6](https://github.com/pongasoft/jamba/issues/6): added cmake targets `build_vst3`, `test_vst3`, `install_vst3`, `install_vst2`, `build_au` and `install_au`
* This change allows those targets to be used directly in an IDE and for example use them for debugging (ex: attach a DAW executable to `install_vst2` or `install_vst3`)
* Modified `jamba.sh` and `jamba.bat` to use the new cmake targets (making the scripts more generic)
* Added ability to customize (extend or completely override) the following targets:
  * `build_vst3` by setting an optional `BUILD_VST3_TARGET` variable
  * `test_vst3` by setting an optional `TEST_VST3_TARGET` variable
  * `build_au` by setting an optional `BUILD_AU_TARGET` variable

### 2019-07-23 - `v3.2.2`
* released [Jamba](https://jamba.dev/) website with [Quickstart](https://jamba.dev/quickstart/) section to make it very easy to generate a blank plugin (answer a few questions in the browser)
* added (optional) VST3 SDK automatic download to the blank plugin (web and command line)
* fixed blank plugin creation issue when no company was provided

### 2019-05-19 - `v3.2.1`
* fixed `jamba.sh` script to handle filename with white spaces and install audio unit before validation (validation tool unfortunately works from an installed component, not a file...)

### 2019-05-19 - `v3.2.0`
* added audio unit validation to `jamba.sh` script (`jamba.sh validate-au`) which runs the `auvaltool` utility on the audio unit plugin
* added switch container view to handle tab like switching (check `Views::SwitchContainerView`)
* added (optional) image for text buttons (`Views::TextButtonView`)
* added (optional) disabled state for momentary buttons (`Views::MomentaryButtonView`)
* added discrete button view (a button which is "on" only when the backing discrete parameter matches the `step` value). This button can be used for radio groups, tabs, etc... (check `Views::DiscreteButtonView`)
* Note: There is a small potentially breaking API change with function `Views::createCustomView` (defined in `Views/CustomViewCreator.h`). Check [commit](https://github.com/pongasoft/jamba/commit/08d06ceda9b936b92c45b8bd9f22a93acfeb3995). The function takes an additional 2 parameters which can be safely ignored if you don't need them.

[All release notes](https://jamba.dev/releases/)

History
-------
This project was created to abstract the common patterns and solutions to many questions implemented across the VAC-6V and A/B Switch vst plugins. Check [Announcing Jamba](https://www.pongasoft.com/blog/yan/vst/2018/08/29/Announcing-Jamba/) blog post for more details.

Misc
----

- This project uses [loguru](https://github.com/emilk/loguru) for logging (included under `src/cpp/pongasoft/logging`)
- This project uses the VST3 SDK by Steinberg Media Technologies GmbH

Licensing
---------

- Apache 2.0 License. Jamba itself can be used according to the terms of the Apache 2.0 license.

- Note that the primary goal of Jamba is to help build a VST3 plugin and as a result you should check the [Steinberg License](http://www.steinberg.net/sdklicenses_vst3) in order to determine under which terms your plugin needs to be licensed.
