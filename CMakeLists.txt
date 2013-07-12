#
#	$Id: CMakeLists.txt 11803 2013-06-24 23:06:22Z pwessel $
#
#	Copyright (c) 1991-2013 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
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
#	Contact info: gmt.soest.hawaii.edu
#-------------------------------------------------------------------------------
#
# To modify the cmake process: Edit your cmake/ConfigUser.cmake file
#
# To build out-of-source do (example):
#
#	mkdir build
#	cd build
#	cmake -DCMAKE_INSTALL_PREFIX=custom -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
#
# CMAKE_BUILD_TYPE can be: empty, Debug, Release, RelWithDebInfo or MinSizeRel
#
# cmake creates a new file cmake/ConfigUser.cmake if it does not already
# exist. You can configure additional options there.
#

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

# Define project name
project (CUSTOM)

# Where to find our CMake modules (this variable is visible in subdirectories).
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/"
	CACHE INTERNAL "Location of our custom CMake modules." FORCE)

# Include configuration options (default options and options overridden by user).
include (ConfigCMake)

# Find UNIX commands
include (FindUnixCommands)
find_program (SVN svn)

# Add subdirectories
add_subdirectory (src)
add_subdirectory (cmake/dist) # make distribution bundles (always last)
get_property(GMT_CUSTOM_LIB_PATH TARGET customlib PROPERTY LOCATION)
GET_FILENAME_COMPONENT (GMT_CUSTOM_LIB_NAME ${GMT_CUSTOM_LIB_PATH} NAME)

# Source release target
#if (SVN AND HAVE_SVN_VERSION)
#	# Export svn working tree
#	add_custom_target (svn_export_release
#		COMMAND ${SVN} --force export
#		${CUSTOM_SOURCE_DIR} ${CUSTOM_RELEASE_PREFIX})
#	add_depend_to_target (custom_release svn_export_release)
#	find_program (GNUTAR NAMES gnutar gtar tar)
#	if (GNUTAR)
#		# Targets for creating tarballs
#		string (REGEX REPLACE ".*/" "" _release_dirname "${CUSTOM_RELEASE_PREFIX}")
#		add_custom_command (OUTPUT ${_release_dirname}.tar.gz
#			COMMAND ${GNUTAR} -cz --owner 0 --group 0 --mode a=rX,u=rwX
#			-f ${CUSTOM_BINARY_DIR}/${_release_dirname}.tar.gz ${_release_dirname}
#			DEPENDS ${CUSTOM_RELEASE_PREFIX}
#			WORKING_DIRECTORY ${CUSTOM_RELEASE_PREFIX}/..
#			VERBATIM)
#		add_custom_command (OUTPUT ${_release_dirname}.tar.bz2
#			COMMAND ${GNUTAR} -cj --owner 0 --group 0 --mode a=rX,u=rwX
#			-f ${CUSTOM_BINARY_DIR}/${_release_dirname}.tar.bz2 ${_release_dirname}
#			DEPENDS ${CUSTOM_RELEASE_PREFIX}
#			WORKING_DIRECTORY ${CUSTOM_RELEASE_PREFIX}/..
#			VERBATIM)
#		add_custom_target (custom_release_tar
#			DEPENDS ${CUSTOM_RELEASE_PREFIX}
#			${_release_dirname}.tar.gz ${_release_dirname}.tar.bz2)
#	endif (GNUTAR)
#endif (SVN AND HAVE_SVN_VERSION)

# Configuration done
message(
	"*  Options:\n"
	"*  GMT library             : ${GMT_LIBRARY}\n"
	"*  GMT include dir         : ${GMT_INCLUDE_DIR}\n"
	"*\n"                          
	"*  Locations:\n"              
	"*  CUSTOM library          : ${GMT_CUSTOM_LIB_NAME}\n"
	"*  Installing CUSTOM in    : ${CMAKE_INSTALL_PREFIX}\n"
	"*  CUSTOM_SHARE_PATH       : ${CMAKE_INSTALL_PREFIX}/${CUSTOM_SHARE_PATH}")

# vim: textwidth=78 noexpandtab tabstop=2 softtabstop=2 shiftwidth=2
