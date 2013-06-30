#
# $Id: CMakeLists.txt 11838 2013-06-28 06:49:32Z pwessel $
#
# Copyright (c) 1991-2013 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
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
# To modify the cmake process: Edit your cmake/ConfigUser.cmake file
#
# src/CMakeLists.txt
#

include (ManageString)
#include (GmtHelperMacros)

# Include CREATE_DEBUG_SYM macro
include (CreateDebugSym)

# On Windows do not prefix the DLLs with the 'lib' prefix
if (WIN32)
	set(LIB_PREFIX "")
else (WIN32)
	set(LIB_PREFIX "lib")
endif (WIN32)

# Find libraries
message (STATUS "Searching dependent libraries. This may take a few minutes...")
find_package (GMT REQUIRED)
include_directories (${GMT_INCLUDE_DIR})

find_package (Threads)

# check for math and POSIX functions
include(ConfigureChecks)

include_directories (${CUSTOM_SOURCE_DIR}/src)
include_directories (${CMAKE_CURRENT_BINARY_DIR})
#include_directories (${PROJECT_BINARY_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Modules in this custom library:
set (CUSTOM_PROGS_SRCS gmtaverage.c gmtmercmap.c)

# Support code for the modules:
set (CUSTOM_LIB_SRCS gmt_custom_module.h gmt_custom_module.c)

# lib targets
set (CUSTOM_LIBS customlib)

# libgmtcustom
add_library (customlib
	${CUSTOM_LIB_SRCS}
	${CUSTOM_PROGS_SRCS})

target_link_libraries (customlib
	${GMT_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT})

if (HAVE_M_LIBRARY)
	# link the math library
	target_link_libraries (customlib m)
endif (HAVE_M_LIBRARY)

# set the build version (VERSION) and the API version (SOVERSION)
set_target_properties (customlib
	PROPERTIES
	OUTPUT_NAME gmtcustom
	PREFIX "${LIB_PREFIX}"
	IMPORT_PREFIX "${LIB_PREFIX}"
	VERSION ${CUSTOM_PACKAGE_VERSION}
	SOVERSION ${CUSTOM_PACKAGE_VERSION_MAJOR}
	INSTALL_RPATH "${CMAKE_INSTALL_RPATH}"
	LINK_FLAGS "${USER_CUSTOMLIB_LINK_FLAGS}"
	DEFINE_SYMBOL "LIBRARY_EXPORTS")

if (HAVE_DLADDR AND HAVE_LIBDL)
	# link the dynamic linking loader library
	target_link_libraries (customlib dl)
endif (HAVE_DLADDR AND HAVE_LIBDL)

# If a renaming of the custom dll has been set in ConfigUser.cmake
if (WIN32 AND CUSTOM_CUSTOM_RENAME)
	set_target_properties (customlib PROPERTIES RUNTIME_OUTPUT_NAME ${CUSTOM_CUSTOM_RENAME})
endif ()

# To expands the command line arguments in Windows, see:
# http://msdn.microsoft.com/en-us/library/8bch7bkk.aspx
if (MSVC)
	set_target_properties(customlib PROPERTIES LINK_FLAGS "setargv.obj")
endif(MSVC)

configure_file (custom_version.h.in custom_version.h)

# generate Mac debugging symbols
create_debug_sym (${CUSTOM_LIBS} ${CUSTOM_PROGS})

# temporary hack
if (NOT CUSTOM_INSTALL_MONOLITHIC)
	set (CUSTOM_MAIN_NAME gmt${CUSTOM_INSTALL_NAME_SUFFIX})
else (NOT CUSTOM_INSTALL_MONOLITHIC)
	set (CUSTOM_MAIN_NAME gmt)
endif (NOT CUSTOM_INSTALL_MONOLITHIC)

# Rename gmt target on non-monolithic installs to prevent version clash
if (NOT CUSTOM_INSTALL_MONOLITHIC)
	set_target_properties (customlib
		PROPERTIES
		OUTPUT_NAME ${CUSTOM_MAIN_NAME})
endif (NOT CUSTOM_INSTALL_MONOLITHIC)

# add the install targets
install (TARGETS ${CUSTOM_LIBS} ${CUSTOM_PROGS}
	ARCHIVE DESTINATION ${CUSTOM_LIBDIR}
	COMPONENT Runtime
	LIBRARY DESTINATION ${CUSTOM_LIBDIR}
	COMPONENT Runtime
	RUNTIME DESTINATION ${CUSTOM_BINDIR}
	COMPONENT Runtime)

if (WIN32 AND _debug_build)
	# install program database files
	set (_pdb_files)
	foreach (_target ${LIB_PREFIX}${CUSTOM_CUSTOM_RENAME} ${CUSTOM_PROGS})
		list (APPEND _pdb_files "${CMAKE_CURRENT_BINARY_DIR}/${_target}.pdb")
	endforeach ()
	install (FILES ${_pdb_files}
		DESTINATION ${CUSTOM_BINDIR}
		COMPONENT Runtime)
endif (WIN32 AND _debug_build)

string (REPLACE ".c" "" _gmt_modules "${CUSTOM_PROGS_SRCS}")
install_module_symlink (${_gmt_modules})

# Non-monolithic installs:
if (UNIX AND NOT CUSTOM_INSTALL_MONOLITHIC)
	# symlink to gmt-wrapper in bindir and libdir:
	get_target_property(_gmt_wrapper_name gmt OUTPUT_NAME)
	file (RELATIVE_PATH _rpath /bin /${CUSTOM_BINDIR}/${_gmt_wrapper_name})
	install (CODE "
	execute_process (
		COMMAND ${CMAKE_COMMAND} -E make_directory
		\${CMAKE_INSTALL_PREFIX}/bin)
	execute_process (
		COMMAND ${CMAKE_COMMAND} -E create_symlink
		${_rpath} \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/${_gmt_wrapper_name}
		COMMAND ${CMAKE_COMMAND} -E create_symlink
		${_gmt_wrapper_name} \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CUSTOM_BINDIR}/gmt)
	")

	# create symlinks in libdir/..:
	file (RELATIVE_PATH _rpath_share /${CUSTOM_LIBDIR} /${CUSTOM_SHARE_PATH})
	string (REGEX REPLACE "^[.][.]/(.*)" "\\1" _rpath_share "${_rpath_share}")
	install (CODE "
	execute_process (
		COMMAND ${CMAKE_COMMAND} -E create_symlink
		${_rpath_share} \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CUSTOM_LIBDIR}/../share
		COMMAND ${CMAKE_COMMAND} -E create_symlink
		${_rpath_man} \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CUSTOM_LIBDIR}/../man)
	")
endif (UNIX AND NOT CUSTOM_INSTALL_MONOLITHIC)

# this prints out the final version of the compiler/linker flags
string (TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
string(REGEX REPLACE "[ ]+" " " _using_cflags ${CMAKE_C_FLAGS} " "
	${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}})
message (STATUS "Using CFLAGS = '${_using_cflags}'")

# vim: textwidth=78 noexpandtab tabstop=2 softtabstop=2 shiftwidth=2
