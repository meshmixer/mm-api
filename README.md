Autodesk Meshmixer API package (mmapi)
v0.2 April 2014
copyright (c) 2014 Autodesk Corporation    http://www.autodesk.com
contact:   Ryan.Schmidt@Autodesk.com  http://autodeskresearch.com/people/ryanschmidt

==============
What is mmapi?
==============
Meshmixer Desktop is remotely-controllable via a C++ API. The actual API
header is part of the meshmixer-desktop code, however I have also set up 
SWIG so that it is possible to generate bindings for other languages.

This mmapi package currently includes C# and Python bindings, with various 
amounts of example code.


========
Setup
========

To enable remote control of meshmixer, you must insert the following line
in your meshmixer.ini file, under the [Options] section:
   
   EnableRemoteStoredCommandServer=true
   
Now on startup meshmixer will listen for remote commands (see details below).

To build API packages from scratch, run the top-level make_package_(language).bat files.
For example, to build python, run make_package_python.bat.
This will require Visual Studio (2012?) to be installed on your machine.
The output API wrapper will be placed in distrib\(language)\, eg distrib\python

If you received a .zip file, then distrib\python may already exist. In this case
you do not necessarily need Visual Studio to use the API, just a suitable compiler/interpreter
for the language you want to use (ie for Python, you would need Python 2.7 installed)


=======
Python
=======

Python is the language that has been used most extensively, with the API.
In examples\python\ you will find various example python code. These examples
can be run from Visual Studio 2012, using Python Tools for Visual Studio (http://pytools.codeplex.com/).

This example folder also includes a python package in the examples\python\mm\ subdirectory.
This package provides "clean" wrappers around the SWIG-generated mmapi (which is kind of ugly).
In particular, there are functions which cleanly translate between SWIG data structures
like C++-vector-wrapper and our C++ vec3f/matrix3f, and standard python data structures
like lists and tuples.

Although this mm package is still in development, we strongly recommend you use it whenever
possible, rather than using the SWIG API directly.

	  
	  
======
Notes
======

The API is not strictly a C++ API. It is actually just a binary stream,
sent over a network socket. Currently:
 - UDP sockets are used, so the command packets have a maximum size. 
 - hostname is hardcoded to localhost (127.0.0.1), listening on port 0xAFCF=45007
 - responses are sent to localhost, port 0xAFDF=45023
 
This is perhaps not the best architecture, but it was easy and is
good enough for now. The main issue is sending large amounts of data,
but nothing in the API really supports that yet.

