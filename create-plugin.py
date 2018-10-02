#!/usr/bin/env python3

# Copyright (c) 2018 pongasoft
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

import sys
import os
from string import Template
import uuid
import datetime
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--plugin", help="The name of the plugin")
parser.add_argument("-2", "--vst2", help="Enable VST2", action="store_true")
parser.add_argument("-a", "--au", help="Enable Audio Unit", action="store_true")
parser.add_argument("-l", "--local", help="Use local jamba", action="store_true")
parser.add_argument("-f", "--filename", help="The filename of the plugin")
parser.add_argument("-C", "--company", help="The name of the company")
parser.add_argument("-c", "--company4", help="The (4 characters) name of the company")
parser.add_argument("-u", "--url", help="The url for the company")
parser.add_argument("-e", "--email", help="The email for the company")
parser.add_argument("-n", "--namespace", help="The C++ namespace")
parser.add_argument("-d", "--directory", help="The directory for the project")
parser.add_argument("-j", "--project", help="The name of the project")
parser.add_argument("-g", "--jambagithash", help="The git hash for jamba")
args = parser.parse_args()


# ask_user
def ask_user(msg, default):
    s = input(msg).strip()
    if len(s) == 0:
        return default if default else ask_user(msg, default)
    return s


# maybe_ask_user
def maybe_ask_user(option, msg, default):
    if option:
        return option
    else:
        if default:
            return ask_user(f'{msg} (leave empty for default [{default}]) = ', default)
        else:
            return ask_user(f'{msg} = ', default)


# confirm_user
def confirm_user(msg):
    s = input(msg).strip()
    if len(s) == 0:
        return True
    return s.lower() == 'y'


# maybe_confirm_user
def maybe_confirm_user(option, msg):
    if option:
        return option
    else:
        return confirm_user(f'{msg} (Y/n)?')


this_script_root_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
blank_plugin_root = os.path.join(this_script_root_dir, 'blank-plugin')


# config
def config(plugin):
    plugin['name'] = maybe_ask_user(args.plugin, "Plugin Name (must be a valid C++ class name)", None)
    plugin['enable_vst2'] = "ON" if maybe_confirm_user(args.vst2, "Enable VST2") else "OFF"
    plugin['enable_audio_unit'] = "ON" if maybe_confirm_user(args.au, "Enable Audio Unit") else "OFF"
    plugin['filename'] = maybe_ask_user(args.filename, "Filename", plugin['name'])
    plugin['company'] = maybe_ask_user(args.company, "Company", 'acme')
    plugin['company_short_4'] = maybe_ask_user(args.company4,
                                               "Short Company Name (4 chars / one capital letter)",
                                               plugin['company'][0:4].capitalize())[0:4]
    plugin['company_url'] = maybe_ask_user(args.url, "Company URL", f'https://www.{plugin["company"]}.com')
    plugin['company_email'] = maybe_ask_user(args.email, "Company Email", f'support@{plugin["company"]}.com')
    plugin['namespace'] = maybe_ask_user(args.namespace, "C++ namespace", f"{plugin['company']}::VST::{plugin['name']}")

    project_dir = maybe_ask_user(args.directory, "Project directory", os.path.realpath(os.curdir))
    project_name = maybe_ask_user(args.project, "Project Name", f'{plugin["company"]}-{plugin["name"]}-plugin')

    plugin['root_dir'] = os.path.join(os.path.realpath(project_dir), project_name)
    plugin['jamba_root_dir'] = this_script_root_dir.replace('\\', '\\\\')
    plugin['local_jamba'] = "" if args.local else "#"

    print(f'''##################
Plugin Name        - {plugin["name"]}
VST2 Enabled       - {plugin["enable_vst2"]}
Audio Unit Enabled - {plugin["enable_audio_unit"]}
Filename           - {plugin["filename"]} (will generate {plugin["filename"]}.vst3)
Company            - {plugin["company"]}
Company (short)    - {plugin["company_short_4"]}
Company URL        - {plugin["company_url"]}
Company Email      - {plugin["company_email"]}
Jamba git hash     - {plugin["jamba_git_hash"]}
C++ Namespace      - {plugin["namespace"]}
Plugin root dir    - {plugin["root_dir"]}
''')
    confirm = input("Are you sure (Y/n)?")
    if len(confirm.strip()) == 0 or confirm.upper() == 'Y':
        return
    else:
        config(plugin)


