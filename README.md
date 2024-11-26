Jamba - A lightweight VST3 framework
======================================

Jamba is a set of helpers (classes, concepts, build files, etc...) built on top of the VST SDK to provide a lightweight framework to build a VST3 plugin. Jamba has been designed to help in building VST3 plugin, not to replace it: you are still writing a VST3 plugin, not a Jamba plugin.

Check the [Jamba](https://jamba.dev/) website for more details.

[![Run Tests (macOS)](https://github.com/pongasoft/jamba/actions/workflows/run-tests-macos-action.yml/badge.svg)](https://github.com/pongasoft/jamba/actions/workflows/run-tests-macos-action.yml) [![Run Tests (Windows)](https://github.com/pongasoft/jamba/actions/workflows/run-tests-windows-action.yml/badge.svg)](https://github.com/pongasoft/jamba/actions/workflows/run-tests-windows-action.yml)

Features
--------

### Bootstrap

- generate a fully buildable, testable, editable and deployable plugin (see [Quick Starting Guide](https://jamba.dev/quickstart/))
- automatically download dependencies

### Build & Deploy

- simple script to build, test, validate, edit and install the plugin from the command line (see [jamba.sh (.bat)](https://jamba.dev/jamba.sh/)
- build a **self-contained** plugin that depends on the VST3 SDK
- optionally wraps the VST3 plugin into an Audio Unit plugin (macOS)
- build a universal plugin for Apple Silicon support (macOS)  
- easily adds resources (images) in a cross-platform way
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
- store/read state in a thread safe fashion (includes version to allow easy state upgrade)
- [Jamba Views](https://jamba.dev/views/)

Getting Started
---------------

Check the [Quickstart](https://jamba.dev/quickstart/) section to getting started.

Latest Release
--------------

7.1.2 | 2024-11-26 | VST SDK 3.7.12+

[Release notes (local)](RELEASE.md)

[Release notes (web)](https://jamba.dev/releases/)

Development
-----------

In order to work on this project itself (not a plugin), you need to clone this repository and install the VST3 SDK as described in the [requirements](https://jamba.dev/requirements/) section. In addition to the unit tests (under `src/test`), the embedded `jamba-test-plugin` plugin constitutes the main source of testing.

Although this project does not contain a great deal of tests, it is being heavily used (and as a result tested) by the plugins it has been designed to build.

| Plugin                                                              | Description                                                                                    |
|---------------------------------------------------------------------|------------------------------------------------------------------------------------------------|
| [vst-ab-switch](https://github.com/pongasoft/vst-ab-switch)         | A/B Audio Switch VST plugin                                                                    |
| [vst-vac-6](https://github.com/pongasoft/vst-vac-6v)                | VST (2 & 3) plugin to analyze and control the peak volume of a signal                          |
| [vst-sam-spl-64](https://github.com/pongasoft/vst-sam-spl-64)       | A free/open source VST2, VST3 and AudioUnit plugin to easily split a sample in up to 64 slices |
| [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) | Documentation plugin for Jamba framework: a simple gain plugin for VST3                        |

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
