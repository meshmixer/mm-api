.. mm-python documentation master file, created by
   sphinx-quickstart on Sun Dec  7 12:03:30 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

mm python module documentation
=====================================

The mm python module is a wrapper for the SWIG-generated mmapi python module.
The purpose of this module is to provide shorter, more convenient
function names, that respect python conventions.

The mm module is organized into various top-level modules:

core meshmixer manipulation modules:

* :mod:`mm.scene`: Scene manipulation

* :mod:`mm.selection`: create/manipulate Selections

* :mod:`mm.tool`: run Tools, set parameters

meshmixer query modules:

* :mod:`mm.convert`: conversion utility functions

* :mod:`mm.util`: spatial queries

utility modules:

* :mod:`mm.frame`: mmFrame class

* :mod:`mm.mm_math`: linear algebra, math utilities


Important Notes
================

This documentation was generated on |today|, the current version of meshmixer at this time is 2.7. You can get the most recent version of the mm-api package, including the mm module and this documentation, at `<https://github.com/meshmixer/mm-api>`_

Please also see the github mainpage readme (ie follow the link above) for more details on the mm-api package, including setup instructions. This package is under active development and will change frequently. In particular, you must have a version compatible with the meshmixer installation you are running.


Usage Tips
===========

These modules all depend on the ``mmapi`` module, which is part of the mm-api package, found in the ``/distrib/python`` folder.

You can ``import mm`` and then access most top-level functions as ``mm.<function_name>``, rather than importing each sub-module. However, specifically for the math manipulation functions, you may find you can write cleaner code by also adding ``import mm.mm_math``.

The Meshmixer desktop application uses a normalized coordinate system internally. These coordinates are referred to as **Scene** coordinates, versus **World** coordinates, which are in the current physical dimensions of the scene. Currently many API functions expect and return points/dimensions in this space. See the :mod:`mm.convert` module for details on converting between Scene and World coordinates.



Meshmixer Manipulation Modules
==============================

mm.scene module
---------------

.. automodule:: mm.scene
    :members:
    :undoc-members:
    :show-inheritance:

mm.selection module
-------------------

.. automodule:: mm.selection
    :members:
    :undoc-members:
    :show-inheritance:

mm.tool module
--------------

.. automodule:: mm.tool
    :members:
    :undoc-members:
    :show-inheritance:


Meshmixer Query Modules
=============================

mm.convert module
-----------------

This module contains functions for converting to/from Meshmixer API values, including:

#. Conversion between SWIG-generated API helper structs/classes and python lists/tuples

#. Conversion between Meshmixer’s internal World and Scene coordinates

World/Scene coordinate conversion is one complication of Meshmixer that is not exposed in the desktop application, but must (currently) be managed by API clients.

Meshmixer internally re-scales imported meshes to a normalized coordinate space, ie a 2x2x2 unit box centered at the origin. API functions for spatial queries, bounding boxes, etc, currently expect points and dimensions in this normalized space, and return normalized results as well. 

You can use the functions in this module to convert between Scene and World coordinates, where World coordinates are in the current units selected in the Units/Dimension Tool. By default, new scenes (i.e. after an import) have units in millimetres.

.. automodule:: mm.convert
    :members:
    :undoc-members:
    :show-inheritance:





mm.util module
--------------

This module contains spatial query functions, i.e. functions which return bounding-boxes, find nearest-point-on-surface, ray-cast, etc. 

**This module is likely to be renamed in the near future**

.. automodule:: mm.util
    :members:
    :undoc-members:
    :show-inheritance:




Utility Modules
=============================

These modules do not actually make meshmixer API calls, they are included to make it easier to use the python API, and/or to write sample code. 


mm.frame module
---------------

.. automodule:: mm.frame
    :members:
    :undoc-members:
    :show-inheritance:

mm.mm_math module
-----------------

.. automodule:: mm.mm_math
    :members:
    :undoc-members:
    :show-inheritance:


Module contents
---------------

.. automodule:: mm
    :members:
    :undoc-members:
    :show-inheritance:



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

