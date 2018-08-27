#!/usr/bin/env python3

import sys
import os
from string import Template
import re
import uuid
import datetime
import subprocess
import argparse

plugin = {}
plugin['name'] = 'Kooza'
plugin['filename'] = plugin['name']
plugin['company'] = 'acme'
plugin['company_url'] = 'https://www.acme.com'
plugin['company_email'] = 'support@acme.com'
plugin['namespace'] = f"{plugin['company']}::VST::{plugin['name']}"

blankPluginRoot = os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'blank-plugin')
pluginRootDir = f"/tmp/{plugin['name']}-plugin"

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

print(blankPluginRoot)
print(plugin)

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

#print(Processor('#include "[-name-]CIDs.h [-namespace_start-]"').substitute(plugin))

def processFile(inFilePath, outFilePath):
  print("Processing ", inFilePath)
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
        path = os.path.join(pluginRootDir, parent, name)
        if(entry.is_dir()):
          os.makedirs(path, exist_ok = True)
          processDir(entry.path, os.path.join(parent, entry.name))
        if(entry.is_file()):
          processFile(entry.path, path)

os.makedirs(pluginRootDir, exist_ok = True)
processDir(blankPluginRoot, '')
