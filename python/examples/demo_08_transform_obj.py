# [RMS] transformation & boolean demo. Shows how to:
#   - import a mesh file and identify new objects
#   - get spatial information about the current selection, ie position & sfc normal
#   - Transform a mesh to be aligned with a 3D position/normal
#   - do a boolean operation
#  !!! Requires an initial Face Selection. The imported part will be
#      positioned at/near the center of the selected region
import os
import sys
import mmapi
from mmRemote import *
import mm

# assumption: we are running 
examples_dir = os.getcwd()
part_filename = os.path.join( examples_dir, "cylinder.obj" )

# initialize connection
remote = mmRemote()
remote.connect()

# check that we have a selection
cur_groups = mm.list_selected_groups(remote)
if len(cur_groups) == 0:
    print "this script requires an active Face Selection"
    raw_input("Press Enter to exit...")
    os._exit(1)

# assumption: we have object we want to modify selected,
# and there is a selection made on this object
obj_list = mm.list_selected_objects(remote)

# find center of current selection, and then closest point on surface
# this gives us a 3D frame we can align an object to
centroid = mm.get_face_selection_centroid(remote)
sel_ctr = centroid
(bFound, selFrame) = mm.find_nearest(remote, sel_ctr)

# exit out of selection tool
mm.clear_face_selection(remote)

# import part we want to position at selection
new_objs = mm.append_objects_from_file(remote, part_filename);

# select imported part
mm.select_objects(remote, new_objs)

# get bbox of part, so that we can put origin at bottom of object if desired 
# (we are assuming that in its file, the part is positioned at the origin, on the ground plane)
(min,max) = mm.get_selected_bounding_box(remote)

# start transform tool
mm.begin_tool(remote, "transform")
cur_origin = mm.get_toolparam(remote, "origin")
dy = cur_origin[1] - min[1]

# compute and apply rotation
rotation = mm.make_matrix_from_axes(selFrame.x, mm.negv3(selFrame.z), selFrame.y )
mm.set_toolparam(remote, "rotation", rotation )

# translate origin of part to frame origin
translate = mm.subv3( selFrame.origin, cur_origin )
# uncomment this line to shift along frame Z, this will place bottom of part on surface (ie at frame origin)
#translate = mm.addv3( translate, mm.mulv3s( selFrame.z, dy ) )
mm.set_toolparam(remote, "translation", translate )

# accept xform
mm.accept_tool(remote)

# make a copy of the object, because boolean will delete it and it is useful to be able to see where the part was
mm.begin_tool(remote, "duplicate")
mm.accept_tool(remote)

# select original object and cylinder we imported
mm.select_objects(remote, [obj_list[0], new_objs[0]] )

# do a boolean difference to subtract the object
mm.begin_tool(remote, "difference")
mm.accept_tool(remote)

#done!
remote.shutdown()



