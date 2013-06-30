**********
gmtmercmap
**********

gmtmercmap - Simple generation of ETOPO1|2|5 Mercator maps

Synopsis
--------

.. include:: common_SYN_OPTs.rst_

**gmtmercmap**
|SYN_OPT-B|
[ **-C**\ *cptfile* ] [ **-D**\ [**b**\ |\ **c**\ |\ **d**] ] 
[ **E**\ [**1**\ |\ **2\ **|\ **5**] ]
|SYN_OPT-K|
|SYN_OPT-O|
|SYN_OPT-P|
|SYN_OPT-R|
|SYN_OPT-U|
|SYN_OPT-U|
|SYN_OPT-X|
|SYN_OPT-Y|
[ **-S** ] [ **-W**\ *width* ]
|SYN_OPT-c|
|SYN_OPT-n|
|SYN_OPT-p|
|SYN_OPT-t|

|No-spaces|

Description
-----------

**gmtmercmap** simplifies the task of making a Mercator map based on the ETOPO1, ETOPO2, or ETOPO5
data sets.  Given your region it automatically determines which data set to use, and automatically
computes shading.  The key user control is the width of the map.  There are no required options;
most are there to enable the creation of further overlays.

Required Arguments
------------------

    None

Optional Arguments
------------------

.. include:: explain_-B.rst_

**-C**\ *cptfile*
    Name of the color palette table to use.  If none is given we default to IT(relief).

**-D**\ [**b**\ |\ **c**\ |\ **d**]
    Dry-run.  Do not make a map but instead produce the equivalent GMT commands for a script.
    Append type of script, choosing among BD(b) (Bourne shell), BD(c) (C-shell), or BD(d) DOS batch commands.

**E**\ [**1**\ |\ **2\ **|\ **5**]
    Force the selection of a particular ETOPO resolution.  Append 1, 2, or 5 for that resolution in arc minutes
    [Default automatically determines a suitable resolution].

.. |Add_-K| unicode:: 0x20 .. just an invisible code
.. include:: explain_-K.rst_

.. |Add_-O| unicode:: 0x20 .. just an invisible code
.. include:: explain_-O.rst_

.. |Add_-P| unicode:: 0x20 .. just an invisible code
.. include:: explain_-P.rst_

.. |Add_-R| unicode:: 0x20 .. just an invisible code
.. include:: explain_-R.rst_

**-S**
    Place a matching color scale bar centered beneath the map [none].

.. |Add_-U| unicode:: 0x20 .. just an invisible code
.. include:: explain_-U.rst_

.. |Add_-V| unicode:: 0x20 .. just an invisible code
.. include:: explain_-V.rst_

**-W**\ *width*
    Specify the full width of your map [19i or 25c, depending on your PROJ_LENGTH_UNIT setting].

.. |Add_-X| unicode:: 0x20 .. just an invisible code
.. include:: explain_-X.rst_

.. |Add_-Y| unicode:: 0x20 .. just an invisible code
.. include:: explain_-Y.rst_

.. |Add_-c| unicode:: 0x20 .. just an invisible code
.. include:: explain_-c.rst_

.. |Add_-n| unicode:: 0x20 .. just an invisible code
.. include:: explain_-n.rst_

.. |Add_-p| unicode:: 0x20 .. just an invisible code
.. include:: explain_-p.rst_

.. |Add_-t| unicode:: 0x20 .. just an invisible code
.. include:: explain_-t.rst_

.. include:: explain_help.rst_

GRID RESOLUTION SELECTION
-------------------------

Unless OPT(E) is set we determine the resolution based on the approximate area of the chosen region in degrees squared.
For areas less than 100 (e.g., 10 by 10) we use 1 minute resolution, while for areas larger than 10000 (100 by 100)
we use the 5-minute resolution; otherwise we default to ETOPO2m.  Note: The program is hard-wired to look for data files
with names of the form etopo1|2|5m_grd.nc in a data directory accessible to GMT (i.e., via GMT_DATADIR or GMT_USERDIR).

Examples
--------

Use the default settings to generate a page-size, near-global map (i.e., from 75S to 75N):

::

    gmtmercmap > map.ps

To generate a map for the mid-Atlantic area, adding a color bar, and selecting a width of 12 cm, use

::

    gmtmercmap -R-30/10/0/30 -P -W12c -S > map.ps

To just generate a Bourne shell script with the commands needed for a map of Australia, try

::

    gmtmercmap -R-100/160/45S/10S -P -W6i -Db > script.sh

The same map can be scripted for a DOS batch job via

::

    gmtmercmap -R-100/160/45S/10S -P -W6i -Dd > script.bat
See Also
--------

`gmt <gmt.html>`_ , `grdimage <grdimage.html>`_ ,
`grdcontour <grdcontour.html>`_ ,
`grdview <grdview.html>`_ ,
`grdgradient <grdgradient.html>`_,
`grdhisteq <grdhisteq.html>`_
