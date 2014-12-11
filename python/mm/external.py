import os
import tempfile
import platform

#
# OpenSCAD connection (for geometry generation)
#

def locate_openscad():
    """local path to OpenSCAD binary on various platforms (currently hardcoded...)"""
    if ( platform.system() == "Windows" ):
        return "\"C:\\Program Files (x86)\\OpenSCAD\\openscad.exe\""
    elif ( platform.system() == "Darwin" ):
        return "/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD"
    else:
        return "openscad"       # assume this is right on linux...

def run_openscad_script(scad_file, output_file):
    """execute an OpenSCAD script stored in scad_file and write the result to output_file"""
    command = locate_openscad() + " -o " + output_file + " " + scad_file
    #print command    #debugging
    status = os.system(r'"%s"'  %command)
    return (status == 0)

def run_openscad_source(script_source, output_file):
    """execute OpenSCAD source code and write the result to output_file"""
    f = tempfile.NamedTemporaryFile(delete=False)
    f.write(script_source)
    f.close()
    command = locate_openscad() + " -o " + output_file + " " + f.name
    #print command  #debugging
    status = os.system(r'"%s"'  %command)
    os.unlink(f.name)
    return (status == 0)
