# short script examples. 
# Note that most of these use the SWIG-generated interface directly, 
# instead of the mm package

import os
import mmapi
from mmRemote import *
import mm

# initialize connection
r = mmRemote()
r.connect()

mm.begin_tool(r, "makePattern")
mm.set_toolparam(r, "pattern", 20)

# set default pipe diameter *in world units* (eg millimeters)
pipe_diam = 2.0
mm.set_toolparam(r, "dimension1", pipe_diam)

# now we can create the pattern. However, the numbers we pass for the 
# start and end points are not new diameters, the are dimensionless
# scaling factors applied to the default pipe_diam. So, 1.0=pipe_diam,
# 2.0 = pipe_diam*2, etc. And if you want to specify a specific diameter,
# you would send (target_diam/pipe_diam) 
#

# create pattern in **Scene** coordinates
#mm.append_pattern_segment(r, (0,0,0), (0,0.5,0), 1.0, 1.5 )
#mm.append_pattern_segment(r, (0,0,0), (0.5,0.0,0), 1.0, 2.0 )
#mm.append_pattern_segment(r, (0,0,0), (0.0,0.0,0.5), 1.0, 3.0 )

# create pattern in **World** coords  (eg millimeters/etc). 
mm.append_pattern_segment(r, (0,10,0), (0,25,0), 1.0, 1.5, False )
mm.append_pattern_segment(r, (0,10,0), (10,10.0,0), 1.0, 2.0, False )
mm.append_pattern_segment(r, (0,10,0), (0.0,10,10), 1.0, 3.0, False )

# Optionally send this to compute the pattern result. Otherwise you can click Update in the UI.
# If you don't send this, you will see the preview render, which is significantly faster...
#mm.tool_utility_command(r, "update")

#done!
r.shutdown();



