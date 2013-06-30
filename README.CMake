////
$Id: README.CMake 11801 2013-06-24 21:19:31Z pwessel $

Copyright (c) 1991-2013 by P. Wessel, W. H. F. Smith, R. Scharroo, J. Luis
and F. Wobbe. See LICENSE.TXT file for copying and redistribution conditions.

Contact info: gmt.soest.hawaii.edu
////

CMake User Guide
================

Build and runtime prerequisites
-------------------------------

1. Software:
  You need Ghostscript, CMake (>=2.8.5), netCDF (>=4.0, netCDF-4/HDF5
  support mandatory).  Optionally install PCRE, GDAL, and FFTW.
2. Data:
  You need gshhg (>=2.2.2); optionally install dcw-gmt (>=1.0.5)

Information for building GMT 5 with CMake
-----------------------------------------

Quick start:
~~~~~~~~~~~~

Checkout GMT5 from its subversion repository:

  $ svn co svn://gmtserver.soest.hawaii.edu/gmt5/trunk gmt5-dev
  $ cd gmt5-dev
  $ cp cmake/ConfigUserTemplate.cmake cmake/ConfigUser.cmake

Edit *cmake/ConfigUser.cmake* [see comments in the file]. Then:

  $ mkdir build
  $ cd build
  $ cmake -DCMAKE_INSTALL_PREFIX=gmt5 -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
  $ make -jx_

where _x_ is the number of threads you want to use and depends on the number
of cores in your CPU and if hyperthreading is available or not.
cmake ill build out-of-source in the the directory _build_. 'CMAKE_BUILD_TYPE'
can be one of: empty, Debug, Release, RelWithDebInfo or MinSizeRel

  $ make -jx_ install

installs a basic gmt in _build/gmt5_.

NOTE: All cmake command line options such as _-DCMAKE_INSTALL_PREFIX_ can be
configured in *cmake/ConfigUser.cmake*.

In more detail:
~~~~~~~~~~~~~~~

For CMake related questions refer to the CMake manual which is available
online: http://www.cmake.org/cmake/help/cmake-2-8-docs.html

To avoid CMake's lengthy command line options you can create your custom
ConfigUser.cmake file in the cmake directory. Simply duplicate the
cmake/ConfigUserTemplate.cmake to cmake/ConfigUser.cmake and then make
changes in cmake/ConfigUser.cmake. See additional comments in that file.

There are two configuration files:

1. "ConfigDefault.cmake" -- is version controlled and used to add new default
    variables and set defaults for everyone. You should not edit this file.
2. "ConfigUser.cmake" -- is not version controlled (currently listed in svn:ignore
    property) and used to override defaults on a per-user basis. There is a
    template file, ConfigUserTemplate.cmake, that you should copy to
    ConfigUser.cmake and make your changes therein.

NOTE: If you want to change CMake behaviour for your build then only modify the
      "ConfigUser.cmake" file (not "ConfigDefault.cmake").

Invoking CMake:
~~~~~~~~~~~~~~~

CMake supports out-of-source builds and puts generated files in a completely
separate directory, so that the source tree is unchanged. Trying to run CMake
in the source tree will result in an error. CMake will first check for the
minimal required version and then search for required and optional libraries
(netCDF, PCRE, GDAL) in the default locations. The configuration step will
fail if dependencies are not met. You have to ensure that CMake can find the
required resources by configuring *cmake/ConfigUser.cmake*.

Finding GSHHG:
~~~~~~~~~~~~~~

GSHHG shorelines are searched in FindGSHHG.cmake and a little helper program
tests the version (gshhg_version.c). If CMake cannot find the shorelines you
have to configure _GSHHG_ROOT_ in cmake/ConfigUser.cmake.

Finding DCW:
~~~~~~~~~~~~~~

DCW (Digital Chart of the World) country polygons are searched at compile time.
The DCW data are optional; they are currently used in pscoast -F for painting
individual countries only.  If CMake cannot auto-find DCW for you then you can
configure _DCW_ROOT_ in cmake/ConfigUser.cmake.

Documentation:
~~~~~~~~~~~~~~

Manpages are available in different formats and can be generated with:

  $ make -jx_ manpages_all

