#
# $Id: ConfigDefault.cmake 11782 2013-06-23 18:10:30Z pwessel $
#
# Copyright (c) 1991-2013 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis and F. Wobbe
# See LICENSE.TXT file for copying and redistribution conditions.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3 or any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# Contact info: gmt.soest.hawaii.edu
#-------------------------------------------------------------------------------
#
# Default CUSTOM settings. DO NOT EDIT THIS FILE!
#
# There are two configuration files:
#   1) "ConfigDefault.cmake" - is version controlled and used to add new default
#      variables and set defaults for everyone.
#   2) "ConfigUser.cmake" - is not version controlled (currently listed in
#      svn:ignore property) and used to override defaults on a per-user basis.
#
# NOTE: If you want to change CMake behaviour just for yourself then copy
#      "ConfigUserTemplate.cmake" to "ConfigUser.cmake" and then edit
#      "ConfigUser.cmake" (not "ConfigDefault.cmake" or "ConfigUserTemplate.cmake").
#

# The CUSTOM package name.
set (CUSTOM_PACKAGE_NAME "CUSTOM")

# a short description of the custom project (only a few words).
set (CUSTOM_PACKAGE_DESCRIPTION_SUMMARY "Custom Extension to The Generic Mapping Tools")

# The CUSTOM package version.
set (CUSTOM_PACKAGE_VERSION_MAJOR "5")
set (CUSTOM_PACKAGE_VERSION_MINOR "0")
set (CUSTOM_PACKAGE_VERSION_PATCH "1b")

# The subversion revision of the CUSTOM source code.
# This is manually set when making CUSTOM *public* releases.
# However, when making internal releases or just an ordinary developer build, leave it
# empty; if it is empty, the revision number is automatically populated for you on build.
#set (CUSTOM_SOURCE_CODE_CONTROL_VERSION_STRING "9047")

# The CUSTOM package version.
set (CUSTOM_PACKAGE_VERSION "${CUSTOM_PACKAGE_VERSION_MAJOR}.${CUSTOM_PACKAGE_VERSION_MINOR}.${CUSTOM_PACKAGE_VERSION_PATCH}")

# CUSTOM_VERSION_YEAR set to current date in cmake/modules/ConfigCMake.cmake
# if not specified here:
#set (CUSTOM_VERSION_YEAR "2013")

# Directory in which to install the release sources per default
if (NOT DEFINED CUSTOM_RELEASE_PREFIX)
	set (CUSTOM_RELEASE_PREFIX ${CUSTOM_BINARY_DIR}/CUSTOM-${CUSTOM_PACKAGE_VERSION}-src)
endif (NOT DEFINED CUSTOM_RELEASE_PREFIX)


# The CUSTOM copyright - string version to be used in a source file.
set (CUSTOM_COPYRIGHT_STRING)
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}Copyright 1991-${CUSTOM_VERSION_YEAR} Tne list of autors\\n")
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}This program comes with NO WARRANTY, to the extent permitted by law.\\n")
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}You may redistribute copies of this program under the terms of the\\n")
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}GNU General Public License.\\n")
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}For more information about these matters, see the file named LICENSE.TXT.\\n")
set (CUSTOM_COPYRIGHT_STRING "${CUSTOM_COPYRIGHT_STRING}\\n")


# You can set the build configuration type as a command-line argument to 'cmake' using -DCMAKE_BUILD_TYPE:STRING=Debug for example.
# If no build configuration type was given as a command-line option to 'cmake' then a default cache entry is set here.
# A cache entry is what appears in the 'CMakeCache.txt' file that CMake generates - you can edit that file directly or use the CMake GUI to edit it.
# The user can then set this parameter via the CMake GUI before generating the native build system.
# NOTE: this is not needed for visual studio because it has multiple configurations in the ide (and CMake includes them all).
# however makefile generators can only have one build type (to have multiple build types you'll need multiple out-of-place builds - one for each build type).
#
# The following are some valid build configuration types:
# 1) Debug - no optimisation with debug info.
# 2) Release - release build optimised for speed.
# 3) RelWithDebInfo - release build optimised for speed with debug info.
# 4) MinSizeRel - release build optimised for size.

# The following is from http://mail.kde.org/pipermail/kde-buildsystem/2008-November/005112.html...
#
# "The way to identify whether a generator is multi-configuration is to
# check whether CMAKE_CONFIGURATION_TYPES is set.  The VS/XCode generators
# set it (and ignore CMAKE_BUILD_TYPE).  The Makefile generators do not
# set it (and use CMAKE_BUILD_TYPE).  If CMAKE_CONFIGURATION_TYPES is not
# already set, don't set it."
#
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
	if (NOT DEFINED CMAKE_BUILD_TYPE)
		# Should we set build type to RelWithDebInfo for developers and
		# to release for general public (ie when GPLATES_SOURCE_RELEASE is true) ?
		# Currently it's Release for both.
		set (CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ${extra_build_configurations}."
		FORCE)
	endif (NOT DEFINED CMAKE_BUILD_TYPE)
endif (NOT DEFINED CMAKE_CONFIGURATION_TYPES)


# Turn this on if you want to...
#  Unix: see compiler commands echoed to console and messages about make
#  entering and leaving directories.
#  VisualStudio: see compiler commands.
# Setting CMAKE_VERBOSE_MAKEFILE to 'true'...
#  Unix: puts 'VERBOSE=1' in the top Makefile.
#  VisualStudio: sets SuppressStartupBanner to FALSE.
# If CMAKE_VERBOSE_MAKEFILE is set to 'false' and you want to turn on
# verbosity temporarily you can...
#  Unix: type 'make VERBOSE=1'  on the command-line when building.
#  VisualStudio: change SuppressStartupBanner to 'no' in "project
#  settings->configuration properties->*->general".
if (NOT DEFINED CMAKE_VERBOSE_MAKEFILE)
	set (CMAKE_VERBOSE_MAKEFILE false)
endif (NOT DEFINED CMAKE_VERBOSE_MAKEFILE)

# prefer shared libs over static
set (BUILD_SHARED_LIBS true)
set (CMAKE_FIND_STATIC LAST)

# search order for find_*
set (CMAKE_FIND_FRAMEWORK LAST)

# vim: textwidth=78 noexpandtab tabstop=2 softtabstop=2 shiftwidth=2