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

def list_selected_groups(remote):
    """Returns a list of unique facegroup IDs for the current face selection (requires an active selection)"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSelectCommand_ListSelectedFaceGroups()
    remote.runCommand(cmd1)
    groups1 = mmapi.vectori()
    cmd1.GetSelectCommandResult_ListSelectedFaceGroups(key1, groups1);
    return vectori_to_list(groups1);

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