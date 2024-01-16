#
#	Copyright (c) 1991-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
#	See LICENSE.TXT file for copying and redistribution conditions.
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU Lesser General Public License as published by
#	the Free Software Foundation; version 3 or any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU Lesser General Public License for more details.
#
#	Contact info: www.generic-mapping-tools.org
#-------------------------------------------------------------------------------
#
# To build out-of-source do (for example):
#
#	mkdir build
#	cd build
#	cmake [-DCMAKE_INSTALL_PREFIX=custom] [-DCMAKE_BUILD_TYPE=RelWithDebInfo] ..
#
# CMAKE_BUILD_TYPE can be: empty, Debug, Release, RelWithDebInfo or MinSizeRel
#

## Define project name. Change this to what your plugin should be named.
project (custom)

# Make sure the user doesn't play dirty with symlinks
get_filename_component (srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component (bindir "${CMAKE_BINARY_DIR}" REALPATH)

# Disallow in-source builds
if (${srcdir} STREQUAL ${bindir})
	message(FATAL_ERROR "In-source builds are not allowed. "
	"Please create a directory and run cmake from there, passing the path "
	"to this source directory as the last argument. This process created "
	"the file `CMakeCache.txt' and the directory `CMakeFiles' in ${srcdir}. "
	"Please remove them.")
endif (${srcdir} STREQUAL ${bindir})

# Define minimum CMake version required
cmake_minimum_required (VERSION 2.8.5)

# Where to find our CMake modules (this variable is visible in subdirectories).
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/"
	CACHE INTERNAL "Location of our custom CMake modules." FORCE)

# Add subdirectories
add_subdirectory (src)

# Get name of the custom supplemental library
get_directory_property (_suppl_lib_name DIRECTORY "src" DEFINITION "SUPPL_LIB_NAME")
get_target_property (_name ${_suppl_lib_name} OUTPUT_NAME)
get_target_property (_prefix ${_suppl_lib_name} PREFIX)
set (GMT_CUSTOM_LIB_NAME ${_prefix}${_name}${CMAKE_SHARED_MODULE_SUFFIX})

# Configuration done
message(
	"*  Options:\n"
	"*  GMT library             : ${GMT_LIBRARY}\n"
	"*  GMT include dir         : ${GMT_INCLUDE_DIR}\n"
	"*\n"                          
	"*  Locations:\n"              
	"*  Custom name             : ${CMAKE_PROJECT_NAME}\n"
	"*  CUSTOM library          : ${GMT_CUSTOM_LIB_NAME}\n"
	"*  Installing CUSTOM in    : ${CMAKE_INSTALL_PREFIX}\n"
	"*  CUSTOM_SHARE_PATH       : ${CMAKE_INSTALL_PREFIX}/${CUSTOM_SHARE_PATH}")
