#!/bin/bash
#
# $Id$
#
# Copyright (c) 2012-2017
# by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
# See LICENSE.TXT file for copying and redistribution conditions.
#
# Below, <TAG> is a users custom shared lib tag set via the
# project statement in the top-level CMakeLists.txt
#
# This script will find all the C files glue in the current dir
# and extract all the THIS_MODULE_{NAME,LIB,PURPOSE,KEYS}
# strings from the sources files, then create two files:
# 	gmt_<TAG>_module.h.
# 	gmt_<TAG>_module.c.
#
# Rerun this script when there are changes in the code or you
# have added more modules.
#

if [ "X$1" = "X" ]; then
	cat << EOF >&2

	gmt_make_custom_code.sh - Build API glue for new supplement

	gmt_make_custom_code.sh assists developers of a custom supplement
	by creating the required glue functions required by the GMT API
	to enable "gmt --help" and "gmt --show-modules" for the custom
	modules.  It also creates another function that is required by
	the GMT_Encode_Options API function used by developers of any
	external APIs, such as MATLAB, Julia, Python, and others.
	
	<TAG> is the developers name for the custom shared plugin and the
	name is obtained from project statement in the top-level CMakeLists.txt.

	Run this script with the <TAG> as arguments and it will create two files:
	
		gmt_<TAG>_module.h.
		gmt_<TAG>_module.c.
	
	The gmt-custom project's CMakeLists.txt files will automatically add
	these to your provided C code.

EOF
	exit -1
fi

if [ ! -f ../CMakeLists.txt ]; then
	echo "gmt_make_custom_code.sh: Cannot find CMakeLists.txt in parent directory - aborting." >&2
	exit -1
fi
set -e
LIB=`grep '^project' ../CMakeLists.txt | tr '()' '  ' | awk '{print $2}'`
if [ "X$LIB" = "X" ]; then
	echo "gmt_make_custom_code.sh: Cannot find project setting in CMakeLists.txt in parent directory - aborting." >&2
	exit -1
fi

echo "gmt_make_custom_code.sh: Scanning for modules" >&2
# Make sure we get both upper- and lower-case versions of the tag
U_TAG=`echo $LIB | tr '[a-z]' '[A-Z]'`
L_TAG=`echo $LIB | tr '[A-Z]' '[a-z]'`

# Look in current dir
grep "#define THIS_MODULE_LIB" *.c | awk -F: '{print $1}' | sort > /tmp/tmp.lis
rm -f /tmp/NAME.lis /tmp/LIB.lis /tmp/PURPOSE.lis /tmp/KEYS.lis /tmp/all.lis
while read program; do
	grep "#define THIS_MODULE_NAME" $program    | awk '{print $3}' | sed -e 's/"//g' >> /tmp/NAME.lis
	grep "#define THIS_MODULE_LIB" $program     | awk '{print $3}' | sed -e 's/"//g' >> /tmp/LIB.lis
	grep "#define THIS_MODULE_PURPOSE" $program | sed -e 's/#define THIS_MODULE_PURPOSE//g' | awk '{print $0}' >> /tmp/PURPOSE.lis
	grep "#define THIS_MODULE_KEYS" $program    | sed -e 's/#define THIS_MODULE_KEYS//g' | awk '{print $0}' >> /tmp/KEYS.lis
done < /tmp/tmp.lis
# Prepend group+name so we can get a list sorted on group name then individual programs
paste /tmp/LIB.lis /tmp/NAME.lis | awk '{printf "%s%s|%s\n", $1, $2, $2}' > /tmp/SORT.txt
paste /tmp/SORT.txt /tmp/LIB.lis /tmp/PURPOSE.lis /tmp/KEYS.lis | sort -k1 > /tmp/SORTED.txt
awk -F"|" '{print $2}' /tmp/SORTED.txt > /tmp/$LIB.txt
rm -f /tmp/tmp.lis /tmp/NAME.lis /tmp/LIB.lis /tmp/PURPOSE.lis /tmp/KEYS.lis /tmp/SORTED.txt /tmp/SORT.txt

# Extract the extension purpose string from CMakeLists.txt
LIB_STRING=`grep LIB_STRING CMakeLists.txt | awk -F= '{print $NF}'`

# The output file produced
FILE_CUSTOM_MODULE_C=gmt_${L_TAG}_module.c
FILE_CUSTOM_MODULE_H=gmt_${L_TAG}_module.h
COPY_YEAR=$(date +%Y)

#
# Generate FILE_CUSTOM_MODULE_H
#
echo "gmt_make_custom_code.sh: Generate ${FILE_CUSTOM_MODULE_H}" >&2

cat << EOF > ${FILE_CUSTOM_MODULE_H}
/* \$Id\$
 *
 * Copyright (c) 2016-${COPY_YEAR} by $USER
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_${L_TAG}_module.h declares the prototypes for ${L_TAG} module functions
 * and the array that contains ${L_TAG} GMT module parameters such as name
 * and purpose strings.
 * DO NOT edit this file directly! Regenerate thee file by running
 *	 make_custom_module_src.sh
 */

#pragma once
#ifndef _GMT_${U_TAG}_MODULE_H
#define _GMT_${U_TAG}_MODULE_H

