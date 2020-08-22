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

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

# This is the main compiled file so should exist
set(VSTSDK2_KNOWN_FILE "public.sdk/source/vst2.x/audioeffect.cpp")

if(NOT EXISTS "${VST2_SDK_ROOT}/${VSTSDK2_KNOWN_FILE}")
  if(EXISTS "${VST2_SDK_ROOT}/VST2_SDK/${VSTSDK2_KNOWN_FILE}")
    set(VST2_SDK_ROOT "${VST2_SDK_ROOT}/VST2_SDK" CACHE PATH "Location of VST2 SDK" FORCE)
  else()
    message(FATAL_ERROR "VST2_SDK_ROOT=${VST2_SDK_ROOT} does not seem to point to a valid VST2 SDK (VST2 is enabled)")
  endif()
endif()

message(STATUS "VST2_SDK_ROOT=${VST2_SDK_ROOT}")
