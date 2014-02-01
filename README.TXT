#	$Id$

This README file explains the various steps you must take to design
and successfully build a custom plugin library that can be accessed
via the main gmt executable, just like the standard and supplemental
GMT modules.  It is required that you have installed GMT 5 before
proceeding.

Paul Wessel, November 5, 2013

1. Decide what your plugin library name should be.  This is a short
   name determines the plugin name, e.g., "custom".
   Below, we will refer to this name as TAG.
2. Edit top-level CMakeLists.txt and change project (example) to project (TAG).
3. You must code up your module.  There are some examples of fully
   working modules in src.  Base your code on those templates.
   Set the THIS_MODULE_LIB parameter in your code to TAG.
4. Add you module's filename.c to CUSTOM_PROGS_SRCS in src/CMakeLists.txt
   and set the string for LIB_STRING to match your suite's purpose
5. Create required source code automatically by running the script
   	gmt_make_module_src.sh
6. In cmake, make a ConfigUser.cmake from the template, make any edits.
7. Create a build directory in the top level directory, cd into build,
   and run cmake (see README.CMake for how).
8. Make your plugin library with make -j<N> and make -j<N> install,
   where <N> is how many cores you want to use in the building.
9. Use gmt.conf's GMT_CUSTOM_LIBS parameter to specify your plugin
   library (i.e., custom.so).
10. Try it out to see if your modules are listed:

	gmt --help
