#
# The MIT License (MIT)
#
# Copyright (c) 2014 athre0z
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

cmake_minimum_required(VERSION 2.8.12)

set(UDIS86_DEFINITIONS "")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(arch "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(arch "x86")
else ()
	message(SEND_ERROR "Unsupported target architecture")
endif ()

if (WIN32)
	set(lib_prefix "lib")
endif ()

find_path(UDIS86_INCLUDE_DIR "udis86.h"
	HINTS ${UDIS86_INCLUDEDIR} ${UDIS86_ROOT} $ENV{UDIS86_ROOT}
	PATH_SUFFIXES "BuildVS2010/Build/Include" "Build/Include")
find_library(UDIS86_LIBRARY 
	NAMES "${lib_prefix}udis86"
	HINTS ${UDIS86_LIBDIR} ${UDIS86_ROOT} $ENV{UDIS86_ROOT}
	PATH_SUFFIXES "BuildVS2010/Build/Lib/${arch}" "Build/Lib/${arch}")

set(UDIS86_INCLUDE_DIRS ${UDIS86_INCLUDE_DIR})
set(UDIS86_LIBRARIES ${UDIS86_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(udis86 DEFAULT_MSG
	UDIS86_LIBRARY UDIS86_INCLUDE_DIR)

mark_as_advanced(UDIS86_INCLUDE_DIR UDIS86_LIBRARY)