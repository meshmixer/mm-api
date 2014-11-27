import mmapi;
from convert import *;
from tool import *;


def clear_face_selection(remote):
    """clear current selection (if there is one) and exit selection tool"""
    cancel_tool(remote)

def select_all(remote):
    """select all faces (will begin selection tool if not in it)"""
    cmd = mmapi.StoredCommands()
    cmd.AppendCompleteToolCommand("cancel")
    cmd.AppendBeginToolCommand("select")
    cmd.AppendSelectCommand_All()
    remote.runCommand(cmd)


def selection_utility_command(remote, command_name):
    """run selection utility command"""
    cmd = mmapi.StoredCommands()
    cmd.AppendSelectUtilityCommand( command_name )
    remote.runCommand(cmd)


def select_facegroups(remote, groups_list):
    """select facegroups"""
    cmd = mmapi.StoredCommands()
    vgroups = mmapi.vectori();
    for group in groups_list:
        vgroups.push_back(group);
    cmd2 = mmapi.StoredCommands()
    cmd2.AppendSelectCommand_ByFaceGroups(vgroups)
    remote.runCommand(cmd2)

    