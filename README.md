Jamba - A lightweight VST2/3 framework
======================================

Jamba is a set of helpers (classes, concepts, build files, etc...) built on top of the VST SDK to provide a lightweight framework to build a VST2/3 plugin.

Features
--------

- by including `jamba/jamba.cmake` in your project you gain access to the following
    - include all the necessary files so that your project depends on the VST SDK (including support for VST2)
    - easily write unit tests for your project (using googletest)
    - build on macOS and Windows 10 (Linux untested but support could be added)
    - package your project as an archive (zip file) for production release
    - scripts to build, test, validate, edit and install the plugin from the command line
- pick and choose which feature is useful to you
- define your VST parameters in one location (inherit from `Parameters`) and use both in real time processing (RT) and GUI code
- define typed VST parameters (ex: a boolean (`bool`) parameter, a gain (`Gain`) parameter, etc...)
- use typed VST parameters directly in RT processing code as well as GUI code
- use Jamba parameters for handling non VST parameters (ex: a label/text string cannot be represented by a VST parameters). Jamba parameters can be automatically saved/restored part of the state
- use Jamba parameters to easily exchange messages between RT and GUI (both directions) in a thread safe fashion
- easily create custom views with their creators (so that they appear in the GUI editor)
- easily use multiple parameters in a custom view (ex: a custom display which displays a gain value (parameter 1) in a color depending on parameter 2)
- store/read state in a thread safe fashion (includes version)
- UI views: toggle button and momentary button with 2 or 4 frames, Text Edit (with input saved part of the state)

History
-------
This project was created to abstract the common patterns and solutions to many questions implemented across the VAC-6V and A/B Switch vst plugins.

Documentation
-------------
The [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) project was designed as a documentation plugin for this project.

Configuration and requirements
------------------------------
This project is known to work on macOS High Siera 10.13.3 with Xcode 9.2 installed. It also has been tested on Windows 10 64 bits and Visual Studio Build tools (2017). It requires `cmake` version 3.9 at the minimum. Because it uses `cmake` it should work on other platforms but it has not been tested.

Downloading the SDK
-------------------
You need to download the VST3 SDK version 3.6.9 from [steinberg](https://download.steinberg.net/sdk_downloads/vstsdk369_01_03_2018_build_132.zip) (shasum 256 => `7c6c2a5f0bcbf8a7a0d6a42b782f0d3c00ec8eafa4226bbf2f5554e8cd764964`). Note that 3.6.10 was released in June 2018 but at this time, this project uses 3.6.9.

Installing the SDK
-------------------
Unpack the SDK to the following location (note how I renamed it with the version number)

* `/Users/Shared/Steinberg/VST_SDK.369` for macOS
* `C:\Users\Public\Documents\Steinberg\VST_SDK.369` for windows.

You can also store in a different location and use the `VST3_SDK_ROOT` variable when using cmake to define its location.

Configuring the SDK
-------------------
In order to build both VST2 and VST3 at the same time, you need to run the following commands

    # for macOS
    cd /Users/Shared/Steinberg/VST_SDK.369
    ./copy_vst2_to_vst3_sdk.sh

    # for Windows
    cd C:\Users\Public\Documents\Steinberg\VST_SDK.369
    copy_vst2_to_vst3_sdk.bat

Using the framework
-------------------

- After installing/configuring the sdk, simply include `jamba.cmake` in your project.
- Inherit from `Parameters` to define your parameters
- Inherit from `RTProcessor` to implement the RT (real time) business logic
- Inherit from `GUIController` to setup the GUI 

Check [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) for a good starting point.

2018-08-25 - Status for tag `v2.0.0`
------------------------------------
* Introduced Jamba parameters to handle non VST parameters and messaging RT <-> GUI
* Added Debug::ParamTable and Debug::ParamLine to display parameters => example

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

* Implemented lock free, memory allocation free and copy free version of SingleElementQueue and AtomicValue
* Generate build, test, validate, edit and install scripts
* Added [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) documentation project

2018-08-05 - Status for tag `v1.0.0`
------------------------------------
* first public release / free / open source

Misc
----
- This project uses [loguru](https://github.com/emilk/loguru) for logging (included under `src/cpp/pongasoft/logging`)

Licensing
---------
GPL version 3
