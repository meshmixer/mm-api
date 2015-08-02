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
#cmd2 = mmapi.StoredCommands()
#cmd2.AppendSceneCommand_SaveScreenShot(os.path.join( os.getcwd(), "screenshot.png" ))
#remote.runCommand(cmd2)

#[RMS] this is how to create a pivot and link it to an object in the scene
#objects = mm.list_objects(remote)
#f = mm.mmFrame()
#f.origin = (1,0,0)
#pivot_id = mm.create_pivot(remote, f)
#link_ok = mm.link_pivot(remote, pivot_id, objects[0])

mesh = mm.packedMesh()
v1 = mesh.appendVertex( (0,0,0) )
v2 = mesh.appendVertex( (1,0,0) )
v3 = mesh.appendVertex( (1,0,1) )
t = mesh.appendTriangle( (v1,v3,v2) )
mesh.write("c:/scratch/livemesh_python.bin")

cmd1 = mmapi.StoredCommands()
create_key = cmd1.AppendSceneCommand_AppendPackedMeshFile("c:/scratch/livemesh_python.bin");
remote.runCommand(cmd1)


cmd2 = mmapi.StoredCommands()
create_key = cmd2.AppendSceneCommand_CreateLiveMeshObject("c:/scratch/livemesh_python.bin");
remote.runCommand(cmd2)
port_name_vec = mmapi.vectorub()
obj_id = mmapi.any_result()
cmd2.GetSceneCommandResult_CreateLiveMeshObject(create_key, port_name_vec, obj_id)
port_name = mm.vectorub_to_string(port_name_vec)
portid = obj_id.i

for i in range(0,100):
    mesh.vertices[0] = (0, 0+i*0.01, 0)

    cmd_lock = mmapi.StoredCommands()
    cmd_lock.AppendSceneCommand_RequestLiveMeshLock(port_name);
    remote.runCommand(cmd_lock);

    mesh.write("c:/scratch/livemesh_python.bin")

    cmd_update = mmapi.StoredCommands()
    cmd_update.AppendSceneCommand_ReleaseLiveMeshLock(port_name);
    cmd_update.AppendSceneCommand_NotifyLiveMeshUpdate(port_name);
    remote.runCommand(cmd_update)


#done!
remote.shutdown();



