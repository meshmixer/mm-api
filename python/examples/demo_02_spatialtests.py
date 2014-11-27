# [RMS] This file demos some spatial query functions:
#    - get bounding box of selected object
#    - find ray/surface intersection
#    - find nearest point on surface to query point
#    - conversion between Scene and World coordinates

import mmapi
from mmRemote import *
import mm
from mm.mm_math import *

# initialize connection
r = mmRemote()
r.connect()

# get object-space bounding box of selected object
(fMin,fMax) = mm.get_selected_bounding_box(r)
print "scene bbox min: ", fMin
print "scene bbox max: ", fMax

# convert to world-space (ie current units)
fMinW = mm.to_world(r, fMin)
fMaxW = mm.to_world(r, fMax)
print "world bbox min: ", fMinW
print "world bbox max: ", fMaxW
print "one world unit in scene dimensions: ", mm.to_scene(r, 1.0)
print "\n"

# construct a ray from far-away-pt through center of bbox, along +Z axis
center = lerpv3(fMin, fMax, 0.5)
diag = subv3(fMax, fMin)
ray_dir = (0.0,0.0,1.0)
ray_origin = subv3(center, mulv3s(ray_dir, 25.0*max(diag)))

# hit-test ray against object
(bHit, hitFrame) = mm.find_ray_hit(r, ray_origin, ray_dir)
print "hit: ", bHit, " world pos: ", hitFrame.origin;

# find nearest point on sfc to (-2,0,0)
(bFound, nearFrame) = mm.find_nearest(r, (-2,0,0) )
print "found: ", bFound, " world pos: ", mm.toW(r, nearFrame.origin);

#done!
r.shutdown()



