import mmapi;
from convert import *;


def begin_tool(remote, tool_name):
    """Begin execution of the Tool called tool_name"""
    cmd = mmapi.StoredCommands()
    cmd.AppendBeginToolCommand(tool_name)
    remote.runCommand(cmd)

def accept_tool(remote):
    """Accept the current tool"""
    cmd = mmapi.StoredCommands()
    cmd.AppendCompleteToolCommand("accept")
    remote.runCommand(cmd)
def cancel_tool(remote):
    """Cancel the current tool"""
    cmd = mmapi.StoredCommands()
    cmd.AppendCompleteToolCommand("cancel")
    remote.runCommand(cmd)

def set_toolparam(remote, param_name, f):
    """Set the parameter param_name of the current tool. This function handles int,float,boolean,vector3,and matrix9 inputs"""
    cmd = mmapi.StoredCommands()
    if type(f) in (int, float, bool):
        key = cmd.AppendToolParameterCommand(param_name, f)
    elif len(f) == 3:
        key = cmd.AppendToolParameterCommand(param_name, f[0],f[1],f[2])
    elif len(f) == 9:
        key = cmd.AppendToolParameterCommand(param_name, f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8])
    remote.runCommand(cmd)
    return key



def get_toolparam(remote, param_name):
    """Returns the current value of the given Tool parameter, or empty list if the parameter is not found."""
    cmd = mmapi.StoredCommands()
    key = cmd.AppendGetToolParameterCommand(param_name)
    remote.runCommand(cmd)
    result_val = mmapi.any_result()
    bFound = cmd.GetToolParameterCommandResult(key, result_val)
    if bFound:
        if result_val.type == 0:
            return result_val.f
        elif result_val.type == 1:
            return result_val.i
        elif result_val.type == 2:
            return result_val.b
        elif result_val.type == 3:
            return (result_val.x, result_val.y, result_val.z)
        elif result_val.type == 4:
            return result_val.m
        else:
            return ()
    else:
        return ()



def tool_utility_command(remote, command_name, arg = -99):
    """Run a Tool utility command, with optional argument (see ::AppendToolUtilityCommand in StoredCommands.h)"""
    cmd = mmapi.StoredCommands()
    if ( isinstance(arg, int) and arg == -99 ):
        cmd.AppendToolUtilityCommand( command_name )
    else:
        cmd.AppendToolUtilityCommand( command_name, arg )
    remote.runCommand(cmd)


def toolquery_new_groups(remote):
    """Get a list of new groups created by last tool (NOTE: many Tools do not support this yet!)"""
    cmd = mmapi.StoredCommands()
    key = cmd.AppendToolQuery_NewGroups()
    remote.runCommand(cmd)
    new_groups = mmapi.vectori()
    bOK = cmd.GetToolQueryResult_NewGroups(key, new_groups)
    return vectori_to_list(new_groups)