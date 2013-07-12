#!/bin/bash
#
# $Id: gmt_make_module_src.sh 11889 2013-07-03 01:33:58Z pwessel $
#
# Copyright (c) 2012-2013
# by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
# See LICENSE.TXT file for copying and redistribution conditions.
#
# Below, <X> is either core, suppl, or a users custom shared lib tag
#
# This script will find all the C files in the current dir (if core)
# or in subdirs (if suppl) and extract all the THIS_MODULE_PURPOSE
# and other strings from the sources files, then create two files:
# gmt_<X>_module.h and gmt_<X>_module.c.
#
# Note: gmt_<X>_module.h and gmt_<X>_module.c are in svn.  Only
# retrun this script when there are changes in the code.
#

if [ $# -ne 1 ]; then
cat << EOF
usage: gmt_make_module_src.sh [tag]
	tag is the name of the set of modules for your custom extension.
EOF
	exit 0
fi
set -e

LIB=$1
# Make sure we get both upper- and lower-case versions of the tag
U_TAG=`echo $LIB | tr '[a-z]' '[A-Z]'`
L_TAG=`echo $LIB | tr '[A-Z]' '[a-z]'`

# Look in current dir (for core or users)
grep "#define THIS_MODULE_LIB		\"$L_TAG\"" *.c | awk -F: '{print $1}' | sort > /tmp/tmp.lis

rm -f /tmp/NAME.lis /tmp/LIB.lis /tmp/PURPOSE.lis /tmp/all.lis
while read program; do
	grep "#define THIS_MODULE_NAME" $program    | awk '{print $3}' | sed -e 's/"//g' >> /tmp/NAME.lis
	grep "#define THIS_MODULE_LIB" $program     | awk '{print $3}' | sed -e 's/"//g' >> /tmp/LIB.lis
	grep "#define THIS_MODULE_PURPOSE" $program | sed -e 's/#define THIS_MODULE_PURPOSE//g' | awk '{print $0}' >> /tmp/PURPOSE.lis
done < /tmp/tmp.lis
# Prepend group+name so we can get a list sorted on group name then individual programs
paste /tmp/LIB.lis /tmp/NAME.lis | awk '{printf "%s%s|%s\n", $1, $2, $2}' > /tmp/SORT.txt
paste /tmp/SORT.txt /tmp/LIB.lis /tmp/PURPOSE.lis | sort -k1 > /tmp/SORTED.txt
awk -F"|" '{print $2}' /tmp/SORTED.txt > /tmp/$LIB.txt
rm -f /tmp/tmp.lis /tmp/NAME.lis /tmp/LIB.lis /tmp/PURPOSE.lis /tmp/SORTED.txt /tmp/SORT.txt

# The file with the message for each shared library
FILE_MODULEINFO=gmt_custom_moduleinfo.txt
LIB_STRING=`grep LIB_STRING $FILE_MODULEINFO | grep ${L_TAG} | awk -F= '{print $NF}'`

# The two output files produced
FILE_GMT_MODULE_C=gmt_${L_TAG}_module.c
FILE_GMT_MODULE_H=gmt_${L_TAG}_module.h
COPY_YEAR=$(date +%Y)
#
# Generate FILE_GMT_MODULE_H
#

cat << EOF > ${FILE_GMT_MODULE_H}
/* \$Id\$
 *
 * Copyright (c) 2012-${COPY_YEAR}
 * by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_${L_TAG}_module.h declares the prototypes for ${L_TAG} module functions
 * and the array that contains ${L_TAG} GMT module parameters such as name
 * and purpose strings.
 * DO NOT edit this file directly! Instead edit $FILE_MODULEINFO
 * and regenerate this file with gmt_make_module_src.sh ${L_TAG}. */

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
gawk '{printf "EXTERN_MSC int GMT_%s (void *API, int mode, void *args);\n", $1;}' /tmp/$LIB.txt >> ${FILE_GMT_MODULE_H}
cat << EOF >> ${FILE_GMT_MODULE_H}

/* Pretty print all modules in the GMT ${L_TAG} library and their purposes */
EXTERN_MSC void gmt_${L_TAG}_module_show_all (void *API);

#ifdef __cplusplus
}
#endif

#endif /* !_GMT_${U_TAG}_MODULE_H */
EOF

#
# Generate FILE_GMT_MODULE_C
#

cat << EOF > ${FILE_GMT_MODULE_C}
/* \$Id\$
 *
 * Copyright (c) 2012-${COPY_YEAR}
 * by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis, and F. Wobbe
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_${L_TAG}_module.c populates the external array of GMT ${L_TAG}
 * module parameters such as name, group and purpose strings.
 * This file also contains the following convenience function to
 * display all module purposes:
 *
 *   void gmt_${L_TAG}_module_show_all (struct GMTAPI_CTRL *API);
 *
 * DO NOT edit this file directly! Instead edit $FILE_MODULEINFO
 * and regenerate this file with gmt_make_module_src.sh ${L_TAG} */

#include "gmt.h"
#include <string.h>

/* Sorted array with information for all GMT ${L_TAG} modules */

/* name, library, and purpose for each module */
struct Gmt_moduleinfo {
	const char *name;             /* Program name */
	const char *component;        /* Component (core, supplement, custom) */
	const char *purpose;          /* Program purpose */
};

struct Gmt_moduleinfo g_${L_TAG}_module[] = {
EOF

# $1 = name, $2 = ${L_TAG}, $3 = tab, $4 = purpose
gawk '
	BEGIN {
		FS = "\t";
	}
	{ printf "\t{\"%s\", \"%s\", %s},\n", $1, $2, $4;
}' /tmp/$LIB.txt >> ${FILE_GMT_MODULE_C}

cat << EOF >> ${FILE_GMT_MODULE_C}
	{NULL, NULL, NULL} /* last element == NULL detects end of array */
};

/* Pretty print all GMT ${L_TAG} module names and their purposes */
void gmt_${L_TAG}_module_show_all (void *API) {
	unsigned int module_id = 0;
	char message[256];

	GMT_Message (API, GMT_TIME_NONE, "\n=== " $LIB_STRING " ===\n");
	while (g_${L_TAG}_module[module_id].name != NULL) {
		if (module_id == 0 || strcmp (g_${L_TAG}_module[module_id-1].component, g_${L_TAG}_module[module_id].component)) {
			/* Start of new supplemental group */
			sprintf (message, "\nModule name:     Purpose of %s module:\n", g_${L_TAG}_module[module_id].component);
			GMT_Message (API, GMT_TIME_NONE, message);
			GMT_Message (API, GMT_TIME_NONE, "----------------------------------------------------------------\n");
		}
		sprintf (message, "%-16s %s\n",
			g_${L_TAG}_module[module_id].name, g_${L_TAG}_module[module_id].purpose);
		GMT_Message (API, GMT_TIME_NONE, message);
		++module_id;
	}
}
EOF

exit 0

# vim: set ft=c:
