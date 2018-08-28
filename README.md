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
- [Generates](#generating-a-blank-plugin) a fully buildable, testable, editable plugin with a simple command

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


Generating a blank plugin
-------------------------

- In order to quickly create a jamba enabled blank plugin (simply copy input to output), clone this project and run the following command (requires python3):

        python3 create-plugin.py

- Follow the prompt (example run)

        > cd /tmp
        > python3 /Volumes/Development/github/org.pongasoft/jamba/create-plugin.py
        Plugin Name (must be a valid C++ class name) = Kooza
        Filename (leave empty for default [Kooza]) = 
        Company (leave empty for default [acme]) = pongasoft
        Company URL (leave empty for default [https://www.pongasoft.com]) = 
        Company Email (leave empty for default [support@pongasoft.com]) = 
        C++ namespace (leave empty for default [pongasoft::VST::Kooza]) = 
        Project directory (leave empty for default [/private/tmp]) = 
        Project Name (leave empty for default [pongasoft-Kooza-plugin]) = 
        ##################
        Plugin Name     - Kooza
        Filename        - Kooza (will generate Kooza.vst3)
        Company         - pongasoft
        Company URL     - https://www.pongasoft.com
        Company Email   - support@pongasoft.com
        Jamba git hash  - v2.0.1
        C++ Namespace   - pongasoft::VST::Kooza
        Plugin root dir - /private/tmp/pongasoft-Kooza-plugin

        Are you sure (Y/n)?
        Generating Kooza plugin....
        Kooza plugin generated under /private/tmp/pongasoft-Kooza-plugin
        You can now configure the plugin:

        For macOs:
        ----------
        ### configuring

        cd <build_folder>
        /private/tmp/pongasoft-Kooza-plugin/configure.sh Debug

        ### building and testing
        cd <build_folder>/build/Debug
        ./build.sh # to build
        ./test.sh # to run the tests
        ./validate.sh # to validate the plugin (VST3)
        ./edit.sh # to run the editor and edit the UI
        ./install.sh # to install locally

        For Windows 10:
        ---------------
        ### configuring

        cd <build_folder>
        /private/tmp/pongasoft-Kooza-plugin/configure.bat

        ### building and testing
        cd <build_folder>/build
        ./build.bat # to build (Debug mode)
        ./test.bat # to run the tests (Debug mode)
        ./validate.bat # to validate the plugin (VST3) in Debug mode
        ./edit.bat # to run the editor and edit the UI

Check [jamba-sample-gain](https://github.com/pongasoft/jamba-sample-gain) for documentation and explanation of the concepts.

Release Notes
-------------

### 2018-08-28 - `v2.0.1`
* Added ability to create a jamba enabled blank plugin: the plugin simply copies the input to the output and offers all the capabilities of jamba (build, test, edit, validate, install, etc...).

### 2018-08-25 - `v2.0.0`
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

### 2018-08-05 - `v1.0.0`
* first public release / free / open source

Misc
----
- This project uses [loguru](https://github.com/emilk/loguru) for logging (included under `src/cpp/pongasoft/logging`)

Licensing
---------
GPL version 3
