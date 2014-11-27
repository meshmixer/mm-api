import mmapi
from mmRemote import *;

# initialize connection
remote = mmRemote();
remote.connect();

# construct commands to run
cmd = mmapi.StoredCommands()
cmd.AppendBeginToolCommand("planeCut")
cmd.AppendCompleteToolCommand("accept")

# execute  commands
remote.runCommand(cmd);

#done!
remote.shutdown();

