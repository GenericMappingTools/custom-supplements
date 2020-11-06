/*
 * Copyright (c) 2012-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 * See LICENSE.TXT file for copying and redistribution conditions.
 */
/* gmt_custom_glue.c populates the external array of this shared lib with
 * module parameters such as name, group, purpose and keys strings.
 * This file also contains the following convenience functions to
 * display all module purposes, list their names, or return keys or group:
 *
 *   int custom_module_show_all    (void *API);
 *   int custom_module_list_all    (void *API);
 *   int custom_module_classic_all (void *API);
 *
 * These functions may be called by gmt --help and gmt --show-modules
 *
 * Developers of external APIs for accessing GMT modules will use this
 * function indirectly via GMT_Encode_Options to retrieve option keys
 * needed for module arg processing:
 *
 *   const char * custom_module_keys  (void *API, char *candidate);
 *   const char * custom_module_group (void *API, char *candidate);
 *
 * All functions are exported by the shared custom library so that gmt can call these
 * functions by name to learn about the contents of the library.
 */

#include "gmt_dev.h"

/* Sorted array with information for all GMT custom modules */
static struct GMT_MODULEINFO modules[] = {
	{"gmtaverage", "gmtaverage", "custom", "Block average (x,y,z) data tables by mean, median, or mode estimation", "<DI,>DO,RG-"},
	{"gmtmercmap", "gmtmercmap", "custom", "Make a Mercator color map from ETOPO 1, 2, or 5 arc min global relief grids", "CCi,>XO,RG-"},
	{"gmtparser", "gmtparser", "custom", "Demonstrate parsing of input data, defaults, and options", ""},
	{"grdfourier", "grdfourier", "custom", "Create a grid, add a spike, filter it in frequency domain, and write output", "<GI,GGO,RG-"},
	{NULL, NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
};

/* Pretty print all shared module names and their purposes for gmt --help */
EXTERN_MSC int custom_module_show_all (void *API) {
	return (GMT_Show_ModuleInfo (API, modules, "GMT custom: The third-party supplements to the Generic Mapping Tools", GMT_MODULE_HELP));
}

/* Produce single list on stdout of all shared module names for gmt --show-modules */
EXTERN_MSC int custom_module_list_all (void *API) {
	return (GMT_Show_ModuleInfo (API, modules, NULL, GMT_MODULE_SHOW_MODERN));
}

/* Produce single list on stdout of all shared module names for gmt --show-classic [i.e., classic mode names] */
EXTERN_MSC int custom_module_classic_all (void *API) {
	return (GMT_Show_ModuleInfo (API, modules, NULL, GMT_MODULE_SHOW_CLASSIC));
}

/* Lookup module id by name, return option keys pointer (for external API developers) */
EXTERN_MSC const char *custom_module_keys (void *API, char *candidate) {
	return (GMT_Get_ModuleInfo (API, modules, candidate, GMT_MODULE_KEYS));
}

/* Lookup module id by name, return group char name (for external API developers) */
EXTERN_MSC const char *custom_module_group (void *API, char *candidate) {
	return (GMT_Get_ModuleInfo (API, modules, candidate, GMT_MODULE_GROUP));
}
