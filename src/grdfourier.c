/*--------------------------------------------------------------------
 *	$Id$
 *
 *	Copyright (c) 1991-2016 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis and F. Wobbe
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
 *  Brief synopsis: grdfourier.c is a dummy program to show how a
 *  user code could read a grid or create on from -R -I [-r], add a spike,
 *  perform a filtering operation in the frequency domain and then write
 *  the modified grid to a file.
 *
 */

#define THIS_MODULE_NAME	"grdfourier"
#define THIS_MODULE_LIB		"custom"
#define THIS_MODULE_PURPOSE	"Create a grid, add a spike, filter it in frequency domain, and write output"
#define THIS_MODULE_KEYS	"<GI,GGO,RG-"

#define MY_FFT_DIM	2	/* Dimension of FFT needed */
#include "gmt.h"		/* All programs using the GMT API needs this */
#include "custom_version.h"	/* Must include this to use Custom_version */

/* Add any other include files needed by your program */
#include <math.h>
#include <string.h>
#ifndef M_PI
#define M_PI          3.14159265358979323846
#endif
#define GMT_PROG_OPTIONS	"-VRIfr"	/* List the GMT options your program may need */

EXTERN_MSC int GMT_grdfourier (void *API, int mode, void *args);

struct GMT_GRDFOURIER_CTRL {	/* Here is where you collect your programs specific options */
	struct In {	/* Input grid file */
		unsigned int active;	/* 1 if this option was specified */
		char *file;	/* Name of input grid file */
	} In;
	struct A {	/* -A<row/col> specifies location where a spike will be added */
		unsigned int active;	/* 1 if this option was specified */
		unsigned int row, col;	/* Location in the grid for spike */
	} A;
	struct D {	/* -D<dir> sets which wavenumbers to use (r|x|y) */
		unsigned int active;	/* 1 if this option was specified */
		char dir;	/* 0, 1, or 2 */
	} D;
	struct F {	/* -F<width> sets Gaussian filter width */
		unsigned int active;	/* 1 if this option was specified */
		double width;	/* Width Gaussian filter */
	} F;
	struct G {	/* -G<outfile> sets the output file name */
		unsigned int active;	/* 1 if this option was specified */
		char *file;	/* The filename */
	} G;
	struct N {	/* -N[f|q|s<nx>/<ny>][+e|m|n][+t<width>][+w[<suffix>]][+z[p]] */
		unsigned int active;	/* 1 if this option was specified */
		void *info;	/* Provided by the API */
	} N;
};

static struct GMT_GRDFOURIER_CTRL * New_Ctrl (void *API) {	/* Allocate and initialize a new control structure for your program*/
	struct GMT_GRDFOURIER_CTRL *C = NULL;

	C = calloc (1, sizeof (struct GMT_GRDFOURIER_CTRL));

	/* Initialize values whose defaults are not 0/false/NULL */

	C->D.dir = 'r';		/* Default is radial wavenumbers */
	C->F.width = 100000.0;	/* Default for -F is 100 km */
	return (C);
}

static void Free_Ctrl (void *API, struct GMT_GRDFOURIER_CTRL *C) {	/* Free memory used by Ctrl and deallocate it */
	if (!C) return;
	if (C->In.file) free (C->In.file);	
	if (C->G.file)  free (C->G.file);	
	if (C->N.info)  GMT_FFT_Destroy (API, C->N.info);
	free (C);	
}

