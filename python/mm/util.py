import mmapi;
from frame import *;

# returns pair of 3-float tuples (min, max)
def get_selected_bounding_box(remote):
    cmd = mmapi.StoredCommands()
    key1 = cmd.AppendQueryCommand_GetBoundingBox()
    remote.runCommand(cmd)

    fMin = mmapi.floatArray(3)
    fMax = mmapi.floatArray(3)
    bOK = cmd.GetQueryResult_GetBoundingBox(key1, fMin, fMax)
    return ( (fMin[0],fMin[1],fMin[2]), (fMax[0],fMax[1],fMax[2]) )


# returns pair of 3-float tuples (min, max)
def get_face_selection_bounding_box(remote):
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

# returns pair of 3-float tuples (min, max)
def get_face_selection_centroid(remote):
    cmd = mmapi.StoredCommands()
    key1 = cmd.AppendQueryCommand_GetSelectedFacesCentroid()
    remote.runCommand(cmd)

    vCentroid = mmapi.floatArray(3)
    bOK = cmd.GetQueryResult_GetSelectedFacesCentroid(key1, vCentroid)
    if bOK:
        return ( vCentroid[0], vCentroid[1], vCentroid[2] )
    else:
        return ( 0,0,0 )


# args are 3-float origin,direction
# returns tuple (bHit, hit_frame) 
def find_nearest(remote, position):
    cmd = mmapi.StoredCommands()
    v = mmapi.vec3f()
    key = cmd.AppendQueryCommand_FindNearestPoint(position[0], position[1], position[2]);
    remote.runCommand(cmd)
    frame = mmapi.frame3f()
    bOK = cmd.GetQueryResult_FindNearestPoint(key, frame)
    hitFrame = mmFrame()
    hitFrame.setFromMM(frame)
    return (bOK, hitFrame)


# args are 3-float origin,direction
# returns tuple (bHit, hit_frame) 
def find_ray_hit(remote, ray_origin, ray_direction):
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


def select_inside_sphere(remote, sphere_center, sphere_radius):
    cmd = mmapi.StoredCommands()
    cmd.AppendSelectCommand_InsideSphere( sphere_center[0],sphere_center[1],sphere_center[2], sphere_radius )
    remote.runCommand(cmd)