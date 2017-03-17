import mmapi;
from convert import *;
from mm.frame import *

def open_mix(remote, path):
    """Open a .mix file at the given location"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_OpenMixFile(path)
    remote.runCommand(cmd)

def save_mix(remote, path):
    """Save the current scene as a .mix file at the given location"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_ExportMixFile(path)
    remote.runCommand(cmd)

def export_mesh(remote, path):
    """Export current selection to a mesh file such as stl"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_ExportMeshFile_CurrentSelection(path)
    remote.runCommand(cmd)

def append_objects_from_file(remote, filename):
    """Read objects from a mesh file, append them to the scene, and return a list of new object IDs"""
    cmd = mmapi.StoredCommands()
    key = cmd.AppendSceneCommand_AppendMeshFile(filename);
    remote.runCommand(cmd)
    new_objs_vec = mmapi.vectori()
    cmd.GetSceneCommandResult_AppendMeshFile(key, new_objs_vec)
    return vectori_to_list(new_objs_vec)

def list_objects(remote):
    """Return a list of object IDs for the current scene"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_ListObjects()
    remote.runCommand(cmd1)
    objects = mmapi.vectori()
    cmd1.GetSceneCommandResult_ListObjects(key1, objects)
    return vectori_to_list(objects)

def list_selected_objects(remote):
    """Returns a list of object IDs for the current set of selected scene objects"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_ListSelectedObjects()
    remote.runCommand(cmd1)
    objects = mmapi.vectori()
    cmd1.GetSceneCommandResult_ListObjects(key1, objects)
    return vectori_to_list(objects)


def select_objects(remote, objects_list):
    """Set the current objects selection to be the set of scene objects corresponding to the IDs in objects_list"""
    select_objects = mmapi.vectori();
    for object in objects_list:
        select_objects.push_back(object);
    cmd2 = mmapi.StoredCommands()
    cmd2.AppendSceneCommand_SelectObjects(select_objects)
    remote.runCommand(cmd2)


def delete_objects(remote, objects_list):
    """Delete the scene objects corresponding to the IDs in objects_list"""
    cur_selection = list_selected_objects(remote)
    select_objects(remote, objects_list)
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_DeleteSelectedObjects();
    remote.runCommand(cmd)
    select_objects(cur_selection, remote)


def get_object_name(remote, object_id):
    """Return the string name of the scene object with the given ID"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_GetObjectName(object_id)
    remote.runCommand(cmd)
    obj_name_vec = mmapi.vectorub()
    cmd.GetSceneCommandResult_GetObjectName(cmd_key, obj_name_vec)
    obj_name = vectorub_to_string(obj_name_vec)
    return obj_name
def set_object_name(remote, object_id, new_name):
    """Modify the string name of the scene object with the given ID"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_SetObjectName(object_id, new_name)
    remote.runCommand(cmd)

def get_object_uuid(remote, object_id):
    """Return the string uuid of the scene object with the given ID"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_GetObjectUUID(object_id)
    remote.runCommand(cmd)
    byte_vec = mmapi.vectorub()
    cmd.GetSceneCommandResult_GetObjectUUID(cmd_key, byte_vec)
    return vectorub_to_string(byte_vec)

def get_object_frame(remote, object_id):
    """get local frame of object"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_GetObjectFrame(object_id)
    remote.runCommand(cmd)
    cur_frame = mmapi.frame3f()
    cmd.GetSceneCommandResult_GetObjectFrame(cmd_key, cur_frame)
    f = mmFrame()
    f.set_frame3f(cur_frame)
    return f

def set_object_frame(remote, object_id, frame):
    """set local frame of object"""
    mmframe = frame.get_frame3f()
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_SetObjectFrame(object_id, mmframe)
    remote.runCommand(cmd)


def find_object_by_name(remote, obj_name):
    """Find the ID of the scene object with the given string name. Returns a 2-tuple (boolFound, object_id)"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_FindObjectByName(obj_name)
    remote.runCommand(cmd)
    result_val = mmapi.any_result()
    bFound = cmd.GetSceneCommandResult_FindObjectByName(cmd_key, result_val)
    return (bFound, result_val.i)

def find_object_by_uuid(remote, uuid):
    """Find the ID of the scene object with the given string name. Returns a 2-tuple (boolFound, object_id)"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_FindObjectByUUID(uuid)
    remote.runCommand(cmd)
    result_val = mmapi.any_result()
    bFound = cmd.GetSceneCommandResult_FindObjectByUUID(cmd_key, result_val)
    return (bFound, result_val.i)

def create_pivot(remote, frame):
    """create a pivot at a given mmFrame (not frame3f)"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_CreatePivot( frame.get_frame3f() )
    remote.runCommand(cmd)
    new_objs_vec = mmapi.vectori()
    cmd.GetSceneCommandResult_AppendMeshFile(cmd_key, new_objs_vec)
    return new_objs_vec[0]

