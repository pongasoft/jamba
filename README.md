Jamba - A lightweight VST2/3 framework
======================================

Jamba is a set of helpers (classes, concepts, build files, etc...) built on top of the VST SDK to provide a lightweight framework to build a VST2/3 plugin.

Features
--------

- by including `jamba/jamba.cmake` in your project you gain access to the following
    - include all the necessary files so that your project depends on the VST SDK (including support for VST2)
    - easily write unit tests for your project (using googletest)
    - build on macOS and Windows 10 (Linux untested but should work)
    - package your project as an archive (zip file) for production release
- pick and choose which feature is useful to you
- define your vst parameters in one location (inherit from `Parameters`) and use both in real time processing (RT) and GUI code
- define typed parameter (ex: a boolean (`bool`) parameter, a gain (`Gain`) parameter, etc...)
- use typed parameter directly in RT processing code as well as GUI code
- easily create custom views with their creators (so that they appear in the GUI editor)
- easily use multiple parameters in a custom view (ex: a custom display which displays a gain value (parameter 1) in a color depending on parameter 2)
- easily setup a UI timer to communicate between the RT processing and the GUI
- store/read state in a thread safe fashion (and handle versioning)
- use non VST parameters aka serializable parameters which can be automatically saved/restored part of the state (ex: a user input string)
- use a toggle button or a momentary button with 2 or 4 frames

History
-------
This project was created to abstract the common patterns and solutions to many questions implemented across the VAC-6V and A/B Switch vst plugins.

Documentation
-------------
This is TBD. At this moment the best documentation is the 2 plugins using the framework.

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

Misc
----
- This project uses [loguru](https://github.com/emilk/loguru) for logging (included under `src/cpp/pongasoft/logging`)

Licensing
---------
GPL version 3
