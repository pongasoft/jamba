#!/usr/bin/env python3

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

def config():
  plugin = dict()
  plugin['name'] = maybeAskUser(args.plugin, "Plugin Name", None)
  plugin['filename'] = maybeAskUser(args.filename, "Filename", plugin['name'])
  plugin['company'] = maybeAskUser(args.company, "Company", 'acme')
  plugin['company_url'] =  maybeAskUser(args.url, "Company URL", f'https://www.{plugin["company"]}.com') 
  plugin['company_email'] = maybeAskUser(args.email, "Company Email", f'support@{plugin["company"]}.com') 
  plugin['namespace'] = maybeAskUser(args.namespace, "C++ namespace", f"{plugin['company']}::VST::{plugin['name']}") 

  projectDir = maybeAskUser(args.directory, "Project directory", os.path.realpath(os.curdir))
  projectName = maybeAskUser(args.project, "Project Name", f'{plugin["company"]}-{plugin["name"]}-plugin')

  plugin['root_dir'] = os.path.join(os.path.realpath(projectDir), projectName)
  
  print(f'''##################
Plugin Name     - {plugin["name"]}
Filename        - {plugin["filename"]}
Company         - {plugin["company"]}
Company URL     - {plugin["company_url"]}
Company Email   - {plugin["company_email"]}
C++ Namespace   - {plugin["namespace"]}
Plugin root dir - {plugin["root_dir"]}
''')
  confirm = input("Are you sure (Y/n)?")
  if len(confirm.strip()) == 0 or confirm == 'Y':
    return plugin
  else:
    return config()

plugin = config()

def generateUUID():
  a = uuid.uuid4().hex
  return f"0x{a[0:8]}, 0x{a[8:16]}, 0x{a[16:24]}, 0x{a[24:32]}"

plugin['namespace_start'] = '\n'.join(list(map(lambda x: f"namespace {x} {{", plugin['namespace'].split('::'))))
plugin['namespace_end']  = '\n'.join(list(map(lambda x: '}', plugin['namespace'].split('::'))))
plugin['processor_uuid']  = generateUUID()
plugin['controller_uuid']  = generateUUID()
plugin['year'] = datetime.datetime.now().year
jambaGitSha = subprocess.run(['git', '--no-pager', 'log', '-1', '--pretty=format:%H'], cwd=blankPluginRoot, capture_output=True, text=True).stdout
plugin['jamba_git_hash'] = subprocess.run(['git', 'describe', '--tags', jambaGitSha], cwd=blankPluginRoot, capture_output=True, text=True).stdout.rstrip()

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
cd <build_folder>
{plugin['root_dir']}/configure.sh Debug

For Windows 10:
---------------
cd <build_folder>
{plugin['root_dir']}/configure.bat
''')
