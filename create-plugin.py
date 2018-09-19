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
import re
import uuid
import datetime
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--plugin", help="The name of the plugin")
parser.add_argument("-f", "--filename", help="The filename of the plugin")
parser.add_argument("-c", "--company", help="The name of the company")
parser.add_argument("-u", "--url", help="The url for the company")
parser.add_argument("-e", "--email", help="The email for the company")
parser.add_argument("-n", "--namespace", help="The C++ namespace")
parser.add_argument("-d", "--directory", help="The directory for the project")
parser.add_argument("-j", "--project", help="The name of the project")
parser.add_argument("-g", "--jambagithash", help="The git hash for jamba")
args = parser.parse_args()

def askUser(msg, default):
  s = input(msg).strip()
  if len(s) == 0:
    return default if default else askUser(msg, default)
  return s

def maybeAskUser(option, msg, default):
  if option:
    return option
  else:
    if default:
      return askUser(f'{msg} (leave empty for default [{default}]) = ', default)
    else:
      return askUser(f'{msg} = ', default)

thisScriptRootDir = os.path.dirname(os.path.realpath(sys.argv[0]))
blankPluginRoot = os.path.join(thisScriptRootDir, 'blank-plugin')

def config(plugin):
  plugin['name'] = maybeAskUser(args.plugin, "Plugin Name (must be a valid C++ class name)", None)
  plugin['filename'] = maybeAskUser(args.filename, "Filename", plugin['name'])
  plugin['company'] = maybeAskUser(args.company, "Company", 'acme')
  plugin['company_url'] =  maybeAskUser(args.url, "Company URL", f'https://www.{plugin["company"]}.com') 
  plugin['company_email'] = maybeAskUser(args.email, "Company Email", f'support@{plugin["company"]}.com') 
  plugin['namespace'] = maybeAskUser(args.namespace, "C++ namespace", f"{plugin['company']}::VST::{plugin['name']}") 

  projectDir = maybeAskUser(args.directory, "Project directory", os.path.realpath(os.curdir))
  projectName = maybeAskUser(args.project, "Project Name", f'{plugin["company"]}-{plugin["name"]}-plugin')

  plugin['root_dir'] = os.path.join(os.path.realpath(projectDir), projectName)
  plugin['jamba_root_dir'] = thisScriptRootDir.replace('\\', '\\\\')
  
  print(f'''##################
Plugin Name     - {plugin["name"]}
Filename        - {plugin["filename"]} (will generate {plugin["filename"]}.vst3)
Company         - {plugin["company"]}
Company URL     - {plugin["company_url"]}
Company Email   - {plugin["company_email"]}
Jamba git hash  - {plugin["jamba_git_hash"]}
C++ Namespace   - {plugin["namespace"]}
Plugin root dir - {plugin["root_dir"]}
''')
  confirm = input("Are you sure (Y/n)?")
  if len(confirm.strip()) == 0 or confirm == 'Y':
    return
  else:
    config(plugin)

plugin = dict()

if args.jambagithash:
  plugin['jamba_git_hash'] = args.jambagithash
else:
  jambaGitSha = subprocess.run(['git', '--no-pager', 'log', '-1', '--pretty=format:%H'], cwd=blankPluginRoot, capture_output=True, text=True).stdout
  plugin['jamba_git_hash'] = subprocess.run(['git', 'describe', '--tags', jambaGitSha], cwd=blankPluginRoot, capture_output=True, text=True).stdout.rstrip()

config(plugin)

def generateUUID():
  a = uuid.uuid4().hex
  return f"0x{a[0:8]}, 0x{a[8:16]}, 0x{a[16:24]}, 0x{a[24:32]}"

plugin['namespace_start'] = '\n'.join(list(map(lambda x: f"namespace {x} {{", plugin['namespace'].split('::'))))
plugin['namespace_end']  = '\n'.join(list(map(lambda x: '}', plugin['namespace'].split('::'))))
plugin['processor_uuid']  = generateUUID()
plugin['controller_uuid']  = generateUUID()
plugin['year'] = datetime.datetime.now().year


ignoredFiles = ['.DS_Store']

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

def processFile(inFilePath, outFilePath):
  with open(inFilePath, 'r') as in_file:
    content = in_file.read()
    content = Processor(content).substitute(plugin)
    with open(outFilePath, 'w') as out_file:
      out_file.write(content)
    os.chmod(outFilePath, os.stat(inFilePath).st_mode)

def processDir(dir, parent):
  with os.scandir(dir) as it:
    for entry in it:
      if(entry.name not in ignoredFiles):
        name = entry.name.replace('__Plugin__', plugin['name'])
        path = os.path.join(plugin['root_dir'], parent, name)
        if(entry.is_dir()):
          os.makedirs(path, exist_ok = True)
          processDir(entry.path, os.path.join(parent, entry.name))
        if(entry.is_file()):
          processFile(entry.path, path)

print(f"Generating {plugin['name']} plugin....")
os.makedirs(plugin['root_dir'], exist_ok = True)
processDir(blankPluginRoot, '')
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
