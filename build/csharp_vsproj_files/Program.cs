using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace mmapi_test
{
    class Program
    {
        static void Main(string[] args)
        {
            mmapi_util.RemoteControl rc = new mmapi_util.RemoteControl();
            rc.Initialize();

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("planeCut");
            sc.AppendCompleteToolCommand("accept");

            rc.ExecuteCommands(sc);
            
        }
    }
}