To generate the PDF documentation you need LaTeX:

  $ make -jx_ gmt_doc

You can choose to install documentation files from an external location instead
of generating the PDF and HTML files from the sources. This is convenient if
LaTeX is not available. Set GMT_INSTALL_EXTERNAL_DOC in cmake/ConfigUser.cmake.

Installing:
~~~~~~~~~~~

  $ make -jx_ install

will compile and install libps, libgmt, and all GMT modules. Optionally it
will install the GSHHG shorelines (if found), DCW (if found), UNIX manpages,
and HTML and PDF documentation.
You have to explicitly make the latter two beforehand or they will be omitted
during installation. You have the choice between a monolithic installation
(_-DGMT_INSTALL_MONOLITHIC=ON_) where everything goes into a common
subdirectory or a distribution-like installation (PREFIX/bin/gmt-VERSION,
PREFIX/lib/gmt-VERSION/, PREFIX/share/gmt-VERSION/,
PREFIX/share/doc/gmt-VERSION/).

GMT is shipped as a single executable.  By default we also set "classic mode"
by installing convenience links for all GMT modules. New GMT users should set
INSTALL_MODULE_LINKS to FALSE in cmake/ConfigUser.cmake.

Updating:
~~~~~~~~~

Assuming you did not delete the build directory and that your current
working directory is the build directory this is just as simple as

  $ cd ..
  $ svn up
  $ cd -
  $ make -jx_ install

CMake will detect any changes to the source files and will automatically
reconfigure. If you deleted all files inside the build directory you have to
run cmake again manually.

Packaging:
~~~~~~~~~~

Currently, packaging with CPack works on MacOSX (Bundle, TGZ, TBZ2),
Windows (ZIP, NSIS), and UNIX (TGZ, TBZ2). On Windows you need to install NSIS
(http://nsis.sourceforge.net/). After building GMT run either one of these:

  $ make package
  $ cpack -G <TGZ|TBZ2|Bundle|ZIP|NSIS>

You probably also want to run cmake with _-DGMT_INSTALL_MONOLITHIC=ON_ before
creating packages.

Creating a source package:
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set GMT_RELEASE_PREFIX in cmake/ConfigUser.cmake and run cmake. Then do

  $ make gmt_doc         # make PDF and HTML documentation
  $ make gmt_doc_shrink  # reduce the size of the PDFs
  $ make gmt_release     # export the source tree and install doc

You should then edit ${GMT_RELEASE_PREFIX}/cmake/ConfigDefault.cmake and
set GMT_PACKAGE_VERSION_MAJOR, GMT_PACKAGE_VERSION_MINOR, and
GMT_PACKAGE_VERSION_PATCH. Also uncomment and set
GMT_SOURCE_CODE_CONTROL_VERSION_STRING to the current svn version.

In-build-dir tests:
~~~~~~~~~~~~~~~~~~~

Examples and tests can be build inside GMT_BINARY_DIR *without* installing.
This is very convenient for testing. Just _enable_testing()_, set _DO_EXAMPLES_
and/or _DO_TESTS_ in ConfigUser.cmake and do:

  $ make -jx_
  $ make check

Optionally set _N_TEST_JOBS_ to the number of ctest jobs to run
simultaneously. You can also select individual tests using regexp with ctest:

  $ ctest -R job1[3-6]


TODO list
---------

* Replace GMT_(malloc|memory): perl -i -pe 's/(GMT_(malloc|memory)[^_].+, )([^,)]+)/$1sizeof($3)/g' files ...

* Clean up config.h.in gmt_version.h.in gmt_notposix.h.in.
  Right now this is a mixture of partly redundant defines that can be
  simplified. (almost done)

* Make exported cmake variables consistent GMT_..., prefix temporary
  variables with "_"

* Add further tests/cmake-find-modules:
  - fftw/Accelerate Framework
  - WIN32: test for USE_MEM_ALIGNED, defined in src/gmt_config.h.in

* Generate macros
  - to build a release (including precompiled manpages, pdf docu, ...)

////
vim: ft=asciidoc tw=78 et ts=2 sts=2 sw=2 autoindent smartindent
////