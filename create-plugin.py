#!/usr/bin/env python3

# Copyright (c) 2018-2021 pongasoft
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
parser.add_argument("-c", "--aucode", help="The 4 characters for the audio unit manufacturer code")
parser.add_argument("-x", "--osx", help="The macOS deployment target")
parser.add_argument("-u", "--url", help="The url for the company")
parser.add_argument("-e", "--email", help="The email for the company")
parser.add_argument("-n", "--namespace", help="The C++ namespace")
parser.add_argument("-d", "--directory", help="The directory for the project")
parser.add_argument("-j", "--project", help="The name of the project")
parser.add_argument("-g", "--jambagithash", help="The git hash for jamba")
parser.add_argument("-s", "--downloadsdk", help="Download the VST SDK", action="store_true")
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
    plugin['macos_deployment_target'] = maybe_ask_user(args.osx, "macOS deployment target", "10.14")
    plugin['audio_unit_manufacturer_code'] = \
        maybe_ask_user(args.aucode,
                       "Audio Unit manufacturer code (4 chars / one capital letter)",
                       plugin['name'][0:4].capitalize())[0:4] if plugin['enable_audio_unit'] == "ON" else "N/A"
    plugin['filename'] = maybe_ask_user(args.filename, "Filename", plugin['name'])
    plugin['company'] = maybe_ask_user(args.company, "Company", 'acme')
    plugin['company_url'] = maybe_ask_user(args.url, "Company URL", f'https://www.{plugin["company"]}.com')
    plugin['company_email'] = maybe_ask_user(args.email, "Company Email", f'support@{plugin["company"]}.com')
    plugin['namespace'] = maybe_ask_user(args.namespace, "C++ namespace", f"{plugin['company']}::VST::{plugin['name']}")
    plugin['download_vst_sdk'] = "ON" if maybe_confirm_user(args.downloadsdk, "Download VST SDK") else "OFF"

    project_dir = maybe_ask_user(args.directory, "Project directory", os.path.realpath(os.curdir))
    project_name = maybe_ask_user(args.project, "Project Name", f'{plugin["company"]}-{plugin["name"]}-plugin')

    plugin['root_dir'] = os.path.join(os.path.realpath(project_dir), project_name)
    plugin['jamba_root_dir'] = this_script_root_dir.replace('\\', '\\\\')
    plugin['local_jamba'] = "" if args.local else "#"
    plugin['remote_jamba'] = "" if not args.local else "#"
    plugin['target'] = f'{plugin["company"]}_{plugin["name"]}' if plugin["company"] else plugin["name"]

    print(f'''##################
Plugin Name        - {plugin["name"]}
VST2 Enabled       - {plugin["enable_vst2"]}
macOS Target       - {plugin["macos_deployment_target"]}
Audio Unit Enabled - {plugin["enable_audio_unit"]}
AU Plugin Code     - {plugin["audio_unit_manufacturer_code"]}
Filename           - {plugin["filename"]} (will generate {plugin["filename"]}.vst3)
Company            - {plugin["company"]}
Company URL        - {plugin["company_url"]}
Company Email      - {plugin["company_email"]}
Jamba git hash     - {plugin["jamba_git_hash"]}
C++ Namespace      - {plugin["namespace"]}
Dowload VST SDK    - {plugin["download_vst_sdk"]}
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


plugin['processor_uuid'] = generate_uuid()
plugin['controller_uuid'] = generate_uuid()
plugin['debug_processor_uuid'] = generate_uuid()
plugin['debug_controller_uuid'] = generate_uuid()
plugin['year'] = datetime.datetime.now().year

ignoredFiles = ['.DS_Store']

if plugin['enable_audio_unit'] != "ON":
    ignoredFiles += ["audio-unit"]

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
{plugin['root_dir']}/configure.py

### building, testing, etc...
cd <build_folder>/build
./jamba.py -h

For Windows 10:
---------------
### configuring

cd <build_folder>
python {plugin['root_dir']}/configure.py

### building, testing, etc... (PowerShell recommended)
cd <build_folder>/build
python ./jamba.py -h''')
