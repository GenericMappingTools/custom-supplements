#
# Copyright (c) 1991-2020 by GMT Team (https://www.generic-mapping-tools.org/team.html)
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
# Contact info: http://www.soest.hawaii.edu/PT/GSFML
#-------------------------------------------------------------------------------
# Below, "custom" and "CUSTOM" should be replaced with your actual supplement name
#
# SUPPL_LIB_NAME [required]
#	name of the supplement package. It can be different from the directory name.
#   If not given then the modules are added to the supplements library
set (SUPPL_LIB_NAME custom)
set (SUPPL_LIB_PURPOSE "GMT custom: Tools for the CUSTOM project")

# SUPPL_PROGS_SRCS [required]
#	list of C source codes for supplemental modules
set (SUPPL_PROGS_SRCS gmtaverage.c gmtmercmap.c gmtparser.c grdfourier.c)

# SUPPL_LIB_SRCS [required]
#	list of C source codes for supplemental library
set (SUPPL_LIB_SRCS ${SUPPL_PROGS_SRCS})

# SUPPL_HEADERS [optional]
#	header files. Will be installed if BUILD_DEVELOPER is TRUE
set (SUPPL_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/custom_config.h)

# SUPPL_EXAMPLE_FILES [optional]
#	README and other example files
#set (SUPPL_EXAMPLE_FILES README.custom)

# SUPPL_EXAMPLE_PROGS [optional]
#	example scripts
#set (SUPPL_EXAMPLE_PROGS test1.sh test2.sh)

# CUSTOM SPECIFIC SETTINGS

set (CUSTOM_PACKAGE_VERSION "6.1.0")

configure_file (custom_config.h.in custom_config.h)

find_package (GMT REQUIRED)
include_directories (${GMT_INCLUDE_DIR})

find_package (NETCDF REQUIRED)
include_directories (${NETCDF_INCLUDE_DIR})

add_library (${SUPPL_LIB_NAME} MODULE
	${SUPPL_LIB_SRCS} 
	${CMAKE_CURRENT_BINARY_DIR}/custom_config.h
	gmt_custom_glue.c
	)

target_link_libraries (${SUPPL_LIB_NAME}
	${GMT_LIBRARIES}
	${NETCDF_LIBRARIES}
)

set_target_properties (${SUPPL_LIB_NAME}
    PROPERTIES
	OUTPUT_NAME ${SUPPL_LIB_NAME}
	RUNTIME_OUTPUT_NAME ${SUPPL_LIB_NAME}
    LIBRARY_OUTPUT_DIRECTORY plugins
    RUNTIME_OUTPUT_DIRECTORY plugins
    PREFIX ""
    DEFINE_SYMBOL "LIBRARY_EXPORTS")
