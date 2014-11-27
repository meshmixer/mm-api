# short script examples. 
# Note that most of these use the SWIG-generated interface directly, 
# instead of the mm package

import os
import mmapi
from mmRemote import *
import mm

# initialize connection
remote = mmRemote()
remote.connect()


#[RMS] this is how to flatten the current set of facegroups into 2D paths and write as SVG
#cmd2 = mmapi.StoredCommands()
#cmd2.AppendSceneCommand_ClearTarget();
#cmd2.AppendBeginToolCommand("select")
#cmd2.AppendToolUtilityCommand("generateFlattenedSVGFromGroups", "C:/scratch/debug/")
#remote.runCommand(cmd2)


#[RMS] this is how to start up the volbrush and set the brush mode
#cmd2 = mmapi.StoredCommands()
#cmd2.AppendBeginToolCommand("volumeBrush")
#cmd2.AppendToolUtilityCommand("setPrimary", "flatten")
#remote.runCommand(cmd2)

#[RMS] this is how to invert the selection
#cmd2 = mmapi.StoredCommands()
#cmd2.AppendSelectUtilityCommand("invert")
#remote.runCommand(cmd2)

#[RMS] this is how to change the normal and color modes
#cmd2 = mmapi.StoredCommands()
#cmd2.ViewControl_SetTriangleColorMode(0)
#cmd2.ViewControl_SetSurfaceNormalMode(0)
#remote.runCommand(cmd2)

#[RMS] this is how to change object visibility
#cmd2 = mmapi.StoredCommands()
#obj_list = mm.list_objects(remote)
#cmd2.AppendSceneCommand_SetHidden(obj_list[0])
#cmd2.AppendSceneCommand_SetVisible(obj_list[0])
#cmd2.AppendSceneCommand_ShowAll();
#remote.runCommand(cmd2)

#[RMS] this is how to save a screenshot (will write to python\examples\screenshot.png)
cmd2 = mmapi.StoredCommands()
cmd2.AppendSceneCommand_SaveScreenShot(os.path.join( os.getcwd(), "screenshot.png" ))
remote.runCommand(cmd2)

#done!
remote.shutdown();



