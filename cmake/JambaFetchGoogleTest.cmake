# Copyright (c) 2021-2023 pongasoft
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

cmake_minimum_required(VERSION 3.19)

jamba_fetch_content(NAME googletest)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "Set by Jamba" FORCE)

# Do not install GoogleTest!
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

smtg_target_setup_universal_binary("gtest")
smtg_target_setup_universal_binary("gtest_main")
smtg_target_setup_universal_binary("gmock")
smtg_target_setup_universal_binary("gmock_main")