static int usage (void *API, int level) {
	/* Specifies the full usage message from the program when no argument are given */
	GMT_Message (API, GMT_TIME_NONE, "%s(%s) %s - %s\n\n", THIS_MODULE_NAME, THIS_MODULE_LIB, CUSTOM_version(), THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Message (API, GMT_TIME_NONE, "usage: grdfourier -G<outgrid> [<ingrid> ][-I<xinc>[/<yinc>]] \n");
	GMT_Message (API, GMT_TIME_NONE, "	[-R<xmin/xmax/ymin/ymax>] [-A<row/col>] [-D<dir>] [-F<width>]\n\n");

	if (level == GMT_SYNOPSIS) return (EXIT_FAILURE);	/* Stop here when only a hyphen is given as argument */

	GMT_Message (API, GMT_TIME_NONE, "\t-G filename for output netCDF grid file.\n");
	GMT_Message (API, GMT_TIME_NONE, "\tOPTIONS:\n");
	GMT_Message (API, GMT_TIME_NONE, "\t<ingrid> is an optional grid file to start with instead of -R -I [-r].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-A Specify a row,col pair indicating where to place a unit impulse [in the middle].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-D Direction for filter: x, y, or r [r]\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-F Specify width for Gaussian filter exp {-(x/width)^2} [100k]\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-I To create a new grid, specify increments <xinc>[/<yinc>].\n");
	/* All programs needing the GMT FFT machinery must display the FFT option. Call it N unless taken.
	 * Pass the dimension of the FFT work (1 for tables, 2 for grids) */
	GMT_FFT_Option (API, 'N', MY_FFT_DIM, "Choose or inquire about suitable grid dimensions for FFT, and set modifiers:");
	GMT_Message (API, GMT_TIME_NONE, "\t-R To create a new grid, specify region <xmin/xmax/ymin/ymax>.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-r Select pixel registration for new grid.\n");

	return (EXIT_FAILURE);
}

static int parse (void *API, struct GMT_GRDFOURIER_CTRL *Ctrl, struct GMT_OPTION *options) {
	/* This parses the options provided to grdfourier and sets parameters in Ctrl.
	 * Note: Ctrl has already been initialized and non-zero default values set.
	 * Any GMT common options will override values set previously by other commands.
	 */
	int ret;
	unsigned int n_errors = 0;	/* Keep track of parsing errors */
	double value[2];
	struct GMT_OPTION *opt = NULL;	/* Loop variable pointing to the current option */

	for (opt = options; opt; opt = opt->next) {	/* Process all the options given */
		if (strchr (GMT_PROG_OPTIONS, opt->option)) continue;	/* Skip GMT common options */
		switch (opt->option) {
			case '<':	/* Input file */
				if (Ctrl->In.active) {
					GMT_Message (API, GMT_TIME_NONE, "Syntax error: Only one input file allowed\n");
				}
				else {
					Ctrl->In.active = 1;
					Ctrl->In.file = malloc (strlen (opt->arg) + 1);
					strcpy (Ctrl->In.file, opt->arg);
				}
				break;
			case 'A':	/* Location of spike */
				Ctrl->A.active = 1;
				if ((ret = GMT_Get_Value (API, opt->arg, value)) == 2) {
					Ctrl->A.row = (unsigned int)value[0];
					Ctrl->A.col = (unsigned int)value[1];
				}
				else {
					GMT_Message (API, GMT_TIME_NONE, "Syntax error: Must give row/col pair\n");
					n_errors ++;
				}
				break;
			case 'D':	/* Select wavenumber direction */
				Ctrl->D.active = 1;
				Ctrl->D.dir = opt->arg[0];
				break;
			case 'F':	/* Gaussian filter width */
				Ctrl->F.active = 1;
				if ((ret = GMT_Get_Value (API, opt->arg, value)) == 1) Ctrl->F.width = value[0];
				break;
			case 'G':	/* Output file */
				Ctrl->G.active = 1;
				Ctrl->G.file = malloc (strlen (opt->arg) + 1);
				strcpy (Ctrl->G.file, opt->arg);
				break;
			case 'N':	/* Grid dimension setting or inquiery */
				Ctrl->N.active = 1;
				if ((Ctrl->N.info = GMT_FFT_Parse (API, 'N', MY_FFT_DIM, opt->arg)) == NULL) n_errors ++;
				break;
			default:	/* Report bad options */
				GMT_Message (API, GMT_TIME_NONE, "Syntax error: Unrecognized option %c%s\n", opt->option, opt->arg);
				n_errors ++;
				break;
		}
	}

	if (!Ctrl->G.active) GMT_Message (API, GMT_TIME_NONE, "Syntax error: Must specify output file\n"), n_errors++;
	if (Ctrl->F.active && Ctrl->F.width <= 0.0) GMT_Message (API, GMT_TIME_NONE, "Syntax error -F: Must specify a positive width\n"), n_errors++;

	return (n_errors);
}

/* Convenience macros to free memory before exiting due to error or completion */
#define Free_Options {if (GMT_Destroy_Options (API, &options) != GMT_NOERROR) return (EXIT_FAILURE);}
#define bailout(code) {Free_Options; return (code);}
#define Return(code) {Free_Ctrl (API, Ctrl); bailout (code);}

int GMT_grdfourier (void *API, int mode, void *args) {
	/* 1. Define local variables */
	int error;
	unsigned int wn_mode = 0;			/* To select radial [0], x (1), or y (2) wavenumber */
	unsigned int rw_mode;				/* Mode to pass when reading or creating grid */
	uint64_t re, im, node;				/* Indeces into grids should be of this type */
	double k, filter, k_ref;			/* Normally all math is done in double */
	double *x = NULL, *y = NULL;			/* Coordinate arrays for the grid */
	struct GMT_GRID *Grid = NULL;			/* This will be pointer to our grid */
	void *FFT_info = NULL;				/* Holds information about all things FFT related */
	struct GMT_GRDFOURIER_CTRL *Ctrl = NULL;	/* Control for this program */
	struct GMT_OPTION *options = NULL;		/* Linked list of program options */

	if (API == NULL) return (EXIT_FAILURE);
 	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	/* Set or get option list */

	if (!options || options->option == GMT_OPT_USAGE) bailout (usage (API, GMT_USAGE));	/* Return the usage message */
	if (options->option == GMT_OPT_SYNOPSIS) bailout (usage (API, GMT_SYNOPSIS));		/* Return the synopsis */

	/* Parse the commont GMT command-line options */
	if (GMT_Parse_Common (API, GMT_PROG_OPTIONS, options)) Return (EXIT_FAILURE);

	/* Allocate Ctrl and parse program-specific options */
	Ctrl = New_Ctrl (API);	/* Allocate and initialize a new control structure */
	if ((error = parse (API, Ctrl, options))) Return (error);

	/* ---------------------------- This is the grdfourier main code ----------------------------*/

	rw_mode = GMT_GRID_ALL | GMT_GRID_IS_COMPLEX_REAL;	/* Place our grid as the real component in a complex grid */
	if (Ctrl->In.active) {	/* User specified an input grid file */
		GMT_Message (API, GMT_TIME_CLOCK, "Read input grid from %s\n", Ctrl->In.file);
		if ((Grid = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, rw_mode, NULL, Ctrl->In.file, NULL)) == NULL)
			Return (EXIT_FAILURE);
	}
	else {	/* Create an empty grid from current -R -I [-r] instead */
		GMT_Message (API, GMT_TIME_CLOCK, "No grid provided, create an empty grid from current -R -I [-r] settings\n");
		if ((Grid = GMT_Create_Data (API, GMT_IS_GRID, GMT_IS_SURFACE, rw_mode, NULL, NULL, NULL, \
			GMT_GRID_DEFAULT_REG, 0, NULL)) == NULL) Return (EXIT_FAILURE);
	}

	x = GMT_Get_Coord (API, GMT_IS_GRID, GMT_X, Grid);	/* Get array of x coordinates */
	y = GMT_Get_Coord (API, GMT_IS_GRID, GMT_Y, Grid);	/* Get array of y coordinates */

	if (!Ctrl->A.active) {	/* We know the grid dimension so we can select the mid point */
		Ctrl->A.row = Grid->header->ny / 2;
		Ctrl->A.col = Grid->header->nx / 2;
	}
	if (Ctrl->A.row >= Grid->header->ny || Ctrl->A.col >= Grid->header->nx) {
		GMT_Message (API, GMT_TIME_CLOCK, "Spike is placed outside the grid! We give up.\n");
		Return (EXIT_FAILURE);
	}
	
	/* Place our spike at the desired location; 2 * since grid is complex */
	node = 2 * GMT_Get_Index (API, Grid->header, Ctrl->A.row, Ctrl->A.col);
	Grid->data[node] = 1.0;	/* The deadly spike */
	GMT_Message (API, GMT_TIME_CLOCK, "Placed spike at %g, %g [col = %u, row = %u]\n", x[Ctrl->A.col], y[Ctrl->A.row], Ctrl->A.col, Ctrl->A.row);
	
	/* Initialize FFT structs, check for NaNs, detrend, save intermediate files, etc., per -N settings */
	
	FFT_info = GMT_FFT_Create (API, Grid, MY_FFT_DIM, GMT_GRID_IS_COMPLEX_REAL, Ctrl->N.info);
	
	switch (Ctrl->D.dir) {	/* Select which type of wavenumber to use */
		case 'x': wn_mode = 0; break;
		case 'y': wn_mode = 1; break;
		case 'r': wn_mode = 2; break;
	}
	GMT_Message (API, GMT_TIME_CLOCK, "Using wavenumbers in the %c direction [wn_mode = %u]\n", Ctrl->D.dir, wn_mode);

	/* Take the forward FFT */
	if (GMT_FFT (API, Grid, GMT_FFT_FWD, GMT_FFT_COMPLEX, FFT_info)) Return (EXIT_FAILURE);

	/* Now do operations in frequency domain.  Here we are just filtering our spike  */
	
	k_ref = 2.0 * M_PI / Ctrl->F.width;	/* Filter is exp (-0.5*(k/k_ref)^2) */
	
	/* Grid->data contains Grid->header->size values with {real, imag} in adjacent positions.  Typically,
	 * you will loop over all of these as below, and obtain the wavenumber using either the real or imaginary
	 * loop variable.  This indirectly loops over all the frequencies in the grid. */
	
	for (re = 0, im = 1; re < Grid->header->size; re += 2, im += 2) {	/* Loop over the entire complex grid */
		k = GMT_FFT_Wavenumber (API, re, wn_mode, FFT_info);	/* Get chosen wavenumber */
		filter = exp (-pow (k/k_ref, 2.0));	/* Compute filter for this wavenumber */
		Grid->data[re] *= filter;		/* Filter real component */
		Grid->data[im] *= filter;		/* Filter imag component */
	}

	/* Take the inverse FFT; the 2/nm scaling is taken care of automatically */
	if (GMT_FFT (API, Grid, GMT_FFT_INV, GMT_FFT_COMPLEX, FFT_info)) Return (EXIT_FAILURE);

	/* Time to write our data out */
	if (GMT_Write_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, rw_mode, NULL, Ctrl->G.file, Grid)) {
		Return (EXIT_FAILURE);
	}

	GMT_FFT_Destroy (API, &FFT_info);	/* Free the FFT machinery */

	/* Destroy options and let GMT garbage collection free memory used byt the API */

	Return (GMT_NOERROR);
}
