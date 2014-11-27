# Demo running Overhangs tool and generating a 3D-printing support structure

import mmapi
from mmRemote import *
import mm

# initialize connection
remote = mmRemote()
remote.connect()

# start overhangs tool
mm.begin_tool(remote, "overhangs")
# generate support structure
mm.tool_utility_command(remote, "generateSupport")
# convert support to solid
mm.tool_utility_command(remote, "convertToSolid", 0)
# done!
mm.accept_tool(remote)

remote.shutdown()