plugin = dict()

if args.jambagithash:
    plugin['jamba_git_hash'] = args.jambagithash
else:
    jambaGitSha = subprocess.run(['git', '--no-pager', 'log', '-1', '--pretty=format:%H'], cwd=blank_plugin_root,
                                 capture_output=True, text=True).stdout
    plugin['jamba_git_hash'] = subprocess.run(['git', 'describe', '--tags', jambaGitSha], cwd=blank_plugin_root,
                                              capture_output=True, text=True).stdout.rstrip()

config(plugin)


# generate_uuid
def generate_uuid():
    a = uuid.uuid4().hex
    return f"0x{a[0:8]}, 0x{a[8:16]}, 0x{a[16:24]}, 0x{a[24:32]}"


plugin['namespace_start'] = '\n'.join(list(map(lambda x: f"namespace {x} {{", plugin['namespace'].split('::'))))
plugin['namespace_end'] = '\n'.join(list(map(lambda x: '}', plugin['namespace'].split('::'))))
plugin['processor_uuid'] = generate_uuid()
plugin['controller_uuid'] = generate_uuid()
plugin['year'] = datetime.datetime.now().year

ignoredFiles = ['.DS_Store']


# class Processor
class Processor(Template):
    delimiter = '[-'
    pattern = r'''
    \[-(?:
       (?P<escaped>-) |            # Expression [-- will become [-
       (?P<named>[^\[\]\n-]+)-\] | # -, [, ], and \n can't be used in names
       \b\B(?P<braced>) |          # Braced names disabled
       (?P<invalid>)               #
    )
    '''


# process_file
def process_file(in_file_path, out_file_path):
    with open(in_file_path, 'r') as in_file:
        content = in_file.read()
        content = Processor(content).substitute(plugin)
        with open(out_file_path, 'w') as out_file:
            out_file.write(content)
        os.chmod(out_file_path, os.stat(in_file_path).st_mode)


# process_dir
def process_dir(directory, parent):
    with os.scandir(directory) as it:
        for entry in it:
            if entry.name not in ignoredFiles:
                name = entry.name.replace('__Plugin__', plugin['name'])
                path = os.path.join(plugin['root_dir'], parent, name)
                if entry.is_dir():
                    os.makedirs(path, exist_ok=True)
                    process_dir(entry.path, os.path.join(parent, entry.name))
                if entry.is_file():
                    process_file(entry.path, path)


print(f"Generating {plugin['name']} plugin....")
os.makedirs(plugin['root_dir'], exist_ok=True)
process_dir(blank_plugin_root, '')
print(f"{plugin['name']} plugin generated under {plugin['root_dir']}")
print(f'''You can now configure the plugin:

For macOs:
----------
### configuring

cd <build_folder>
{plugin['root_dir']}/configure.sh Debug

### building and testing
cd <build_folder>/build/Debug
./build.sh    # to build
./test.sh     # to run the test
./validate.sh # to validate the plugin (VST3)
./edit.sh     # to run the editor and edit the UI
./install.sh  # to install locally
./archive.sh  # to build the zip archive

For Windows 10:
---------------
### configuring

cd <build_folder>
{plugin['root_dir']}/configure.bat

### building and testing
cd <build_folder>/build
./build.bat [Debug|Release]    # to build 
./test.bat [Debug|Release]     # to run the tests
./validate.bat [Debug|Release] # to validate the plugin (VST3)
./edit.bat                     # to run the editor and edit the UI (editor not available in Release mode)
./archive.bat [Debug|Release]  # to build the zip archive''')
