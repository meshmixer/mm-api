Autodesk Meshmixer API package (mm-api)
v0.2 November 2014
copyright (c) 2014 Autodesk Corporation    http://www.autodesk.com
contact:   Ryan.Schmidt@Autodesk.com       http://autodeskresearch.com/people/ryanschmidt

[WARNING] This API distribution is compatible with meshmixer 2.7
          It may not work with previous or later versions.

Do you have the latest code? Sync here: https://github.com/meshmixer/mm-api

===================================================
CAUTION! PRE-ALPHA RELEASE! CHANGES ARE COMING! 
===================================================
The Meshmixer API is under active development, and it is highly
likely that non-backwards-compatible changes will occur on a
regular basis. In most cases this will simply result in
additions to the API, however the following are also likely:
   - future versions of the StoredCommands C++ class will not be binary-compatible
   - existing C++ API functions may be renamed or removed

Why release this API before it stabilizes? Because we need
your feedback! Please let us know which parts of Meshmixer
you would like to see exposed in the API.

===============
What is mm-api?
===============
The Autodesk Meshmixer desktop application is remotely-controllable via a 
C++ API. You can use this API to perform operations inside the running
desktop application. 

The mm-api package also includes SWIG definitions which allow bindings
to be generated for other languages, such as Python, C#, etc.
Currently we include scripts/etc for building Python bindings,
but other languages may be included in the future.

Since the SWIG wrappers are somewhat verbose / unpleasant to use,
for Python we are also developing a cleaner interface on top-level
of the SWIG wrapper. See details below.

[WARNING] Currently the python setup included in the distribution is for Windows only.
          To use OSX you will have to download SWIG yourself.


====================
Quick Start - Python
====================
0) Install Python 2.7
1) open your meshmixer.ini file (paths below) and add the following 
   line under the [Options] section: 
      EnableRemoteStoredCommandServer=true
2) start meshmixer, import bunny
3) run the script \distrib\python\test.py 
   If your bunny is cut in half, the script worked

   
   
========
Setup
========

To enable remote control of meshmixer, you must insert the following line
in your meshmixer.ini file, under the [Options] section:
   
   EnableRemoteStoredCommandServer=true
   
Now on startup meshmixer will listen for remote commands (see details below).

You can find your meshmixer.ini file here:
     (Windows) C:\Users\<username>\AppData\Roaming\Autodesk\meshmixer.ini
	 (OSX)     /Users/<username>/.config/Autodesk/meshmixer.ini

To build API packages from scratch, run the top-level build_python_distrib.bat file.
This will require Visual Studio 2012 to be installed on your machine.
The output API wrapper will be copied to \distrib\python

If you received a .zip file, then \distrib\python may already exist. 
In this case you do not need Visual Studio, simply a Python 2.7 interpreter.


=======
Python
=======

We are currently focusing on creating a cleaner wrapper for the API in Python.
This "mm" python module is stored in \python\mm\

Various usage examples for the raw API and mm module are in \python\examples\
Note that the mm module will need to be in your python path for these
examples scripts to run.

If you have Visual Studio, you can install Python Tools for Visual Studio 
(http://pytools.codeplex.com/) Then you can open \python\mm.sln, and 
immediately run the various examples. The examples vcproject is already
configured to include the top-level \distrib\python\ in the path

This mm module is still under active development, and many functions in the 
SWIG-generated API are not yet available in the mm module. However, we strongly
recommend that you use the mm module whenever possible. Not only does it
result in cleaner code, but it will insulate you from the inevitable
future changes in the SWIG-generated API.

(Have you made improvements to the mm module? Make a pull request!)


===============
Documentation ?  
===============
Currently we do not have any stand-alone documentation for the API capabilities.
This will improve in the future. (Would you like to help? Contribute!)

API functions take the form of simple commands or query/responses, so usage
is straightforward. See the StoredCommands.h header for documentation of the
available functions, and valid strings/arguments.

Many examples of Python API usage are available inside the functions of
the mm Python module. Usage in other languages is very similar.
	  
	  
===============
Technical Notes
===============

The API is implemented as a command stream which can be serialized by
the client, sent to Meshmixer, and then de-serialized and executed.
StoredCommands.h/cpp constructs and serializes the command stream,
using BinarySerializer.h (These files can be found in \build\).

After each StoredCommands is received and executed, Meshmixer sends
a second StoredCommands back to the client, which contains any
"result" information (ie results of queries, operation success/fail info, etc).

Some things in StoredCommands are a bit convoluted because of SWIG,
which cannot handle certain C++ idioms/types. 

The API is in fact not really a C++ API, as the serialized command objects
are just binary streams sent over a network socket. This has the
advantage that you do not have to link to anything, or dynamically
load any DLLs into meshmixer. API clients run in entirely separate processes.
Another potentially-useful capability is that if you can produce the same bits, 
you don't actually need to use StoredCommands (although this seems like this would be complicated...).

In terms of the network communication, currently this happens via UDP datagrams.
Note that this limits the command packets to a maximum size.
Meshmixer listens on localhost (127.0.0.1) port 0xAFCF=45007
and sends responses to localhost port 0xAFDF=45023
 
This is perhaps not the best architecture, but it is easy to use in
many languages, and is sufficient for the current API capabilities.
This approach does prevent sending large amounts of data (for example full meshes), 
and so it is likely to be re-designed in the future. 


===========
Changelog
===========
(only significant changes that will impact usage/compatibility)

[November 27, 2014]
- initial release, compatible with mm 2.7
