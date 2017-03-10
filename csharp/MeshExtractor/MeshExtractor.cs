using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using mm;
using g3;

namespace MeshExtractor
{
    public partial class MeshExtractor : Form
    {

        RemoteControl mmRemote;

        public MeshExtractor()
        {
            InitializeComponent();
        }

        private void mesh_to_obj_button_Click(object sender, EventArgs e)
        {
            mmRemote = new mm.RemoteControl();
            if (mmRemote.Initialize()) {
                statusLabel.Text = "Connected!";
            } else {
                statusLabel.Text = "Could not connect!";
                return;
            }

            List<int> vSelected = mmRemote.ListSelectedObjects();
            if ( vSelected.Count == 1 ) {
                statusLabel.Text = "Reading...";
                DMesh3 mesh = g3Conversion.ReadMeshFromMM(mmRemote, vSelected[0]);
                statusLabel.Text = "Writing...";

                StandardMeshWriter writer = new StandardMeshWriter();
                WriteOptions options = WriteOptions.Defaults;
                options.bWriteGroups = true;
                writer.Write("c:\\scratch\\___EXPORT.obj", new List<WriteMesh>() { new WriteMesh(mesh) }, options);

                statusLabel.Text = "Done!";
            } else {
                MessageBox.Show("This command only works if a single object is selected");
            }


            mmRemote.Shutdown();
            statusLabel.Text = "(disconnected)";
        }





        private void bake_unwrap_uvs_Click(object sender, EventArgs e)
        {
           mmRemote = new mm.RemoteControl();
            if (mmRemote.Initialize()) {
                statusLabel.Text = "Connected!";
            } else {
                statusLabel.Text = "Could not connect!";
                return;
            }

            List<int> vSelected = mmRemote.ListSelectedObjects();
            if ( vSelected.Count == 1 ) {
                statusLabel.Text = "Reading...";
                DenseMeshUVSet uvSet;
                DMesh3 mesh = g3Conversion.ConvertUnwrapToUVs(mmRemote, out uvSet);
                //statusLabel.Text = "Writing...";

                StandardMeshWriter writer = new StandardMeshWriter();
                WriteOptions options = WriteOptions.Defaults;
                options.bWriteGroups = true;
                options.bWriteUVs = true;
                options.AsciiHeaderFunc = () => {
                    return "mtllib default.mtl\r\nusemtl Texture_0\r\n";
                };
                WriteMesh wm = new WriteMesh(mesh) { UVs = uvSet };
                writer.Write("c:\\scratch\\___EXPORT_UV.obj", new List<WriteMesh>() { wm }, options);

                statusLabel.Text = "Done!";
            } else {
                MessageBox.Show("This command only works if a single object is selected");
            }


            mmRemote.Shutdown();
            statusLabel.Text = "(disconnected)";
        }
    }
}
