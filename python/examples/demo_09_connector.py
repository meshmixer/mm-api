import os
import sys
import mmapi
from mmRemote import *
import mm
from mm.mm_math import *
import math




def openscad_matrix(m9):
    return "[ " + '[{0},{1},{2},0],'.format(m9[0],m9[1],m9[2]) +  '[{0},{1},{2},0],'.format(m9[3],m9[4],m9[5]) + '[{0},{1},{2},0],'.format(m9[6],m9[7],m9[8]) + "[0,0,0,1] ]";
def openscad_vector(v3):
    return '[{0},{1},{2}]'.format(v3[0],v3[1],v3[2]);

def openscad_cylinder_pp(v0, v1, r):
    axis = subv3(v1,v0)
    dist = lenv3(axis)
    axis = normalizev3(axis)
    rotMat = make_align_axis_matrix((0,0,1), axis)
    rotation = "multmatrix(m = " + openscad_matrix(rotMat) + ") "
    translation = "translate(v = " + openscad_vector(v0) + ") "
    cylinder = 'cylinder(h = {0}, r = {1})'.format(dist, r);
    return translation + rotation + cylinder + ";";



output_filename = "c:\\scratch\\AAAA_connector.off"

scad_script = ""
connector_dirs = [ (1,0,0), (0,1,0), (0,0,1) ]
#connector_dirs = [ (1,1,0), (1,1,1) ]
connector_length = 20
post_radius = 5
post_tolerance = 0.25
origin = (1,1,1)
scad_script += "union() {\n"
for dir in connector_dirs:
    cyl = openscad_cylinder_pp(origin, addv3(origin,mulv3s(dir,connector_length)), post_radius)
    scad_script = scad_script + cyl + "\n";
scad_script += "}\n"

print "script is: "
print scad_script;

mm.run_openscad_source(scad_script, output_filename)