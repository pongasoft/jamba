#!/usr/bin/env python3

# Copyright (c) 2020 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

import argparse
import os
import sys
import platform

parser = argparse.ArgumentParser(allow_abbrev=False,
                                 usage='configure.py [--h] [--n] [--f] [--r] [---vst3 VST3_SDK_ROOT] [---vst2 VST2_SDK_ROOT] [--G GENERATOR] [--B BUILD_DIR] [--- <cmake_options>]',
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 epilog='''
Notes
  ---vst3 defaults to /Users/Shared/Steinberg/VST_SDK.<JAMBA_VST3SDK_VERSION>
  ---vst2 defaults to /Users/Shared/Steinberg/VST_SDK.<JAMBA_VST2SDK_VERSION>
  
  -G defaults to "Xcode" on macOS and "Visual Studio 16 2019" for Windows10
  run 'cmake --help' to get the list of generators supported
  
  For single-config generators, Debug is used by default and can be changed with -r for Release
  For multi-config generators, -r is ignored
   
  To provide extra options to CMake you do it this way
  python3 configure.py -- -Wdev

Examples
  # Specify an explicit path to the VST3 sdk and uses default generator
  python3 configure.py ---vst3 /opt/local/VST_SDK.3.7.0

  # Use default paths and uses another generator
  python3 configure.py -G "CodeBlocks - Unix Makefiles"

  # Use defaults
  python3 configure.py
''')
parser.add_argument("-n", "--dry-run", help="Dry run (prints what it is going to do)", action="store_true", dest="dry_run")
parser.add_argument("-f", "--force", help="Force a regeneration (delete and recreate build folder)", action="store_true")
parser.add_argument("--vst3", help="Path to the VST3 SDK (optional)", dest="vst3_sdk_root")
parser.add_argument("--vst2", help="Path to the VST2 SDK (optional)", dest="vst2_sdk_root")
parser.add_argument("-r", "--release", help="Use CMake Release build type (for single-config generators)", action="store_true")
parser.add_argument("-G", "--generator", help="CMake generator (optional)")
parser.add_argument("-B", "--build-dir", help="Build folder (defaults to ./build)", dest="build_dir")
parser.add_argument('cmake_options', help="Any options for cmake", nargs=argparse.REMAINDER)

args = parser.parse_args()

# The CMakeLists.txt file is a sibling of this script
this_script_root_dir = os.path.dirname(os.path.realpath(sys.argv[0]))

# VST3_SDK_ROOT
vst3_sdk_root = [f'-DVST3_SDK_ROOT:PATH={args.vst3_sdk_root}'] if args.vst3_sdk_root else []

# VST2_SDK_ROOT
vst2_sdk_root = [f'-DVST2_SDK_ROOT:PATH={args.vst2_sdk_root}'] if args.vst2_sdk_root else []

# CMake generator
cmake_generator = ['-G']
if args.generator:
    cmake_generator.append(args.generator)
else:
    if platform.system() == 'Darwin':
        cmake_generator.append('Xcode')
    else:
        cmake_generator.append('Visual Studio 16 2019')

# CMake options
cmake_options = [] if not args.cmake_options else args.cmake_options[1:]

# CMake build type (for single config generators)
cmake_build_type = [f'-DCMAKE_BUILD_TYPE={"Release" if args.release else "Debug"}']

# CMake build directory
build_dir = args.build_dir if args.build_dir else 'build'
cmake_build_dir = ['-B', build_dir]

# CMake command
cmake_command = ['cmake',
                 *cmake_build_dir,
                 *vst3_sdk_root, *vst2_sdk_root,
                 *cmake_build_type,
                 *cmake_generator,
                 *cmake_options,
                 this_script_root_dir]

if args.dry_run:
    escaped_command = ' '.join([f'"{x}"' for x in cmake_command[1:]])
    print(f'cmake {escaped_command}')
else:
    if args.force:
        import shutil
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)

    import subprocess
    subprocess.run(cmake_command)





