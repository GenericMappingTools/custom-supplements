Copyright (c) 1991-2024 by the GMT Team. See LICENSE.TXT file for copying and redistribution conditions.

Contact info: www.generic-mapping-tools.org

CMake User Guide
================

Build and runtime prerequisites
-------------------------------

You need CMake (>=2.8.5) and an installation of GMT (>=5.2.0).

Information for building GMT 5 with CMake
-----------------------------------------

Quick start:
~~~~~~~~~~~~

Checkout the GMT extension template from its subversion repository;
here we call the directory "custom":

  $ git@github.com:GenericMappingTools/custom-supplements.git custom
  $ cd custom
  $ cp cmake/ConfigUserTemplate.cmake cmake/ConfigUser.cmake

Edit *cmake/ConfigUser.cmake* [see comments in the file]. Then:

  $ mkdir build
  $ cd build
  $ cmake -DCMAKE_INSTALL_PREFIX=custom -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
  $ make -jx_

where _x_ is the number of threads you want to use and depends on the number
of cores in your CPU and if hyperthreading is available or not.
cmake will build out-of-source in the the directory _build_. 'CMAKE_BUILD_TYPE'
can be one of: empty, Debug, Release, RelWithDebInfo or MinSizeRel

  $ make -jx_ install

installs a basic custom in _build/custom_.

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

Finding GMT:
~~~~~~~~~~~~

GMT is searched in FindGMT.cmake. If CMake cannot find GMT then you
have to configure _GMT_ROOT_ in cmake/ConfigUser.cmake.

Installing:
~~~~~~~~~~~

  $ make -jx_ install

will compile and install the shared library.

You have the choice between a monolithic installation
(_-DGMT_INSTALL_MONOLITHIC=ON_) where everything goes into a common
subdirectory or a distribution-like installation (PREFIX/bin/custom-VERSION,
PREFIX/lib/custom-VERSION/).

CUSTOM is shipped as a single shared library.  By default we also set "classic mode"
by installing convenience links for all CUSTOM modules. New GMT users should set
INSTALL_MODULE_LINKS to FALSE in cmake/ConfigUser.cmake to avoid these links.

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

Set CUSTOM_RELEASE_PREFIX in cmake/ConfigUser.cmake and run cmake. Then do

  $ make custom_release     # export the source tree and install doc

You should then edit ${CUSTOM_RELEASE_PREFIX}/cmake/ConfigDefault.cmake and
set CUSTOM_PACKAGE_VERSION_MAJOR, CUSTOM_PACKAGE_VERSION_MINOR, and
CUSTOM_PACKAGE_VERSION_PATCH. Also uncomment and set
CUSTOM_SOURCE_CODE_CONTROL_VERSION_STRING to the current svn version.

////
vim: ft=asciidoc tw=78 et ts=2 sts=2 sw=2 autoindent smartindent
////
