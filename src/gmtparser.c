/*--------------------------------------------------------------------
 *	$Id$
 *
 *	Copyright (c) 1991-2017 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis and F. Wobbe
 *	See LICENSE.TXT file for copying and redistribution conditions.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation; version 3 or any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	Contact info: gmt.soest.hawaii.edu
 *--------------------------------------------------------------------*/
/*
 * Author:	Paul Wessel
 * Date:	15-JAN-2015
 * Version:	5 API
 *
 *  Brief synopsis: gmtparser tests API conversions, common settings,
 *  and parameters via GMT_Get_Value, GMT_Get_Default, GMT_Get_Common
 *
 */

#define THIS_MODULE_NAME	"gmtparser"
#define THIS_MODULE_LIB		"custom"
#define THIS_MODULE_PURPOSE	"Demonstrate parsing of input data, defaults, and options"
#define THIS_MODULE_KEYS	""

#include "gmt.h"		/* All programs using the GMT API needs this */
#include "custom_version.h"	/* Must include this to use Custom_version */
#include <string.h>

#define GMT_PROG_OPTIONS	"-BIJKOPRUVXYafghinorst"	/* All the GMT common options */

EXTERN_MSC int GMT_gmtparser (void *API, int mode, void *args);

static int usage (void *API, int level) {
	/* Specifies the full usage message from the program when no argument are given */
	GMT_Message (API, GMT_TIME_NONE, "%s(%s) %s - %s\n\n", THIS_MODULE_NAME, THIS_MODULE_LIB, CUSTOM_version(), THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Message (API, GMT_TIME_NONE, "usage: gmtparser [<any number of the GMT common options>]\n\n");

	if (level == GMT_SYNOPSIS) return (EXIT_FAILURE);	/* Stop here when only a hyphen is given as argument */

	GMT_Message (API, GMT_TIME_NONE, "\tgmtparser is used to demonstrate how GMT parser text.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tThere are three sections to the sequential program.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t  1. You will be asked to enter things like coordinates and dimensions (with units)\n");
	GMT_Message (API, GMT_TIME_NONE, "\t  2. You will be asked to enter names of GMT default parameter\n");
	GMT_Message (API, GMT_TIME_NONE, "\t  3. In this section the program will report which common options were set and report values.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tThe first to section ends when you enter your answer as -\n");
	GMT_Message (API, GMT_TIME_NONE, "\tgmtparser also demonstrates reporting elapsed time\n");
	GMT_Message (API, GMT_TIME_NONE, "\tCoordinates will be return in decimal degrees (if geographic) or decimal Cartesian units.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tDimensions (given in inch, cm, point) will be returned in current unit.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t  These answer thus depends on your choice of PROJ_LENGTH_UNIT [cm].\n");
	GMT_Message (API, GMT_TIME_NONE, "\tLengths (given in feet, survey feet, meter, km, miles, nautical miles) will be returned in meters.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tIn section 1, ret = ?? gives the number of parsed arguments.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tIn section 3, ret = ?? gives the number of values returned for this common options.\n");

	return (EXIT_FAILURE);
}

static void report (char *name, int count, double par[]) {
	int k;
	fprintf (stderr, "Got %s: ret = %d", name, count);
	if (count) fprintf (stderr, "\tValues:");
	for (k = 0; k < count; k++) fprintf (stderr, "\t%.12g", par[k]);
	fprintf (stderr, "\n");
}

#define Free_Options {if (GMT_Destroy_Options (API, &options) != GMT_NOERROR) return (EXIT_FAILURE);}
#define Return(code) {Free_Options; return (code);}

int GMT_gmtparser (void *API, int mode, void *args) {
	int ret, k;
	double value[100];
	char input[BUFSIZ], parameter[BUFSIZ], *commons = GMT_PROG_OPTIONS, string[2] = {0, 0};
	struct GMT_OPTION *options = NULL;		/* Linked list of program options */

	if (API == NULL) return (EXIT_FAILURE);
 	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	/* Set or get option list */

	if (!options || options->option == GMT_OPT_USAGE) Return (usage (API, GMT_USAGE));	/* Return the usage message */
	if (options->option == GMT_OPT_SYNOPSIS) Return (usage (API, GMT_SYNOPSIS));		/* Return the synopsis */

	/* Parse the given command GMT command-line options */
	if (GMT_Parse_Common (API, GMT_PROG_OPTIONS, options)) Return (EXIT_FAILURE);

	/* ---------------------------- This is the gmtparser main code ----------------------------*/

	GMT_Message (API, GMT_TIME_CLOCK, "Enter various lengths, distances, coordinates, either one-by-one or comma/slash separated.  End with - (a single hyphen):\n");
	while (scanf ("%s", input) == 1 && input[0]) {		/* As long as user provides input... */
		if (!strcmp (input, "-")) break;		/* OK, the end signal */
		ret = GMT_Get_Value (API, input, value);	/* Convert one or more strings to doubles */
		report (input, ret, value);			/* Report them as decimal values */
	}
	
	GMT_Message (API, GMT_TIME_CLOCK, "Enter name of GMT default parameter.  End with -:\n");
	while (scanf ("%s", input) == 1 && input[0]) {		/* As long as user provides input... */
		if (!strcmp (input, "-")) break;		/* OK, the end signal */
		ret = GMT_Get_Default (API, input, parameter);	/* Look up the string value */
		if (ret == 0) fprintf (stderr, "%20s = %s\n", input, parameter);	/* Print the string */
	}
	
	GMT_Message (API, GMT_TIME_CLOCK, "These are the GMT common options and values you specified:\n");
	for (k = 0; k < strlen (commons); k++) {	/* Check every common option this module knows about */
		if ((ret = GMT_Get_Common (API, commons[k], value)) == -1) continue;
		string[0] = commons[k];
		report (string, ret, value);
	}
	
	/* Destroy options and let GMT garbage collection free used memory */
	Return (GMT_NOERROR);
}
