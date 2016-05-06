# Autodesk Meshmixer API  (mm-api)  
v0.0 2016  
copyright (c) 2014-2016 Autodesk Corporation    http://www.autodesk.com  
contact:   Ryan.Schmidt@Autodesk.com       http://autodeskresearch.com/people/ryanschmidt  
See LICENSE file for license information  

**WARNING** This API distribution is compatible with meshmixer 3.0 and 32-bit Python 2.7.
It may not work with previous or later versions of meshmixer or Python.

Do you have the latest code? Sync here: https://github.com/meshmixer/mm-api

===================================================
The Meshmixer API is under active development, and it is
likely that non-backwards-compatible changes will occur. 
In most cases this will simply result in additions to the API, 
however the following are also likely:
   - future versions of the StoredCommands C++ class will not be binary-compatible
   - existing C++ API functions may be renamed or removed

Why release this API before it stabilizes? Because we need
your feedback! Please let us know which parts of Meshmixer
you would like to see exposed in the API.


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



Quick Start - Python
====================
[0] Install **32-bit Python 2.7**
[1] start meshmixer, import bunny  
[2] run the script \distrib\python\test.py (or \distrib\python_osx\test.py on Macs)
   If your bunny is cut in half, the script worked

   
   

Setup
========

To build API packages from scratch, run the top-level build_python_distrib.bat file.
This will require Visual Studio 2012 to be installed on your machine.
The output API wrapper will be copied to \distrib\python.
On OSX, run the top-level script build_python_distrib.sh, which will
produce an output API wrapper in \distrib\python_osx

If you received a .zip file, then \distrib\python and \python_osx may already exist. 
In this case you do not need Visual Studio or XCode, simply a Python 2.7 interpreter.
*Currently we only include a 32-bit libary for Windows.*


C++ Details
==============

(coming soon. But basically you create a StoredCommands object, call its functions to append to the command stream, and then serialize this object and send the data to meshmixer via UDP socket. You have to do that part yourself! We recommend using Qt, it is really easy, and is what we use on the meshmixer side. IP/Port details below)


Python Details
==============

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

(Have you made improvements to the mm module? Submit a pull request!)



Documentation ?  
===============

The C++ interface is not yet documented. 
API functions take the form of simple commands or query/responses, so usage
is straightforward. See the StoredCommands.h header for documentation of the
available functions, and valid strings/arguments.

For Python, preliminary documentation of the mm module is available in
\distrib\documentation\python_html (open index.html).
This documentation will be updated periodically.

Many examples of Python API usage are available inside the functions of
the mm Python module. Usage in other languages will be very similar.
	  

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



Changelog
===========
(only significant changes that will impact usage/compatibility)

#### [June 19, 2015]
- meshmixer 2.9, C# version

#### [December 18, 2014]
- OSX version added

#### [November 27, 2014]
- initial release, compatible with mm 2.7
 


