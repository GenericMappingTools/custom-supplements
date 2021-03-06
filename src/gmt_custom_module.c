/*
 * Copyright (c) 2012-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_custom_module.c populates the external array of GMT custom with
 * module parameters such as name, group, purpose and keys strings.
 * This file also contains the following convenience functions to
 * display all module purposes or just list their names:
 *
 *   void gmt_custom_module_show_all (struct GMTAPI_CTRL *API);
 *   void gmt_custom_module_list_all (void *API);
 *
 * These functions may be called by gmt --help and gmt --show-modules
 *
 * Developers of external APIs for accessing GMT modules will use this
 * function indirectly via GMT_Encode_Options to retrieve option keys
 * needed for module arg processing:
 *
 *   char * gmt_custom_module_keys (void *API, const char *module);
 *
 * DO NOT edit this file directly! Regenerate the file by running
 * 	gmt_make_module_src.sh custom
 */
#include "gmt.h"
#include "gmt_notposix.h"       /* Non-POSIX extensions */
#define gmt_M_unused(x) (void)(x)
#define GMT_LEN256 256
#include "gmt_supplements_module.h"
#include <string.h>

/* Sorted array with information for all GMT custom modules */

/* name, library, and purpose for each module */
struct Gmt_moduleinfo {
	const char *mname;            /* Program (modern) name */
	const char *cname;            /* Program (classic) name */
	const char *component;        /* Component (core, supplement, custom) */
	const char *purpose;          /* Program purpose */
	const char *keys;             /* Program option info for external APIs */
};

static int sort_on_classic (const void *vA, const void *vB) {
	const struct Gmt_moduleinfo *A = vA, *B = vB;
	if (A == NULL) return +1;	/* Get the NULL entry to the end */
	if (B == NULL) return -1;	/* Get the NULL entry to the end */
	return strcmp(A->cname, B->cname);
}

static struct Gmt_moduleinfo g_custom_module[] = {
	{"gmtaverage", "gmtaverage", "custom", , "Block average (x,y,z) data tables by mean, median, or mode estimation"},
	{"gmtmercmap", "gmtmercmap", "custom", , },
	{"gmtparser", "gmtparser", "custom", , },
	{"grdfourier", "grdfourier", "custom", , },
	{NULL, NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
};

/* Pretty print all GMT custom module names and their purposes for gmt --help */
void gmt_custom_module_show_all (void *V_API) {
	unsigned int module_id = 0;
	char message[GMT_LEN256];
	GMT_Message (V_API, GMT_TIME_NONE, "\n===  GMT custom: The custom modules of the Generic Mapping Tools  ===\n");
	while (g_custom_module[module_id].cname != NULL) {
		if (module_id == 0 || strcmp (g_custom_module[module_id-1].component, g_custom_module[module_id].component)) {
			/* Start of new supplemental group */
			snprintf (message, GMT_LEN256, "\nModule name:     Purpose of %s module:\n", g_custom_module[module_id].component);
			GMT_Message (V_API, GMT_TIME_NONE, message);
			GMT_Message (V_API, GMT_TIME_NONE, "----------------------------------------------------------------\n");
		}
		snprintf (message, GMT_LEN256, "%-16s %s\n",
			g_custom_module[module_id].mname, g_custom_module[module_id].purpose);
			GMT_Message (V_API, GMT_TIME_NONE, message);
		++module_id;
	}
}

/* Produce single list on stdout of all GMT custom module names for gmt --show-modules */
void gmt_custom_module_list_all (void *V_API) {
	unsigned int module_id = 0;
	gmt_M_unused(V_API);
	while (g_custom_module[module_id].cname != NULL) {
		printf ("%s\n", g_custom_module[module_id].mname);
		++module_id;
	}
}

/* Produce single list on stdout of all GMT custom module names for gmt --show-classic [i.e., classic mode names] */
void gmt_custom_module_classic_all (void *V_API) {
	unsigned int module_id = 0;
	size_t n_modules = 0;
	gmt_M_unused(V_API);

	while (g_custom_module[n_modules].cname != NULL)	/* Count the modules */
		++n_modules;

	/* Sort array on classic names since original array is sorted on modern names */
	qsort (g_custom_module, n_modules, sizeof (struct Gmt_moduleinfo), sort_on_classic);

	while (g_custom_module[module_id].cname != NULL) {
		printf ("%s\n", g_custom_module[module_id].cname);
		++module_id;
	}
}

/* Lookup module id by name, return option keys pointer (for external API developers) */
const char *gmt_custom_module_keys (void *API, char *candidate) {
	int module_id = 0;
	gmt_M_unused(API);

	/* Match actual_name against g_module[module_id].cname */
	while (g_custom_module[module_id].cname != NULL &&
	       strcmp (candidate, g_custom_module[module_id].cname))
		++module_id;

	/* Return Module keys or NULL */
	return (g_custom_module[module_id].keys);
}

/* Lookup module id by name, return group char name (for external API developers) */
const char *gmt_custom_module_group (void *API, char *candidate) {
	int module_id = 0;
	gmt_M_unused(API);

	/* Match actual_name against g_module[module_id].cname */
	while (g_custom_module[module_id].cname != NULL &&
	       strcmp (candidate, g_custom_module[module_id].cname))
		++module_id;

	/* Return Module keys or NULL */
	return (g_custom_module[module_id].component);
}
