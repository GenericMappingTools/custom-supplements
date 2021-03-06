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
 * Date:	5-MAY-2017
 * Version:	5 API
 *
 * Brief synopsis: gmtmercmap will make a nice Mercator map using etopo[1|2|5].
 * It also serves as a demonstration of the GMT5 API.
 *
 */

#include "gmt_dev.h"

#define THIS_MODULE_CLASSIC_NAME	"gmtmercmap"
#define THIS_MODULE_MODERN_NAME		"gmtmercmap"
#define THIS_MODULE_LIB			"custom"
#define THIS_MODULE_PURPOSE		"Make a Mercator color map from ETOPO 1, 2, or 5 arc min global relief grids"
#define THIS_MODULE_KEYS		"CCi,>XO,RG-"
#define THIS_MODULE_NEEDS		"JR"
#define THIS_MODULE_OPTIONS		"->BKOPRUVXYcnptxy"

#include "custom_version.h"	/* Must include this to use Custom_version */

#define MAP_BAR_GAP	"36p"	/* Offset color bar 36 points below map */
#define MAP_BAR_HEIGHT	"8p"	/* Height of color bar, if used */
#define MAP_OFFSET	"100p"	/* Start map 100p from paper edge when colorbar is requested */
#define TOPO_INC	500.0	/* Build cpt in steps of 500 meters */

EXTERN_MSC int GMT_gmtmercmap (void *API, int mode, void *args);

enum enum_script {BASH_MODE = 0,	/* Write Bash script */
	CSH_MODE,				/* Write C-shell script */
	DOS_MODE};				/* Write DOS script */
	
/* Control structure for gmtmercmap */

struct GMTMERCMAP_CTRL {
	struct C {	/* -C<cptfile> */
		unsigned int active;
		char *file;
	} C;
	struct D {	/* -D[b|c|d] */
		unsigned int active;
		int mode;
	} D;
	struct E {	/* -E[1|2|5] */
		unsigned int active;
		int mode;
	} E;
	struct W {	/* -W<width> */
		unsigned int active;
		double width;
	} W;
	struct S {	/* -S */
		unsigned int active;
	} S;
};

static void *New_Ctrl (unsigned int length_unit) {	/* Allocate and initialize a new control structure */
	struct GMTMERCMAP_CTRL *C;

	C = calloc (1, sizeof (struct GMTMERCMAP_CTRL));
	C->C.file = strdup ("earth");
	C->W.width = (length_unit == 0) ? 25.0 : ((length_unit == 1) ? 10.0 : 700);	/* 25cm (SI/A4) or 10i (US/Letter) or 700pt */
	return (C);
}

static void Free_Ctrl (struct GMTMERCMAP_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	if (C->C.file) free (C->C.file);
	free ((void*)C);
}

static int usage (void *API, unsigned int length_unit, int level) {
	char width[8];
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	if (length_unit == 0)
		strcpy (width, "25c");
	else if (length_unit == 1)
		strcpy (width, "10i");
	else
		strcpy (width, "700p");
	GMT_Message (API, GMT_TIME_NONE, "usage: %s [-C<cpt>] [-D[b|c|d]] [-E1|2|5] [-K] [-O] [-P]\n\t[%s] [-S] [%s] [%s]\n", name, GMT_R2_OPT, GMT_U_OPT, GMT_V_OPT);
	GMT_Message (API, GMT_TIME_NONE, "\t[-W<width>] [%s] [%s] [%s]\n\t[%s]\n\t[%s] [%s]\n\n", GMT_X_OPT, GMT_Y_OPT, GMT_c_OPT, GMT_n_OPT, GMT_p_OPT, GMT_t_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "\n\tOPTIONS:\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-C Color palette to use [relief].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-D Dry-run: Print equivalent GMT commands instead; no map is made.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t   Append b, c, or d for Bourne shell, C-shell, or DOS syntax [Default is Bourne].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-E Force the ETOPO resolution chosen [auto].\n");
	GMT_Option (API, "K,O,P");
	GMT_Message (API, GMT_TIME_NONE, "\t-R sets the map region [Default is -180/180/-75/75].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-S plot a color scale beneath the map [none].\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-W Specify the width of your map [%s].\n", width);
	GMT_Option (API, "U,V,X,c,n,p,t,.");

	return (GMT_MODULE_USAGE);
}

