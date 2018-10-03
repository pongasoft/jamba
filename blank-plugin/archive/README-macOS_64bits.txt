Install instructions
====================

macOS (64 bits)
---------------
For VST2, copy [-filename-].vst3 and RENAME into [-filename-].vst under
  - $HOME/Library/Audio/Plug-ins/VST for a single user
  - or /Library/Audio/Plug-ins/VST for all users (may require admin access)
  - or any DAW specific path (64bits)
MAKE SURE TO RENAME the file otherwise it will not work

For VST3, copy [-filename-].vst3 under
  - $HOME/Library/Audio/Plug-ins/VST3 for a single user
  - or /Library/Audio/Plug-ins/VST3 for all users (may require admin access)
  - or any DAW specific path (64bits)

For Audio Unit, copy [-filename-].component under
  - $HOME/Library/Audio/Plug-ins/Components for a single user
  - or /Library/Audio/Plug-ins/Components for all users (may require admin access)
  - Note: you may have to reboot your computer for the Audio Unit to appear in Logic
          (or kill the AudioComponentRegistrar process(es))