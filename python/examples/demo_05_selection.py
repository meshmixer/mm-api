# selection examples:
#   - list selected objects
#   - list selected facegroups

import mmapi
from mmRemote import *
import mm

# initialize connection
remote = mmRemote()
remote.connect()

# print list of selected object IDs
selected_objects = mm.list_selected_objects(remote)
print "selected objects: ", selected_objects

# print list of selected group IDs (empty if no selection)
groups_list = mm.list_selected_groups(remote)
print "selected groups: ", groups_list

#done!
remote.shutdown()



