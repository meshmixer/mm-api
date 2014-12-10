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