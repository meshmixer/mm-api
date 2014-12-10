import mmapi;
from frame import *;

def get_selected_bounding_box(remote):
    """return the axis-aligned bounding box of the selected objects as two 3-tuples (min, max)"""
    cmd = mmapi.StoredCommands()
    key1 = cmd.AppendQueryCommand_GetBoundingBox()
    remote.runCommand(cmd)

    fMin = mmapi.floatArray(3)
    fMax = mmapi.floatArray(3)
    bOK = cmd.GetQueryResult_GetBoundingBox(key1, fMin, fMax)
    return ( (fMin[0],fMin[1],fMin[2]), (fMax[0],fMax[1],fMax[2]) )


def get_face_selection_bounding_box(remote):
    """return the axis-aligned bounding box of the selected faces as two 3-tuples (min, max). Requires active face selection."""
    cmd = mmapi.StoredCommands()
    key1 = cmd.AppendQueryCommand_GetSelectedFacesBoundingBox()
    remote.runCommand(cmd)

    fMin = mmapi.floatArray(3)
    fMax = mmapi.floatArray(3)
    bOK = cmd.GetQueryResult_GetSelectedFacesBoundingBox(key1, fMin, fMax)
    if bOK:
        return ( (fMin[0],fMin[1],fMin[2]), (fMax[0],fMax[1],fMax[2]) )
    else:
        return ( (0,0,0), (0,0,0) )

def get_face_selection_centroid(remote):
    """return the geometric centroid of the selected faces as a 3-tuple. Requires active face selection."""
    cmd = mmapi.StoredCommands()
    key1 = cmd.AppendQueryCommand_GetSelectedFacesCentroid()
    remote.runCommand(cmd)

    vCentroid = mmapi.floatArray(3)
    bOK = cmd.GetQueryResult_GetSelectedFacesCentroid(key1, vCentroid)
    if bOK:
        return ( vCentroid[0], vCentroid[1], vCentroid[2] )
    else:
        return ( 0,0,0 )


def find_nearest(remote, position):
    """Find the nearest point on the 3D surface to the input 3-tuple. Returns a tuple (bOK, hitFrame), where bOK is a boolean indicating if a nereast point was found, and hitFrame is an mmFrame at the hit point, with Z axis oriented to the surface normal"""
    cmd = mmapi.StoredCommands()
    v = mmapi.vec3f()
    key = cmd.AppendQueryCommand_FindNearestPoint(position[0], position[1], position[2]);
    remote.runCommand(cmd)
    frame = mmapi.frame3f()
    bOK = cmd.GetQueryResult_FindNearestPoint(key, frame)
    hitFrame = mmFrame()
    hitFrame.setFromMM(frame)
    return (bOK, hitFrame)


def find_ray_hit(remote, ray_origin, ray_direction):
    """Find the intersection of a ray (specified by 3-tuples for origin and direction) and the 3D surface. Returns a tuple (bOK, hitFrame), where bOK is a boolean indicating if a hit was found, and hitFrame is an mmFrame at the hit point, with Z axis oriented to the surface normal"""
    cmd = mmapi.StoredCommands()
    o = mmapi.vec3f()
    o.x = ray_origin[0]; o.y = ray_origin[1]; o.z = ray_origin[2]
    d = mmapi.vec3f()
    d.x = ray_direction[0]; d.y = ray_direction[1]; d.z = ray_direction[2]
    key = cmd.AppendQueryCommand_FindRayIntersection(o,d)
    remote.runCommand(cmd)
    frame = mmapi.frame3f()
    bOK = cmd.GetQueryResult_FindRayIntersection(key, frame)
    hitFrame = mmFrame()
    hitFrame.setFromMM(frame)
    return (bOK, hitFrame)

