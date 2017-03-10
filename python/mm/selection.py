import mmapi;
from convert import *;
from tool import *;


def clear_face_selection(remote):
    """Clear the current face selection (if there is one) and exit the Selection Tool"""
    cancel_tool(remote)

def select_all(remote):
    """Select all faces of the active object (will initialize a Selection Tool if necessary)"""
    cmd = mmapi.StoredCommands()
    cmd.AppendCompleteToolCommand("cancel")
    cmd.AppendBeginToolCommand("select")
    cmd.AppendSelectCommand_All()
    remote.runCommand(cmd)


def selection_utility_command(remote, command_name):
    """Run a selection utility command (see ::AppendSelectUtilityCommand in StoredCommands.h)"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSelectUtilityCommand( command_name )
    remote.runCommand(cmd)


def select_facegroups(remote, groups_list):
    """Select the faces which have a facegroup ID in groups_list"""
    cmd = mmapi.StoredCommands()
    vgroups = mmapi.vectori();
    for group in groups_list:
        vgroups.push_back(group);
    cmd2 = mmapi.StoredCommands()
    cmd2.AppendSelectCommand_ByFaceGroups(vgroups)
    remote.runCommand(cmd2)

def select_inside_sphere(remote, sphere_center, sphere_radius):
    """Select the faces inside the sphere with given center/radius. Requires active Selection Tool."""
    cmd = mmapi.StoredCommands()
    cmd.AppendSelectCommand_InsideSphere( sphere_center[0],sphere_center[1],sphere_center[2], sphere_radius )
    remote.runCommand(cmd)    


def select_hole(remote, hole_index, mode = 0):
    """Select the faces which have a facegroup ID in groups_list"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSelectCommand_HoleBorderRing(hole_index, mode)
    remote.runCommand(cmd)

def select_triangles(remote, triangles_list, mode = 0):
    """Select the listed triangles"""
    cmd = mmapi.StoredCommands()
    vtris = mmapi.vectori();
    for t in triangles_list:
        vtris.push_back(t);
    cmd2 = mmapi.StoredCommands()
    cmd2.AppendSelectCommand_ByTriangleID(vtris, mode)
    remote.runCommand(cmd2)



def list_selected_groups(remote):
    """Returns a list of unique facegroup IDs for the current face selection (requires an active selection)"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSelectCommand_ListSelectedFaceGroups()
    remote.runCommand(cmd1)
    groups1 = mmapi.vectori()
    cmd1.GetSelectCommandResult_ListSelectedFaceGroups(key1, groups1);
    return vectori_to_list(groups1);

def list_selected_triangles(remote):
    """Returns a list of currently-selected triangles (requires an active selection)"""
    cmd1 = mmapi.StoredCommands()
    key1 = cmd1.AppendSelectCommand_ListSelectedTriangles()
    remote.runCommand(cmd1)
    groups1 = mmapi.vectori()
    cmd1.GetSelectCommandResult_ListSelectedTriangles(key1, groups1);
    return vectori_to_list(groups1);