def link_pivot(remote, pivot_id, obj_id):
    """link pivot to a given object"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_LinkPivot(pivot_id, obj_id)
    remote.runCommand(cmd)
    ok = cmd.GetSceneCommandResult_IsOK(cmd_key)
    return (ok != 0)

def unlink_pivot(remote, pivot_id):
    """unlink a pivot (no-op if pivot is not linked)"""
    cmd = mmapi.StoredCommands()
    cmd_key = cmd.AppendSceneCommand_UnlinkPivot(pivot_id)
    remote.runCommand(cmd)




def set_as_target(remote):
    """Set the current scene object as the Target object"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_SetAsTarget();
    remote.runCommand(cmd)
def clear_target(remote):
    """clear the current Target object setting"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_ClearTarget();
    remote.runCommand(cmd)




def list_number_of_holes(remote):
    """Returns a list of object IDs for the current set of selected scene objects"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendQueryCommand_ListNumberOfHoles()
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetQueryResult_ListNumberOfHoles(key1, result_val)
    return result_val.i



def select_printer(remote, printer_name):
    """Select printer based on name"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSceneCommand_SelectPrinter(printer_name)
    remote.runCommand(cmd)







def create_mesh(remote):
    """force MM to compactify mesh object, so that vertex/triangle ids are contiguous"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_CreateMesh()
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_CreateMesh(key1, result_val)
    return result_val.i

def compact_mesh(remote, objectid):
    """force MM to compactify mesh object, so that vertex/triangle ids are contiguous"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_CompactMesh(objectid)
    remote.runCommand(cmd1)

def update_mesh(remote, objectid):
    """force MM to update data structures for mesh object"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_UpdateMesh(objectid)
    remote.runCommand(cmd1)

def get_vertex_count(remote, objectid):
    """Returns number of vertices for object"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_GetVertexCount(objectid)
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_GetVertexCount(key1, result_val)
    return result_val.i

def get_triangle_count(remote, objectid):
    """Returns number of triangles for object"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_GetTriangleCount(objectid)
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_GetTriangleCount(key1, result_val)
    return result_val.i

def get_vertex_info(remote, objectid, vertexid):
    """returns tuple of 3-tuples of vertex info"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_GetVertexInfo(objectid, vertexid)
    remote.runCommand(cmd1)
    p = mmapi.vec3f() 
    n = mmapi.vec3f()
    c = mmapi.vec3f()
    cmd1.GetSceneCommandResult_GetVertexInfo(key1, p, n, c)
    return ( (p.x,p.y,p.z), (n.x,n.y,n.z), (c.x,c.y,c.z) )

def set_vertex_position(remote, objectid, vertexid, pos):
    cmd1 = mmapi.StoredCommands()
    p = mmapi.vec3f()
    p.x = pos[0]
    p.y = pos[1]
    p.z = pos[2]
    key1 = cmd1.AppendSceneCommand_SetVertexPosition(objectid, vertexid, p)
    remote.runCommand(cmd1)
def set_vertex_color(remote, objectid, vertexid, col):
    cmd1 = mmapi.StoredCommands()
    p = mmapi.vec3f()
    p.x = col[0]
    p.y = col[1]
    p.z = col[2]
    key1 = cmd1.AppendSceneCommand_SetVertexColor(objectid, vertexid, p)
    remote.runCommand(cmd1)


def append_vertex(remote, objectid, position, normal=(0,1,0), color=(1,1,1) ):
    """Appends a vertex to mesh and returns new VertexID"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_AppendVertex(objectid, to_vec3f(position), to_vec3f(normal), to_vec3f(color) )
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_AppendVertex(key1, result_val)
    return result_val.i


def append_triangle(remote, objectid, newtri, newgroup = 0):
    """Appends a triangle to mesh and returns new TriangleID"""
    cmd1 = mmapi.StoredCommands()
    t = to_vec3i(newtri)
    key1 = cmd1.AppendSceneCommand_AppendTriangle(objectid, to_vec3i(newtri), newgroup )
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_AppendTriangle(key1, result_val)
    return result_val.i


def get_triangle(remote, objectid, triangleid):
    """returns tuple of indices"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_GetTriangleIndices(objectid, triangleid)
    remote.runCommand(cmd1)
    t = mmapi.vec3i()
    cmd1.GetSceneCommandResult_GetTriangleIndices(key1, t)
    return (t.i, t.j, t.k)

def get_group(remote, objectid, triangleid):
    """returns group id for triangle"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_GetTriangleGroup(objectid, triangleid)
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_GetTriangleGroup(key1, result_val)
    return result_val.i


def allocate_group(remote, objectid):
    """allocates a new group id"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_AllocateNewGroupID(objectid)
    remote.runCommand(cmd1)
    result_val = mmapi.any_result()
    cmd1.GetSceneCommandResult_AllocateNewGroupID(key1, result_val)
    return result_val.i

def set_group(remote, objectid, triangleid, groupid):
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSceneCommand_SetTriangleGroup(objectid, triangleid, groupid)
    remote.runCommand(cmd1)


