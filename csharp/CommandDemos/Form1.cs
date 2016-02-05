using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;

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
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("planeCut");
            sc.AppendCompleteToolCommand("accept");

            rc.ExecuteCommands(sc);

            rc.Shutdown();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            mm.RemoteControl rc = new mm.RemoteControl();
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

            rc.Shutdown();
        }


        private void queriesButton_Click(object sender, EventArgs e)
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            mm.Vector3 Min = new mm.Vector3(), Max = new mm.Vector3();
            rc.SelectionBoundingBox(ref Min, ref Max);

            outputTextBox.Text = String.Format("Bounding Box [{0},{1},{2}]  [{3},{4},{5}]",
                Min[0], Min[1], Min[2], Max[0], Max[1], Max[2]);

            rc.Shutdown();
        }





        private void testWriteButton_Click(object sender, EventArgs e)
        {
            mm.PackedMesh pm = new mm.PackedMesh();
            pm.InitializeToTestMesh();
            pm.Write("c:\\scratch\\livemesh_test.bin");
        }
    }
}
