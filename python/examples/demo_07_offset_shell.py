# Demo sequence of commands:
#   - run Offset tool on current object
#   - separate new shell and set as Target
#   - select a patch on original object and Attract To Target
#   (coded for default bunny, but should work with other objects)

import mmapi
from mmRemote import *
import mm

# initialize connection
r = mmRemote()
r.connect()

# save initial selection list
initial_selection = mm.list_selected_objects(r)

# generate shell using Offset tool
mm.select_all(r)
mm.begin_tool(r, "offset")
mm.set_toolparam(r, "offsetWorld", 1.25)
mm.accept_tool(r)

# now Offset is done, and shell is selected. Next we Separate it
mm.begin_tool(r, "separate")
mm.accept_tool(r)

# read back current selection (will be shell)
shell_objects = mm.list_selected_objects(r)
mm.set_object_name(r, shell_objects[0], "shell")

# set current as target (will be offset shell)
mm.set_as_target(r)

# restore original selection
mm.select_objects(r, initial_selection)

# select an area on the surface
(found,sfc_pt) = mm.find_nearest(r, (1,1,1))
mm.select_inside_sphere(r, sfc_pt.origin, mm.toS(r,10.0))

# run Attract to Target tool
mm.begin_tool(r, "attractToTarget")
mm.accept_tool(r)

# clear target and/or delete shell
#mm.clear_target(remote)
#mm.delete_objects(remote, shell_objects)

#done!
r.shutdown();