#ifdef __cplusplus /* Basic C++ support */
extern "C" {
#endif

/* Declaration modifiers for DLL support (MSC et al) */
#include "declspec.h"

/* Prototypes of all modules in the GMT ${L_TAG} library */
EOF
gawk '{printf "EXTERN_MSC int GMT_%s (void *API, int mode, void *args);\n", $1;}' /tmp/$LIB.txt >> ${FILE_CUSTOM_MODULE_H}
cat << EOF >> ${FILE_CUSTOM_MODULE_H}

/* Pretty print all modules in the GMT ${L_TAG} library and their purposes */
EXTERN_MSC void gmt_${L_TAG}_module_show_all (void *API);
/* List all modules in the GMT ${L_TAG} library to stdout */
EXTERN_MSC void gmt_${L_TAG}_module_list_all (void *API);
/* Function called by GMT_Encode_Options so developers can get information about a module */
EXTERN_MSC const char * gmt_${L_TAG}_module_info (void *API, char *candidate);

#ifdef __cplusplus
}
#endif

#endif /* !_GMT_${U_TAG}_MODULE_H */
EOF

#
# Generate FILE_CUSTOM_MODULE_C
#
echo "gmt_make_custom_code.sh: Generate ${FILE_CUSTOM_MODULE_C}" >&2

cat << EOF > ${FILE_CUSTOM_MODULE_C}
/* \$Id\$
 *
 * Copyright (c) 2016-${COPY_YEAR} by $USER
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_${L_TAG}_module.c populates the local array g_${L_TAG}_module
 * with parameters such as name, group, purpose and keys strings.
 * This file also contains the following convenience functions to
 * display all module purposes or just list their names:
 *
 *   void gmt_${L_TAG}_module_show_all (void *API);
 *   void gmt_${L_TAG}_module_list_all (void *API);
 *
 * These functions may be called by gmt --help and gmt --show-modules
 *
 * Developers of external APIs for accessing GMT modules will use this
 * function indirectly via GMT_Encode_Options to retrieve option keys
 * needed for module arg processing:
 *
 *   char * gmt_${L_TAG}_module_info (void *API, const char *module);
 *
 * DO NOT edit this file directly! Regenerate the file by running
 *	 make_custom_module_src.sh
 */

EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}
#include "gmt.h"
#include "${FILE_CUSTOM_MODULE_H}"
#include <string.h>

#ifndef gmt_M_unused
#define gmt_M_unused(x) (void)(x)
#endif
EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}

/* Sorted array with information for all ${LIB} modules */

/* name, library, and purpose for each module */
struct Gmt_moduleinfo {
	const char *name;             /* Program name */
	const char *component;        /* Component (core, supplement, custom) */
	const char *purpose;          /* Program purpose */
	const char *keys;             /* Program option info for external APIs */
};
EOF

cat << EOF >> ${FILE_CUSTOM_MODULE_C}

static struct Gmt_moduleinfo g_${L_TAG}_module[] = {
EOF

# $1 = name, $2 = ${L_TAG}, $3 = tab, $4 = purpose, $5 = tab, $6 = keys
gawk '
	BEGIN {
		FS = "\t";
	}
	{ printf "\t{\"%s\", \"%s\", %s, %s},\n", $1, $2, $4, $6;
}' /tmp/$LIB.txt >> ${FILE_CUSTOM_MODULE_C}

cat << EOF >> ${FILE_CUSTOM_MODULE_C}
	{NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
};
EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}

/* Pretty print all GMT ${LIB} module names and their purposes for gmt --help */
void gmt_${LIB}_module_show_all (void *V_API) {
	unsigned int module_id = 0;
	char message[256] = {""};
EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}
	GMT_Message (V_API, GMT_TIME_NONE, "\n=== " $LIB_STRING " ===\n");
	while (g_${L_TAG}_module[module_id].name != NULL) {
		if (module_id == 0 || strcmp (g_${L_TAG}_module[module_id-1].component, g_${L_TAG}_module[module_id].component)) {
			/* Start of new supplemental group */
			sprintf (message, "\nModule name:     Purpose of %s module:\n", g_${L_TAG}_module[module_id].component);
			GMT_Message (V_API, GMT_TIME_NONE, message);
			GMT_Message (V_API, GMT_TIME_NONE, "----------------------------------------------------------------\n");
		}
EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}
	sprintf (message, "%-16s %s\n",
		g_${L_TAG}_module[module_id].name, g_${L_TAG}_module[module_id].purpose);
		GMT_Message (V_API, GMT_TIME_NONE, message);
EOF
cat << EOF >> ${FILE_CUSTOM_MODULE_C}
		++module_id;
	}
}

/* Produce single list on stdout of all GMT ${LIB} module names for gmt --show-modules */
void gmt_${LIB}_module_list_all (void *API) {
	unsigned int module_id = 0;
	gmt_M_unused(API);
	while (g_${L_TAG}_module[module_id].name != NULL) {
		printf ("%s\n", g_${L_TAG}_module[module_id].name);
		++module_id;
	}
}

/* Lookup module id by name, return option keys pointer (for external API developers) */
const char *gmt_${L_TAG}_module_info (void *API, char *candidate) {
	int module_id = 0;
	gmt_M_unused(API);

	/* Match actual_name against g_module[module_id].name */
	while (g_${L_TAG}_module[module_id].name != NULL &&
	       strcmp (candidate, g_${L_TAG}_module[module_id].name))
		++module_id;

	/* Return Module keys or NULL */
	return (g_${L_TAG}_module[module_id].keys);
}
EOF

exit 0

# vim: set ft=c:
