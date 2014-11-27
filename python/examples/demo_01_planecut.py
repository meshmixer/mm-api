# [RMS] this is a simple demo that just runs the plane cut command
#   and accepts the result. This cuts the current object in half.

import mmapi
from mmRemote import *
import mm

# initialize connection
remote = mmRemote()
remote.connect()

# run planeCut command
mm.begin_tool(remote, "planeCut")

# rotate cutting plane 90 degrees
rotation = mm.make_rotZ_matrix(3.14159*0.5)
mm.set_toolparam(remote, "rotation", rotation )

# accept result
mm.accept_tool(remote)

#done!
remote.shutdown()



