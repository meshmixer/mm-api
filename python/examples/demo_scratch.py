# short script examples. 
# Note that most of these use the SWIG-generated interface directly, 
# instead of the mm package

import os
import mmapi
from mmRemote import *
import mm
from mm.mm_math import *
from mm.convert import *

import ctypes

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
#cmd2 = mmapi.StoredCommands()
#cmd2.AppendSceneCommand_SaveScreenShot(os.path.join( os.getcwd(), "screenshot.png" ))
#remote.runCommand(cmd2)

#[RMS] this is how to create a pivot and link it to an object in the scene
#objects = mm.list_objects(remote)
#f = mm.mmFrame()
#f.origin = (1,0,0)
#pivot_id = mm.create_pivot(remote, f)
#link_ok = mm.link_pivot(remote, pivot_id, objects[0])

#cmd2 = mmapi.StoredCommands()
#cmd2.AppendBeginToolCommand("select")
#cmd2.ViewControl_TakeFocus();
#remote.runCommand(cmd2)

#mm.begin_tool(remote, "volumeBrush")
#mm.tool_utility_command(remote, "setPrimary", "drag")
#mm.tool_utility_command(remote, "setSecondary", "bubbleSmooth")

#mm.select_printer(remote, "replicator 2")


#mm.begin_tool(remote, "remesh")
#mm.set_toolparam(remote, "goalType", 2)

# mmapi.mat3f param read/write
#mm.begin_tool(remote, "transform");
#mat1 = to_mat3f( (0.36,0.48,-0.8, -0.8,0.6,0, 0.48,0.64,0.6) )
#mm.set_toolparam(remote, "rotation", mat1)
#mat2 = mm.get_toolparam_mat3f(remote, "rotation")

obj_list = mm.list_objects(remote)
uuid = mm.get_object_uuid(remote, obj_list[0])
(found, id) = mm.find_object_by_uuid(remote, uuid)

#done!
remote.shutdown();