static int parse (void *API, struct GMTMERCMAP_CTRL *Ctrl, struct GMT_OPTION *options)
{
	/* This parses the options provided to gmtmercmap and sets parameters in Ctrl.
	 * Note Ctrl has already been initialized and non-zero default values set.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */

	unsigned int n_errors = 0;
	struct GMT_OPTION *opt = NULL;

	for (opt = options; opt; opt = opt->next) {	/* Process all the options given */
		if (strchr (THIS_MODULE_OPTIONS, opt->option)) continue;	/* Common options already processed */

		switch (opt->option) {
			/* Processes program-specific parameters */

			case 'C':	/* CPT master file */
				Ctrl->C.active = 1;
				free (Ctrl->C.file);
				Ctrl->C.file = strdup (opt->arg);
				break;
			case 'D':	/* Just issue equivalent GMT commands in a script */
				Ctrl->D.active = 1;
				switch (opt->arg[0]) {
					case 'b':  Ctrl->D.mode = BASH_MODE; break;
					case 'c':  Ctrl->D.mode = CSH_MODE;  break;
					case 'd':  Ctrl->D.mode = DOS_MODE;  break;
					default:   Ctrl->D.mode = BASH_MODE; break;
				}
				break;
			case 'E':	/* Select the ETOPO model to use */
				Ctrl->E.active = 1;
				switch (opt->arg[0]) {
					case '1':  Ctrl->E.mode = 1; break;
					case '2':  Ctrl->E.mode = 2;  break;
					case '5':  Ctrl->E.mode = 5;  break;
					default:   n_errors++; break;
				}
				break;
			case 'W':	/* Map width */
				Ctrl->W.active = 1;
				GMT_Get_Value (API, opt->arg, &Ctrl->W.width);
				break;
			case 'S':	/* Draw scale beneath map */
				Ctrl->S.active = 1;
				break;

			default:	/* Report bad options */
				GMT_Report (API, GMT_MSG_NORMAL, "Syntax error: Unrecognized argument %c%s\n", opt->option, opt->arg);
				n_errors++;
				break;
		}
	}

	return (n_errors);
}

#define ETOPO1M_LIMIT 100	/* ETOPO1 cut-offs in degrees squared for 1 arc min */
#define ETOPO2M_LIMIT 10000	/* ETOPO2 cut-offs in degrees squared for 2 arc min */

static void set_var (int mode, char *name, char *value)
{	/* Assigns the text variable given the script mode */
	switch (mode) {
		case BASH_MODE: printf ("%s=%s\n", name, value); break;
		case CSH_MODE:  printf ("set %s = %s\n", name, value); break;
		case DOS_MODE:  printf ("set %s=%s\n", name, value); break;
	}
}

static void set_dim (int mode, unsigned int length_unit, char *name, double value)
{	/* Assigns the double value given the script mode and prevailing measure unit [value is passed in inches] */
	static char unit[3] = "cip", text[256];
	double out = value;
	sprintf (text, "%g%c", out, unit[length_unit]);
	set_var (mode, name, text);
}

void place_var (int mode, char *name, unsigned int end)
{	/* Prints this variable to stdout where needed in the script via the assignment static variable */
	if (mode == DOS_MODE)
		printf ("%%%s%%", name);
	else
		printf ("${%s}", name);
	if (end) putchar ('\n');
}

#define M_free_options(mode) {if (mode >= 0 && GMT_Destroy_Options (API, &options) != GMT_OK) exit (GMT_MEMORY_ERROR);}
#define bailout(code) {M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (Ctrl); bailout (code);}

