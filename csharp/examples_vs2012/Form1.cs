using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace examples
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            mmapi_util.RemoteControl rc = new mmapi_util.RemoteControl();
            rc.Initialize();

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("planeCut");
            sc.AppendCompleteToolCommand("accept");

            rc.ExecuteCommands(sc);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            mmapi_util.RemoteControl rc = new mmapi_util.RemoteControl();
            rc.Initialize();

            // you could import a mesh using this command:
            //sc.AppendSceneCommand_AppendMeshFile(...);

            StoredCommands sc = new StoredCommands();
            // [RMS] select entire mesh
            sc.AppendBeginToolCommand("select");
            sc.AppendSelectCommand_All();
            // [RMS] start remesh tool
            sc.AppendBeginToolCommand("remesh");
            // [RMS] configure parameters
            sc.AppendToolParameterCommand("density", 0.2f);
            sc.AppendToolParameterCommand("smooth", 1.0f);
            // [RMS] accept result
            sc.AppendCompleteToolCommand("accept");
            sc.AppendCompleteToolCommand("cancel");     // [RMS] to exit out of selection

            rc.ExecuteCommands(sc);

            // you could export result using this command
            //sc.AppendSceneCommand_ExportMeshFile_CurrentSelection(...);

        }
    }
}
