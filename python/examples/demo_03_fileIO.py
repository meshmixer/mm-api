# demo read/write of .MIX files

import os
import mmapi
from mmRemote import *
import mm;

# initialize connection
remote = mmRemote()
remote.connect()

# run commands

examples_dir = os.getcwd()
mm.open_mix(remote, os.path.join( examples_dir, "sphere.mix" ))
mm.save_mix(remote, os.path.join( examples_dir, "sphere.sphere_exported" ))

#done!
remote.shutdown()



