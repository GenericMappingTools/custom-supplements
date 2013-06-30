#
# $Id: ConfigCMake.cmake 10119 2012-05-19 08:08:25Z fwobbe $
#
# Useful CMake variables.
#
# There are three configuration files:
#   1) "ConfigDefault.cmake" - is version controlled and used to add new default
#      variables and set defaults for everyone.
#   2) "ConfigUser.cmake" in the source tree - is not version controlled
#      (currently listed in svn:ignore property) and used to override defaults on
#      a per-user basis.
#   3) "ConfigUser.cmake" in the build tree - is used to override
#      "ConfigUser.cmake" in the source tree.
#
# NOTE: If you want to change CMake behaviour just for yourself then copy
#      "ConfigUserTemplate.cmake" to "ConfigUser.cmake" and then edit
#      "ConfigUser.cmake" (not "ConfigDefault.cmake" or "ConfigUserTemplate.cmake").
#
include ("${CMAKE_SOURCE_DIR}/cmake/ConfigDefault.cmake")

# If "ConfigUser.cmake" doesn't exist then create one for convenience.
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/ConfigUser.cmake")
	include ("${CMAKE_SOURCE_DIR}/cmake/ConfigUser.cmake")
endif (EXISTS "${CMAKE_SOURCE_DIR}/cmake/ConfigUser.cmake")

# If you've got a 'ConfigUser.cmake' in the build tree then that overrides the
# one in the source tree.
if (EXISTS "${CMAKE_BINARY_DIR}/cmake/ConfigUser.cmake")
	include ("${CMAKE_BINARY_DIR}/cmake/ConfigUser.cmake")
endif (EXISTS "${CMAKE_BINARY_DIR}/cmake/ConfigUser.cmake")

###########################################################
# Do any needed processing of the configuration variables #
###########################################################

# Build type
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
	set (DEBUG_BUILD TRUE)
