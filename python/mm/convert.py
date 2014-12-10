import mmapi;

def vectori_to_list(vi):
    """convert a SWIG vectori object to a Python list"""
    sz = vi.size()
    lst = []
    for i in xrange(0,sz):
        lst.append( vi[i] )
    return lst

def vectorub_to_string(vub):
    """convert a SWIG vectorub object to a Python string"""
    sz = vub.size()
    lst = []
    for i in xrange(0,sz-1):    # skip trailing null
        lst.append( vub[i] )
    return str(bytearray(lst));


def to_world_xyz(remote, x,y,z):
    """convert a 3D point from Scene space to World space"""
    vScene = mmapi.floatArray(3)
    vScene[0] = x
    vScene[1] = y
    vScene[2] = z
    cmd2 = mmapi.StoredCommands()
    ckey = cmd2.AppendQueryCommand_ConvertPointToWorld(vScene)
    remote.runCommand(cmd2);
    vWorld = mmapi.floatArray(3)
    cmd2.GetQueryResult_ConvertPointToWorld(ckey, vWorld)
    return (vWorld[0],vWorld[1],vWorld[2])
def to_world_f(remote, f):
    """convert a scalar dimension from Scene space to World space"""
    cmd2 = mmapi.StoredCommands()
    ckey = cmd2.AppendQueryCommand_ConvertScalarToWorld(f)
    remote.runCommand(cmd2);
    vWorld = mmapi.floatArray(1)
    cmd2.GetQueryResult_ConvertScalarToWorld(ckey, vWorld)
    return vWorld[0]
def to_world(remote, v):
    """convert input from Scene space to World space (dimension or 3D point)"""
    if ( isinstance(v,float) ):
        return to_world_f(remote, v)
    else:
        return to_world_xyz(remote, v[0],v[1],v[2])
def toW(remote, v):
    """same as to_world(), but shorter"""
    return to_world(remote,v)

def to_scene_xyz(remote, x,y,z):
    """convert a 3D point from World space to Scene space"""
    vWorld = mmapi.floatArray(3)
    vWorld[0] = x
    vWorld[1] = y
    vWorld[2] = z
    cmd2 = mmapi.StoredCommands()
    ckey = cmd2.AppendQueryCommand_ConvertPointToScene(vWorld)
    remote.runCommand(cmd2);
    vScene = mmapi.floatArray(3)
    cmd2.GetQueryResult_ConvertPointToScene(ckey, vScene)
    return (vScene[0],vScene[1],vScene[2])
def to_scene_f(remote, f):
    """convert a scalar dimension from World space to Scene space"""
    cmd2 = mmapi.StoredCommands()
    ckey = cmd2.AppendQueryCommand_ConvertScalarToScene(f)
    remote.runCommand(cmd2);
    vScene = mmapi.floatArray(1)
    cmd2.GetQueryResult_ConvertScalarToScene(ckey, vScene)
    return vScene[0]
def to_scene(remote, v):
    """convert input from World space to Scene space (dimension or 3D point)"""
    if ( isinstance(v,float) ):
        return to_scene_f(remote, v)
    else:
        return to_scene_xyz(remote, v[0],v[1],v[2])
def toS(remote, v):
    """same as to_scene(), but shorter"""
    return to_scene(remote,v)