int GMT_gmtmercmap (void *API, int mode, void *args) {
	int error, min;
	unsigned int B_active, K_active, O_active, P_active, X_active, Y_active;
	unsigned int length_unit = 0;	/* cm */
	
	double area, z, z_min, z_max, wesn[4];
	
	char file[256], z_file[GMT_STR16], i_file[GMT_STR16];
	char cmd[BUFSIZ], c_file[GMT_STR16], t_file[GMT_STR16], def_unit[16];
	static char unit[3] = "cip";

	struct GMT_GRID *G = NULL, *I = NULL;
	struct GMT_PALETTE *P = NULL;
	struct GMT_DATASET *T = NULL;
	struct GMTMERCMAP_CTRL *Ctrl = NULL;
	struct GMT_OPTION *options = NULL;

	/*----------------------- Standard module initialization and parsing ----------------------*/

	if (API == NULL) return (EXIT_FAILURE);
 	if (mode == GMT_MODULE_PURPOSE) return (usage (API, length_unit, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	/* Set or get option list */

	if (!options || options->option == GMT_OPT_USAGE) 
		bailout (usage (API, length_unit, GMT_USAGE));		/* Return the usage message */
	if (options && options->option == GMT_OPT_SYNOPSIS) 
		bailout (usage (API, length_unit, GMT_SYNOPSIS));	/* Return the synopsis */

	/* Parse the common command-line arguments */
	if (GMT_Parse_Common (API, THIS_MODULE_OPTIONS, options)) return (EXIT_FAILURE);	/* Parse the common options */
	GMT_Get_Default (API, "PROJ_LENGTH_UNIT", def_unit);
	if (!strcmp (def_unit, "cm")) length_unit = 0;
	else if (!strcmp (def_unit, "inch")) length_unit = 1;
	else if (!strcmp (def_unit, "point")) length_unit = 2;

	Ctrl = New_Ctrl (length_unit);	/* Allocate and initialize a new control structure */
	if ((error = parse (API, Ctrl, options))) Return (error);

	/*---------------------------- This is the gmtmercmap main code ----------------------------*/

	/* 1. If -R is not given, we must set a default map region, here -R-180/+180/-75/+75 */
	
	if (GMT_Get_Common (API, 'R', wesn) == GMT_NOTSET){	/* Get or set default world region */
		wesn[GMT_XLO] = -180.0;	wesn[GMT_XHI] = +180.0;
		wesn[GMT_YLO] =  -75.0;	wesn[GMT_YHI] =  +75.0;
	}
	B_active = (GMT_Get_Common (API, 'B', NULL) == 0);	/* 1 if -B was specified */
	K_active = (GMT_Get_Common (API, 'K', NULL) == 0);	/* 1 if -K was specified */
	O_active = (GMT_Get_Common (API, 'O', NULL) == 0);	/* 1 if -O was specified */
	P_active = (GMT_Get_Common (API, 'P', NULL) == 0);	/* 1 if -P was specified */
	X_active = (GMT_Get_Common (API, 'X', NULL) == 0);	/* 1 if -X was specified */
	Y_active = (GMT_Get_Common (API, 'Y', NULL) == 0);	/* 1 if -Y was specified */
	
	/* 2. Unless -E, determine approximate map area in degrees squared (just dlon * dlat), and use it to select which ETOPO?m.nc grid to use */
	
	if (Ctrl->E.active)	/* Specified the exact resolution to use */
		min = Ctrl->E.mode;
	else {	/* Determine resolution automatically from map area */
		area = (wesn[GMT_XHI] - wesn[GMT_XLO]) * (wesn[GMT_YHI] - wesn[GMT_YLO]);
		min = (area < ETOPO1M_LIMIT) ? 1 : ((area < ETOPO2M_LIMIT) ? 2 : 5);	/* Use earth_relief_[1,2,5]m.grd depending on area */
	}

	sprintf (file, "@earth_relief_%2.2dm", min);	/* Make the selected file name and make sure it is accessible */
	if ((G = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_GRID_HEADER_ONLY, NULL, file, NULL)) == NULL) {
		GMT_Report (API, GMT_MSG_NORMAL, "Unable to locate file %s in the GMT search directories\n", file);
		Return (EXIT_FAILURE);
	}
	
	if (Ctrl->D.active) {	/* Just write equivalent GMT shell script instead of making a map */
		int step = 0;
		char *comment[3] = {"#", "#", "REM"};	/* Comment for csh, bash and DOS [none], respectively */
		char *proc[3] = {"C-shell", "Bash", "DOS"};	/* Name of csh, bash and DOS */
		char prefix[64], region[256], width[256];
		struct GMT_OPTION *opt = NULL;
		time_t now = time (NULL);
		
		GMT_Report (API, GMT_MSG_VERBOSE, "Create % script that can be run to build the map\n", proc[Ctrl->D.mode]);
		if (Ctrl->D.mode == DOS_MODE)	/* Don't know how to get process ID in DOS */
			sprintf (prefix, "tmp");
		else
			sprintf (prefix, "/tmp/$$");
			
		switch (Ctrl->D.mode) {
			case BASH_MODE: printf ("#!/bin/bash\n"); break;
			case CSH_MODE:  printf ("#!/bin/csh\n"); break;
			case DOS_MODE:  printf ("REM DOS script\n"); break;
		}
		printf ("%s Produced by gmtmercmap on %s%s\n", comment[Ctrl->D.mode], ctime (&now), comment[Ctrl->D.mode]);
		
		switch (Ctrl->D.mode) {	/* Deal with noclobber first */
			case BASH_MODE: printf ("set +o noclobber\n"); break;
			case CSH_MODE:  printf ("unset noclobber\n"); break;
		}
		printf ("%s------------------------------------------\n", comment[Ctrl->D.mode]);
		printf ("%s %d. Set variables you may change later:\n", comment[Ctrl->D.mode], ++step);
		printf ("%s Name of plot file:\n", comment[Ctrl->D.mode]);
		set_var (Ctrl->D.mode, "map", "merc_map.ps");
		sprintf (region, "%g/%g/%g/%g", wesn[GMT_XLO], wesn[GMT_XHI], wesn[GMT_YLO], wesn[GMT_YHI]);
		sprintf (width, "%g", Ctrl->W.width);
		printf ("%s Data region:\n", comment[Ctrl->D.mode]);
		set_var (Ctrl->D.mode, "region", region);
		printf ("%s Map width:\n", comment[Ctrl->D.mode]);
		set_dim (Ctrl->D.mode, length_unit, "width", Ctrl->W.width);
		printf ("%s Intensity of illumination:\n", comment[Ctrl->D.mode]);
		set_var (Ctrl->D.mode, "intensity", "0.8");
		printf ("%s Azimuth of illumination:\n", comment[Ctrl->D.mode]);
		set_var (Ctrl->D.mode, "azimuth", "45");
		printf ("%s Color table:\n", comment[Ctrl->D.mode]);
		set_var (Ctrl->D.mode, "cpt", Ctrl->C.file);
		if (Ctrl->S.active) {	/* Plot color bar so set variables */
			printf ("%s Vertical shift from map to top of color bar:\n", comment[Ctrl->D.mode]);
			set_var (Ctrl->D.mode, "scale_shift", MAP_BAR_GAP);
			printf ("%s Width of color bar:\n", comment[Ctrl->D.mode]);
			set_dim (Ctrl->D.mode, length_unit, "scale_width", 0.9*Ctrl->W.width);
			printf ("%s Height of color bar:\n", comment[Ctrl->D.mode]);
			set_var (Ctrl->D.mode, "scale_height", MAP_BAR_HEIGHT);
			printf ("%s Map offset from paper edge:\n", comment[Ctrl->D.mode]);
			set_var (Ctrl->D.mode, "map_offset", MAP_OFFSET);
		}
		printf ("%s------------------------------------------\n\n", comment[Ctrl->D.mode]);
		printf ("%s %d. Extract grid subset:\n", comment[Ctrl->D.mode], ++step);
		printf ("gmt grdcut %s -G%s_topo.nc -R", file, prefix); place_var (Ctrl->D.mode, "region", 1);
		printf ("%s %d. Compute intensity grid for artificial illumination:\n", comment[Ctrl->D.mode], ++step);
		printf ("gmt grdgradient %s_topo.nc -fg -G%s_int.nc -Nt", prefix, prefix); place_var (Ctrl->D.mode, "intensity", 0);
		printf (" -A"); place_var (Ctrl->D.mode, "azimuth", 1);
		printf ("%s %d. Determine symmetric relief range and get suitable CPT file:\n", comment[Ctrl->D.mode], ++step);
		switch (Ctrl->D.mode) {
			case BASH_MODE: printf ("T_opt=`gmt grdinfo %s_topo.nc -T%g+s`\n", prefix, TOPO_INC); break;
			case CSH_MODE:  printf ("set T_opt = `gmt grdinfo %s_topo.nc -T%g+s`\n", prefix, TOPO_INC); break;
			case DOS_MODE: /* Must determine the grdinfo result directly */
				if (GMT_Open_VirtualFile (API, GMT_IS_DATASET, GMT_IS_TEXT, GMT_OUT, NULL, t_file) != GMT_NOERROR) exit (EXIT_FAILURE);
				sprintf (cmd, "%s -R%s -T%g+s > %s", file, region, TOPO_INC, t_file);			/* The grdinfo command line */
				if (GMT_Call_Module (API, "grdinfo", GMT_MODULE_CMD, cmd) != GMT_NOERROR) exit (EXIT_FAILURE);	/* This will return the -T<string> back via the T textset */
				if ((T = GMT_Read_VirtualFile (API, t_file)) == NULL) exit (EXIT_FAILURE);	/* Get pointer to that container with the input textset */
				printf ("set T_opt=%s\n", T->table[0]->segment[0]->text[0]);
				printf ("gmt makecpt -C"); place_var (Ctrl->D.mode, "cpt", 0);
				printf (" %%T_opt%% > %s_color.cpt\n", prefix);
				if (GMT_Close_VirtualFile (API,t_file) != GMT_NOERROR) exit (EXIT_FAILURE);
				break;
		}
		if (Ctrl->D.mode != DOS_MODE) {
			printf ("gmt makecpt -C"); place_var (Ctrl->D.mode, "cpt", 0);
			printf (" ${T_opt} > %s_color.cpt\n", prefix);
		}
		printf ("%s %d. Make the color map:\n", comment[Ctrl->D.mode], ++step);
		printf ("gmt grdimage %s_topo.nc -I%s_int.nc -C%s_color.cpt -JM", prefix, prefix, prefix); place_var (Ctrl->D.mode, "width", 0);
		if (!B_active)
			printf (" -Ba -BWSne");	/* Add default frame annotation */
		else {	/* Loop over arguments and find all -B options */
			for (opt = options; opt; opt = opt->next)
				if (opt->option == 'B') printf (" -B%s", opt->arg);
		}
		if (O_active) printf (" -O");	/* Add optional user options */
		if (P_active) printf (" -P");	/* Add optional user options */
		if (Ctrl->S.active || K_active) printf (" -K");	/* Either gave -K or implicit via -S */
		if (!X_active && !O_active) printf (" -Xc");	/* User gave neither -X nor -O so we center the map */
		if (Ctrl->S.active) {	/* May need to add some vertical offset to account for the color scale */
			if (!Y_active && !K_active) printf (" -Y"), place_var (Ctrl->D.mode, "map_offset", 0);	/* User gave neither -K nor -Y so we add 0.75i offset to fit the scale */
		}
		printf (" > "); place_var (Ctrl->D.mode, "map", 1);
		if (Ctrl->S.active) {	/* Plot color bar centered beneath map */
			printf ("%s %d. Overlay color scale:\n", comment[Ctrl->D.mode], ++step);
			printf ("gmt psscale -C%s_color.cpt -R -J -Bxa -By+lm -O -DJCB+w", prefix); place_var (Ctrl->D.mode, "scale_width", 0);	putchar ('/');	place_var (Ctrl->D.mode, "scale_height", 0);
			printf ("+h+o0/"); place_var (Ctrl->D.mode, "scale_shift", 0);	/* The psscale command line */
			if (K_active) printf (" -K");		/* Add optional user options */
			printf (" >> "); place_var (Ctrl->D.mode, "map", 1);
		}
		printf ("%s %d. Remove temporary files:\n", comment[Ctrl->D.mode], ++step);
		if (Ctrl->D.mode == DOS_MODE) printf ("del %s_*.*\n", prefix); else printf ("rm -f %s_*\n", prefix);
		Return (EXIT_SUCCESS);
	}
	
	/* Here we actuallly make the map */
	
	GMT_Report (API, GMT_MSG_VERBOSE, "Create Mercator map of area %g/%g/%g/%g with width %g%c\n",
		wesn[GMT_XLO], wesn[GMT_XHI], wesn[GMT_YLO], wesn[GMT_YHI], 
		Ctrl->W.width, unit[length_unit]);
		
	/* 3. Load in the subset from the selected etopo?m.nc grid */
	
	GMT_Report (API, GMT_MSG_VERBOSE, "Read subset from %s\n", file);
	if ((G = GMT_Read_Data (API, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_GRID_DATA_ONLY, wesn, file, G)) == NULL) Return (EXIT_FAILURE);

	/* 4. Compute the illumination grid via GMT_grdgradient */
	
	GMT_Report (API, GMT_MSG_VERBOSE, "Compute artificial illumination grid from %s\n", file);
	/* Register the topography as read-only input and register the output intensity surface to a memory location */
	if (GMT_Open_VirtualFile (API, GMT_IS_GRID, GMT_IS_SURFACE, GMT_IN,     G, z_file) != GMT_NOERROR) exit (EXIT_FAILURE);
	if (GMT_Open_VirtualFile (API, GMT_IS_GRID, GMT_IS_SURFACE, GMT_OUT, NULL, i_file) != GMT_NOERROR) exit (EXIT_FAILURE);
	memset (cmd, 0, BUFSIZ);
	sprintf (cmd, "%s -G%s -Nt0.8 -A45 -fg", z_file, i_file);			/* The grdgradient command line */
	if (GMT_Call_Module (API, "grdgradient", GMT_MODULE_CMD, cmd) != GMT_NOERROR) Return (EXIT_FAILURE);	/* This will write the intensity grid to an internal allocated container */
	if ((I = GMT_Read_VirtualFile (API, i_file)) == NULL) Return (EXIT_FAILURE);	/* Get the intensity grid */
	if (GMT_Close_VirtualFile (API, i_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Done with this virtual file */
	
	/* 5. Determine a reasonable color range based on TOPO_INC m intervals and retrieve a CPT */
	
	GMT_Report (API, GMT_MSG_VERBOSE, "Determine suitable color range and build CPT file\n");
	/* Round off to nearest TOPO_INC m and make a symmetric scale about zero */
	z_min = floor (G->header->z_min/TOPO_INC)*TOPO_INC;
	z_max = floor (G->header->z_max/TOPO_INC)*TOPO_INC;
	z = fabs (z_min);
	if (fabs (z_max) > z) z = fabs (z_max);	/* Make it symmetrical about zero */
	/* Register the output CPT file to a memory location */
	if (GMT_Open_VirtualFile (API, GMT_IS_PALETTE, GMT_IS_NONE, GMT_OUT, NULL, c_file) != GMT_NOERROR) exit (EXIT_FAILURE);
	memset (cmd, 0, BUFSIZ);
	sprintf (cmd, "-C%s -T%g/%g ->%s", Ctrl->C.file, -z, z, c_file);	/* The makecpt command line */
	if (GMT_Call_Module (API, "makecpt", GMT_MODULE_CMD, cmd) != GMT_NOERROR) Return (EXIT_FAILURE);	/* This will write the output CPT to memory */
	if ((P = GMT_Read_VirtualFile (API, c_file)) == NULL) Return (EXIT_FAILURE);	/* Get the CPT */
	if (GMT_Close_VirtualFile (API, c_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Done with this virtual file */
	
	/* 6. Now make the map */
	
	GMT_Report (API, GMT_MSG_VERBOSE, "Generate the Mercator map\n");
	/* Register the three input sources (2 grids and 1 CPT); output is PS that goes to stdout */
	if (GMT_Init_VirtualFile (API, 0, z_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Reset the grid for reading again */
	if (GMT_Open_VirtualFile (API, GMT_IS_GRID, GMT_IS_SURFACE, GMT_IN, I, i_file) != GMT_NOERROR) exit (EXIT_FAILURE);
	if (GMT_Open_VirtualFile (API, GMT_IS_PALETTE, GMT_IS_NONE, GMT_IN, P, c_file) != GMT_NOERROR) exit (EXIT_FAILURE);
	memset (cmd, 0, BUFSIZ);
	sprintf (cmd, "%s -I%s -C%s -JM%g%c -Ba -BWSne", z_file, i_file, c_file, Ctrl->W.width, unit[length_unit]);	/* The grdimage command line */
	if (O_active) strcat (cmd, " -O");	/* Add optional user options */
	if (P_active) strcat (cmd, " -P");	/* Add optional user options */
	if (Ctrl->S.active || K_active) strcat (cmd, " -K");	/* Either gave -K or it is implicit via -S */
	if (!X_active && !O_active) strcat (cmd, " -Xc");	/* User gave neither -X nor -O so we center the map */
	if (Ctrl->S.active) {	/* May need to add some vertical offset to account for the color scale */
		if (!Y_active && !K_active) strcat (cmd, " -Y" MAP_OFFSET);	/* User gave neither -K nor -Y so we add 0.75i offset to fit the scale */
	}
	if (GMT_Call_Module (API, "grdimage", GMT_MODULE_CMD, cmd) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Lay down the Mercator image */
	
	/* 7. Plot the optional color scale */
	
	if (Ctrl->S.active) {
		GMT_Report (API, GMT_MSG_VERBOSE, "Append color scale bar\n");
		/* Register the CPT to be used by psscale */
		if (GMT_Init_VirtualFile (API, 0, c_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Reset the CPT for reading again */
		memset (cmd, 0, BUFSIZ);
		sprintf (cmd, "-C%s -R -J -DJCB+w%g%c/%s+h+o0/%s -Bxa -By+lm -O", c_file, 0.9*Ctrl->W.width, unit[length_unit], MAP_BAR_HEIGHT, MAP_BAR_GAP);	/* The psscale command line */
		if (K_active) strcat (cmd, " -K");		/* Add optional user options */
		if (GMT_Call_Module (API, "psscale", GMT_MODULE_CMD, cmd) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Place the color bar */
	}
	if (GMT_Close_VirtualFile (API, z_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Done with this virtual file */
	if (GMT_Close_VirtualFile (API, i_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Done with this virtual file */
	if (GMT_Close_VirtualFile (API, c_file) != GMT_NOERROR) Return (EXIT_FAILURE);	/* Done with this virtual file */
	
	/* 8. Let the GMT API garbage collection free the memory used */
	GMT_Report (API, GMT_MSG_VERBOSE, "Mapping completed\n");
	
	Return (EXIT_SUCCESS);
}
