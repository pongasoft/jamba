Install instructions
====================

macOS (64 bits)
---------------
For a single user, install under $HOME/Library/Audio/Plug-ins
For all users, install under /Library/Audio/Plug-ins (may require admin access)

The directory structure in this archive matches the file system, so you can simply
copy it under $HOME/Library/Audio/Plug-ins (or /Library/Audio/Plug-ins)

Or if you prefer to install each file individually:

For VST3, copy VST3/[-filename-].vst3 under
  - $HOME/Library/Audio/Plug-ins/VST3 for a single user
  - or /Library/Audio/Plug-ins/VST3 for all users (may require admin access)
  - or any DAW specific path (64bits)

For Audio Unit, copy Components/[-filename-].component under $HOME/Library/Audio/Plug-ins/Components
  - $HOME/Library/Audio/Plug-ins/Components for a single user
  - or /Library/Audio/Plug-ins/Components for all users (may require admin access)
  - Note: you may have to reboot your computer for the Audio Unit to appear in Logic
          (or kill the AudioComponentRegistrar process(es))