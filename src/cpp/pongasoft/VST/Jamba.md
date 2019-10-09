What is Jamba? {#mainpage}
--------------
Jamba is a set of helpers (classes, concepts, build files, etc...) built on top of the VST SDK to provide a lightweight framework to build a VST2/3 plugin. Jamba has been designed to help in building VST2/3 plugin, not to replace it: you are still writing a VST2/3 plugin, not a Jamba plugin.

Jamba is a free and open source project hosted on [github](https://github.com/pongasoft/jamba).

The Jamba main [website](https://jamba.dev) contains more information about the project.

Features
--------

### Bootstrap

- generate a fully buildable, testable, editable and deployable plugin (see [Quick Starting Guide](https://jamba.dev/quickstart/))
- automatically download dependencies

### Build & Deploy

- simple script to build, test, validate, edit and install the plugin from the command line (see [jamba.sh (.bat)](https://jamba.dev/jamba.sh/))
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

Quick documentation links
-------------------------

### pongasoft::VST::Parameters

VST parameters are what the host DAW deal with (for example a knob is usually tied to a VST parameter).
Both the GUI and the RT (Real Time) are "sharing" those parameters. As a result Jamba makes them front and center:
you define them in one place and use them in the GUI and RT.

Note: the Jamba framework introduces a new kind of parameters that go beyond VST parameters
(which are limited to a double value in the range [0.0, 1.0]) called Jamba parameters (Jmb).

Start by inhering from this class to register your own parameters.

### pongasoft::VST::RT::RTState

After declaring your parameters, you should inherit from RTState which is the class that is being used from the
RT (real time) processor.

### pongasoft::VST::RT::RTProcessor

The Real Time (RT) processing code is where the main logic of the plugin resides. The DAW repeatedly calls the
process method (actually pongasoft::VST::RT::RTProcessor::processInputs32Bits or
pongasoft::VST::RT::RTProcessor::processInputs64Bits in Jamba) to process a batch of samples.
This is usually called a "frame". The processor uses the pongasoft::VST::RT::RTState class.
You simply need to inherit from pongasoft::VST::RT::RTProcessor.

### pongasoft::VST::GUI::GUIState

After declaring your parameters, you should inherit from pongasoft::VST::GUI::GUIState which is the class that is being used in the UI
(only necessary for Jmb parameters as Vst parameters are direcly accessible in the UI)

### pongasoft::VST::GUI::GUIController

The entry point of the GUI is the GUI controller. Jamba takes care of most of the details of the implementation
for you: you simply need to inherit from pongasoft::VST::GUI::GUIController. The controller uses the
pongasoft::VST::GUI::GUIState class and makes it available to all the views).
/