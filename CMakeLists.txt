#
# The MIT License (MIT)
#
# Copyright (c) 2017 Joel Höner <athre0z@zyantific.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)
project(REtypedef)

set(IDA_ENABLE_QT_SUPPORT ON CACHE BOOL "")
include("ida-cmake/cmake/IDA.cmake")

set(headers
    "src/Config.hpp"
    "src/Core.hpp"
    "src/ImportExport.hpp"
    "src/InlineDetour.hpp"
    "src/RETypedef.hpp"
    "src/Settings.hpp"
    "src/SubstitutionManager.hpp"
    "src/Ui.hpp"
    "src/Utils.hpp")
set(sources
    "src/Core.cpp"
    "src/ImportExport.cpp"
    "src/InlineDetour.cpp"
    "src/REtypedef.cpp"
    "src/Settings.cpp"
    "src/SubstitutionManager.cpp"
    "src/Ui.cpp"
    "src/Utils.cpp")
set(forms
    "ui/AboutDialog.ui"
    "ui/SubstitutionEditor.ui")
set(resources 
    "resources/rsrc.qrc")

# Add Zydis dependency.
set(ZYDIS_FEATURE_EVEX FALSE CACHE BOOL "" FORCE) 
set(ZYDIS_FEATURE_MVEX FALSE CACHE BOOL "" FORCE) 
set(ZYDIS_FEATURE_FLAGS FALSE CACHE BOOL "" FORCE) 
set(ZYDIS_FEATURE_CPUID FALSE CACHE BOOL "" FORCE) 
set(ZYDIS_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TOOLS FALSE CACHE BOOL "" FORCE)
add_subdirectory("../zydis" "zydis")

# Add IDA plugin target
include_directories("src")
add_ida_qt_plugin(
    ${CMAKE_PROJECT_NAME} 
    ${headers} ${sources} 
    ${forms} ${resources})
target_link_libraries(${CMAKE_PROJECT_NAME} "Zydis")
