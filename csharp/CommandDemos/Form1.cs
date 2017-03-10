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
using mm;
using g3;

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
            plane_cut_test();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            remesh_test();
        }

        private void alignButton_Click(object sender, EventArgs e)
        {
            align_test();
        }

        private void setColorButton_Click(object sender, EventArgs e)
        {
            set_mesh_color_test();
        }



        private void smoothGroupsButton_Click(object sender, EventArgs e)
        {
            smooth_groups_test();
        }


        private void plane_cut_test()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("planeCut");
            sc.AppendCompleteToolCommand("accept");

            rc.ExecuteCommands(sc);

            rc.Shutdown();
        }


        private void remesh_test()
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


        private int get_object_id(mm.RemoteControl rc, string name)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_FindObjectByName(name);
            rc.ExecuteCommands(sc);
            any_result r = new any_result();
            bool bOK = sc.GetSceneCommandResult_FindObjectByName(key, r);
            return (bOK) ? r.i : -1;
        }
        private List<int> get_selected_objects(mm.RemoteControl rc)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_ListSelectedObjects();
            rc.ExecuteCommands(sc);
            vectori v = new vectori();
            bool bOK = sc.GetSceneCommandResult_ListSelectedObjects(key, v);
            return v.ToList();
        }

        private Vector3f get_pivot_location(mm.RemoteControl rc, int pivotID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetObjectFrame(pivotID);
            rc.ExecuteCommands(sc);
            frame3f f = new frame3f();
            bool bOK = sc.GetSceneCommandResult_GetObjectFrame(key, f);
            return new Vector3f(f.origin_x, f.origin_y, f.origin_z);
        }

        private void align_test()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            int sourcePivotID = get_object_id(rc, "source_pivot");
            int destPivotID = get_object_id(rc, "dest_pivot");

            System.Diagnostics.Debug.Assert(sourcePivotID != -1, "scene must have pivot with name source_pivot");
            System.Diagnostics.Debug.Assert(destPivotID != -1, "scene must have pivot with name dest_pivot");

            List<int> selected = get_selected_objects(rc);
            System.Diagnostics.Debug.Assert(selected != null && selected.Count == 1, "the mesh you want to move must be selected");

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("align");
            sc.AppendToolParameterCommand("sourceType", 13);
            sc.AppendToolParameterCommand("targetType", 13);
            sc.AppendToolUtilityCommand("setSourcePivot", sourcePivotID);
            sc.AppendToolUtilityCommand("setDestPivot", destPivotID);
            //sc.AppendToolParameterCommand("flip", true);
            sc.AppendCompleteToolCommand("accept");
        }


        private void smooth_groups_test()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            List<int> o = get_selected_objects(rc);
            int id = o[0];

            StoredCommands sc = new StoredCommands();
            uint key1 = sc.AppendSceneCommand_ListFaceGroups(id);
            rc.ExecuteCommands(sc);
            vectori groupv = new vectori();
            sc.GetSceneCommandResult_ListFaceGroups(key1, groupv);
            var vGroups = groupv.ToList();

            foreach (int g in vGroups)
            {
                StoredCommands cmd = new StoredCommands();
                vectori v = new vectori() { g };
                cmd.AppendSelectCommand_ByFaceGroups(v);
                cmd.AppendBeginToolCommand("smoothBoundary");
                cmd.AppendToolParameterCommand("preserveGroupBorders", false);
                cmd.AppendToolParameterCommand("preserveBoundary", true);
                cmd.AppendCompleteToolCommand("accept");
                cmd.AppendCompleteToolCommand("cancel");
                rc.ExecuteCommands(cmd);
            }

            rc.Shutdown();
        }



        private void set_mesh_color_test()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            List<int> selected = get_selected_objects(rc);
            System.Diagnostics.Debug.Assert(selected != null && selected.Count == 1, "the mesh you want to color must be selected");

            vec3f c = new vec3f();
            c.x = 1.0f; c.y = 0.0f; c.z = 0.0f;

            StoredCommands sc = new StoredCommands();
            sc.ViewControl_SetTriangleColorMode(0);
            sc.AppendSceneCommand_SetAllVertexColors(selected[0], c);
            sc.AppendSceneCommand_UpdateMesh(selected[0]);

            rc.ExecuteCommands(sc);

            rc.Shutdown();
        }



        /*
         *  [RMS] test code for dropping solid parts
         */

        private void drop_part_test_1()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();


            // [RMS] use a raycast to get an initial drop point for the part
            StoredCommands raycast_cmd = new StoredCommands();
            vec3f ray_o = new vec3f();
            ray_o.x = 0.0f; ray_o.y = 0.0f; ray_o.z = -100.0f;
            vec3f ray_d = new vec3f();
            ray_d.x = 0.0f; ray_d.y = 0.0f; ray_d.z = 1.0f;
            uint rayhit_key = raycast_cmd.AppendQueryCommand_FindRayIntersection(ray_o, ray_d);
            rc.ExecuteCommands(raycast_cmd);
            frame3f f = new frame3f();
            bool bHit = raycast_cmd.GetQueryResult_FindRayIntersection(rayhit_key, f);

            System.Diagnostics.Debug.Assert(bHit);

            // begin the interactive part drop. Note that the path is hardcoded to the part .obj file, however
            // the type of slash doesn't matter, we handle that internally
            StoredCommands drop_part_cmd = new StoredCommands();
            drop_part_cmd.AppendActionCommand_DropSolidPartAtPoint(
                "C:\\Users\\schmidr\\Documents/meshmixer\\libraries\\parts\\default\\Primitives\\1397485517_00001_cone.obj",
                //                "C:/GitHub/meshmixer/meshmixer_devel/libraries\\parts\\default\\Primitives\\1397485517_00001_cone.obj",
                 f, 0.0f, true);
            drop_part_cmd.AppendToolParameterCommand("operationType", 0);
            rc.ExecuteCommands(drop_part_cmd);


            // accept the drop position
            //StoredCommands accept_cmd = new StoredCommands();
            //accept_cmd.AppendActionCommand_AcceptDropPart();
            //rc.ExecuteCommands(accept_cmd);


            rc.Shutdown();
        }

        private void drop_part_test_2()
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();


            // [RMS] use a raycast to get an initial drop point for the part
            StoredCommands raycast_cmd = new StoredCommands();
            vec3f ray_o = new vec3f();
            ray_o.x = 0.0f; ray_o.y = 0.0f; ray_o.z = -100.0f;
            vec3f ray_d = new vec3f();
            ray_d.x = 0.0f; ray_d.y = 0.0f; ray_d.z = 1.0f;
            uint rayhit_key = raycast_cmd.AppendQueryCommand_FindRayIntersection(ray_o, ray_d);
            rc.ExecuteCommands(raycast_cmd);
            frame3f f = new frame3f();
            bool bHit = raycast_cmd.GetQueryResult_FindRayIntersection(rayhit_key, f);

            System.Diagnostics.Debug.Assert(bHit);

            StoredCommands drop_part_cmd2 = new StoredCommands();
            drop_part_cmd2.AppendActionCommand_DropSolidPartAtPoint(
                "C:/Users/schmidr/Documents/meshmixer/libraries/parts/user/My Parts/1469555765_00001_solidWatchTest.obj", f, 0.0f, true);
            drop_part_cmd2.AppendToolParameterCommand("operationType", 0);
            rc.ExecuteCommands(drop_part_cmd2);

            // accept the drop position
            //StoredCommands accept_cmd = new StoredCommands();
            //accept_cmd.AppendActionCommand_AcceptDropPart();
            //rc.ExecuteCommands(accept_cmd);

            rc.Shutdown();
        }




        private frame3f get_pivot_frame(mm.RemoteControl rc, int pivotID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetObjectFrame(pivotID);
            rc.ExecuteCommands(sc);
            frame3f f = new frame3f();
            bool bOK = sc.GetSceneCommandResult_GetObjectFrame(key, f);
            return f;
        }

        private void pivotTransformButton_Click(object sender, EventArgs e)
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            // in world units
            Vector3f frameTranslation = new Vector3f(0, 0, 20);

            int objectID = get_object_id(rc, "object");
            int pivotID = get_object_id(rc, "pivot");

            List<int> selected = get_selected_objects(rc);
            System.Diagnostics.Debug.Assert(selected != null && selected.Count == 1 && selected[0] == objectID, "the mesh you want to move must be selected");

            frame3f pivotFrame = get_pivot_frame(rc, pivotID);
            Vector3f x = new Vector3f(pivotFrame.tan1_x, pivotFrame.tan1_y, pivotFrame.tan1_z);
            Vector3f y = new Vector3f(pivotFrame.tan2_x, pivotFrame.tan2_y, pivotFrame.tan2_z);
            Vector3f z = new Vector3f(pivotFrame.origin_x, pivotFrame.origin_y, pivotFrame.origin_z);
            Vector3f worldTranslation = frameTranslation[0] * x
                + frameTranslation[1] * y + frameTranslation[2] * z;

            vec3f translate = worldTranslation.toVec3f();

            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("transform");
            sc.AppendToolParameterCommand("translationWorld", translate);
            rc.ExecuteCommands(sc);


            rc.Shutdown();
        }



        private void queriesButton_Click(object sender, EventArgs e)
        {
            mm.RemoteControl rc = new mm.RemoteControl();
            rc.Initialize();

            AxisAlignedBox3f bounds = new AxisAlignedBox3f();
            rc.SelectionBoundingBox(ref bounds);
            Vector3f Min = bounds.Min, Max = bounds.Max;

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
