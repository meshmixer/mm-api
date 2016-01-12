import os
import mmapi
from mmRemote import *
import mm

# change this to a path that exists on your computer
TEST_FILE_PATH="c:/scratch/livemesh_python.bin"

# initialize connection
remote = mmRemote()
remote.connect()

# create simple packedMesh object and store in file
mesh = mm.packedMesh()
n = (0,1,0)
v1 = mesh.appendVertex( (0,0,0), n, (1,0,0) )
v2 = mesh.appendVertex( (15,0,0), n, (0,1,0) )
v3 = mesh.appendVertex( (15,0,15), n, (0,0,1) )
t = mesh.appendTriangle( (v1,v3,v2) )
mesh.write(TEST_FILE_PATH)

# create new scene object from this file
cmd1 = mmapi.StoredCommands()
create_key = cmd1.AppendSceneCommand_AppendPackedMeshFile(TEST_FILE_PATH);
remote.runCommand(cmd1)

# register a new livemesh object based on this file and read back livemesh portname
cmd2 = mmapi.StoredCommands()
create_key = cmd2.AppendSceneCommand_CreateLiveMeshObject(TEST_FILE_PATH);
remote.runCommand(cmd2)
port_name_vec = mmapi.vectorub()
obj_id = mmapi.any_result()
cmd2.GetSceneCommandResult_CreateLiveMeshObject(create_key, port_name_vec, obj_id)
port_name = mm.vectorub_to_string(port_name_vec)
portid = obj_id.i

# animate this mesh
for i in range(0,2500):
    mesh.vertices[0] = (0, 0+i*0.1, 0)

    cmd_lock = mmapi.StoredCommands()
    cmd_lock.AppendSceneCommand_RequestLiveMeshLock(port_name);
    remote.runCommand(cmd_lock);

    mesh.write(TEST_FILE_PATH)

    cmd_update = mmapi.StoredCommands()
    cmd_update.AppendSceneCommand_ReleaseLiveMeshLock(port_name);
    cmd_update.AppendSceneCommand_NotifyLiveMeshUpdate(port_name);
    remote.runCommand(cmd_update)

#done!
remote.shutdown();



