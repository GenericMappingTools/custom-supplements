#	$Id$

This README file explains the various steps you must take to design
and successfully build a custom plugin library that can be accessed
via the main gmt executable, just like the standard and official
supplemental GMT modules.  It is required that you have installed
GMT 5.2 and made it your active GMT version before proceeding.

Paul Wessel, November 18, 2015

1.  Decide what your plugin library name should be.  This is a short
    single word that determines the plugin name, e.g., "custom".
    Below, we will refer to this name as the <TAG>.
2.  Edit top-level CMakeLists.txt and change project (example) to project (<TAG>).
3.  Code up your module(s).  There are several examples of fully
    working modules in src.  Model your code after those templates.
    Set the THIS_MODULE_LIB parameter in your modules to <TAG> and supply
    reasonable settings to THIS_MODULE_KEYS (see API documentation for the
    GMT_Encode_Options function for more details).
4.  Add all your modules filenames to CUSTOM_PROGS_SRCS in src/CMakeLists.txt
    and set the string for LIB_STRING to match your plugin's key purpose.
5.  Create required glue source code automatically by running the script
   	gmt_make_custom_code.sh <TAG>
    in the src directory.
6.  In cmake, make ConfigUser.cmake by copying the template and make any edits.
7.  Create a build directory in the top level directory, cd into build,
    and run "cmake .." (see README.CMake for more details).
8.  Make your plugin library with "make -j<n>" and then run "make -j<n> install",
    where <n> is how many cores you want to use in the building [1].
9.  Use gmt.conf's GMT_CUSTOM_LIBS parameter to specify where to find your plugin
    library (i.e., /full/path/to/<TAG>.so).
10. Try it out to see if your modules are listed:

	gmt --help
	gmt --show-modules
