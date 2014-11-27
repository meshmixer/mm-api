# demo scene object manipulations:
#   - list objects in scene
#   - select specific objects

import mmapi
from mmRemote import *
import mm

# initialize connection
remote = mmRemote()
remote.connect()

# request list objects in scene
objects = mm.list_objects(remote)

print "There are " + str(len(objects)) + " objects in the scene\n"

last_obj = objects[-1]

print "Selecting the last object with ID " + str(last_obj) + "...\n"

# select last object in list
select_list = [last_obj]
mm.select_objects(remote, select_list)

#done!
remote.shutdown()