endif (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")

# Here we change it to add the SVN revision number for non-public releases - see Package.cmake for
# why this has to be done here.
set (CUSTOM_PACKAGE_VERSION_WITH_SVN_REVISION ${CUSTOM_PACKAGE_VERSION})
# Add the Subversion version number to the package filename if this is a non-public release.
# A non-public release has an empty 'CUSTOM_SOURCE_CODE_CONTROL_VERSION_STRING' variable in 'ConfigDefault.cmake'.
set (HAVE_SVN_VERSION)
if (NOT CUSTOM_SOURCE_CODE_CONTROL_VERSION_STRING)
	# Get the location, inside the staging area location, to copy the application bundle to.
	execute_process (
		COMMAND svnversion ${CUSTOM_SOURCE_DIR}
		RESULT_VARIABLE SVN_VERSION_RESULT
		OUTPUT_VARIABLE SVN_VERSION_OUTPUT
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if (SVN_VERSION_RESULT)
		message (STATUS "Unable to determine svn version number for non-public release - ignoring.")
	else (SVN_VERSION_RESULT)
		if (SVN_VERSION_OUTPUT MATCHES "Unversioned")
			message (STATUS "Unversioned source tree, non-public release.")
		else (SVN_VERSION_OUTPUT MATCHES "Unversioned")
			# The 'svnversion' command can output a range of revisions with a colon
			# separator - but this causes problems with filenames so we'll remove the
			# colon and the end revision after it.
			string (REGEX REPLACE ":.*$" "" SVN_VERSION ${SVN_VERSION_OUTPUT})
			if (NOT SVN_VERSION STREQUAL exported)
				# Set the updated package version.
				set (CUSTOM_PACKAGE_VERSION_WITH_SVN_REVISION "${CUSTOM_PACKAGE_VERSION}_r${SVN_VERSION}")
				set (HAVE_SVN_VERSION TRUE)
			endif (NOT SVN_VERSION STREQUAL exported)
		endif (SVN_VERSION_OUTPUT MATCHES "Unversioned")
	endif (SVN_VERSION_RESULT)
endif (NOT CUSTOM_SOURCE_CODE_CONTROL_VERSION_STRING)

# The current CUSTOM version.
set (CUSTOM_VERSION_STRING "${CUSTOM_PACKAGE_NAME} ${CUSTOM_PACKAGE_VERSION_WITH_SVN_REVISION}")

set (CUSTOM_LONG_VERSION_STRING "${CUSTOM_PACKAGE_NAME} - ${CUSTOM_PACKAGE_DESCRIPTION_SUMMARY}, Version ${CUSTOM_PACKAGE_VERSION_WITH_SVN_REVISION}")

# Set install name suffix used for directories and main gmt executable
if (NOT DEFINED CUSTOM_INSTALL_NAME_SUFFIX)
	set (CUSTOM_INSTALL_NAME_SUFFIX "-${CUSTOM_PACKAGE_VERSION_WITH_SVN_REVISION}")
endif (NOT DEFINED CUSTOM_INSTALL_NAME_SUFFIX)

# Get date
try_run (_exit_today _compiled_today
	${CMAKE_BINARY_DIR}/CMakeTmp
	${CMAKE_MODULE_PATH}/today.c
	CMAKE_FLAGS
	RUN_OUTPUT_VARIABLE _today)

if (NOT _compiled_today OR _exit_today EQUAL -1)
	message (WARNING "Date not implemented, please file a bug report.")
	set(_today "1313;13;13;Undecember")
endif (NOT _compiled_today OR _exit_today EQUAL -1)

list(GET _today 0 YEAR)
list(GET _today 1 MONTH)
list(GET _today 2 DAY)
list(GET _today 3 MONTHNAME)
list(GET _today 0 1 2 DATE)
string (REPLACE ";" "-" DATE "${DATE}")
set (_today)

# set package date
if (NOT CUSTOM_VERSION_YEAR)
	set (CUSTOM_VERSION_YEAR ${YEAR})
endif (NOT CUSTOM_VERSION_YEAR)

# reset list of extra license files
set (CUSTOM_EXTRA_LICENSE_FILES)

# location of GNU license files
set (COPYING_GPL ${CUSTOM_SOURCE_DIR}/COPYINGv3)
set (COPYING_LGPL ${CUSTOM_SOURCE_DIR}/COPYING.LESSERv3)

# Install path for CUSTOM binaries, headers and libraries
include (GNUInstallDirs) # defines CMAKE_INSTALL_LIBDIR (lib/lib64)
if (CUSTOM_INSTALL_MONOLITHIC)
	set (CUSTOM_INCDIR include)
	set (CUSTOM_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set (CUSTOM_BINDIR bin)
else (CUSTOM_INSTALL_MONOLITHIC)
	set (CUSTOM_INCDIR include/gmt${CUSTOM_INSTALL_NAME_SUFFIX})
	set (CUSTOM_LIBDIR ${CMAKE_INSTALL_LIBDIR}/gmt${CUSTOM_INSTALL_NAME_SUFFIX}/lib)
	set (CUSTOM_BINDIR ${CMAKE_INSTALL_LIBDIR}/gmt${CUSTOM_INSTALL_NAME_SUFFIX}/bin)
endif (CUSTOM_INSTALL_MONOLITHIC)

# use, i.e. don't skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CUSTOM_LIBDIR}")

# make executables relocatable on supported platforms
if (UNIX AND NOT CYGWIN)
	# find relative libdir from executable dir
	file (RELATIVE_PATH _rpath /${CUSTOM_BINDIR} /${CUSTOM_LIBDIR})
	# remove trailing /
	string (REGEX REPLACE "/$" "" _rpath "${_rpath}")
	if (APPLE)
		# relative RPATH on osx
		set (CMAKE_INSTALL_NAME_DIR @loader_path/${_rpath})
	else (APPLE)
		# relative RPATH on Linux, Solaris, etc.
		set (CMAKE_INSTALL_RPATH "\$ORIGIN/${_rpath}")
	endif (APPLE)
endif (UNIX AND NOT CYGWIN)

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Make GNU and Intel C compiler default to C99
if (CMAKE_C_COMPILER_ID MATCHES "(GNU|Intel)" AND NOT CMAKE_C_FLAGS MATCHES "-std=")
	set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
endif ()

# vim: textwidth=78 noexpandtab tabstop=2 softtabstop=2 shiftwidth=2
