using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SharpGL;
using System.Timers;

namespace LinkedMeshDemo
{
    /// <summary>
    /// The main form class.
    /// </summary>
    public partial class SharpGLForm : Form
    {
        mm.RemoteControl mmRemote;

        public mm.PackedMesh mesh;
        public mm.MeshChangedListener signal;

        public System.Timers.Timer brush_timer;

        /// <summary>
        /// Initializes a new instance of the <see cref="SharpGLForm"/> class.
        /// </summary>
        public SharpGLForm()
        {
            InitializeComponent();

            mesh = new mm.PackedMesh();
            signal = new mm.MeshChangedListener(0xACDF, "c:\\scratch\\livemesh_out.bin");

            signal.ForceReadPort();
            signal.GetLastMesh(mesh);

            signal.Start();
            signal.OnMeshChanged += new mm.MeshChangedHandler(OnMeshUpdated);

            mmRemote = new mm.RemoteControl();
            mmRemote.Initialize();
        }

        private void OnMeshUpdated(object sender)
        {
            // post to main thread...
            if (InvokeRequired)
            {
                BeginInvoke(new mm.MeshChangedHandler(OnMeshUpdated), new object[] { sender });
                return;
            }
            signal.GetLastMesh(mesh);
            //mesh.Read("c:\\scratch\\livemesh_out.bin");
        }

        private void SharpGLForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.signal.Stop();

            mmRemote.Shutdown();
        }


        private void draw_mesh(mm.PackedMesh m, OpenGL gl)
        {
            gl.Begin(OpenGL.GL_TRIANGLES);

            gl.Color(1, 0, 0, 1);

            for (int ti = 0; ti < m.TriangleCount; ++ti)
            {
                int[] t = m.GetTriangle(ti);
                for (int j = 0; j < 3; ++j)
                {
                    if (m.HasNormals)
                        gl.Normal(m.GetNormal(t[j]));
                    if (m.HasColors)
                        gl.Color(m.GetColor(t[j]));
                    float[] v = m.GetVertex(t[j]);
                    gl.Vertex(v);
                }
            }

            gl.End();
        }

        /// <summary>
        /// Handles the OpenGLDraw event of the openGLControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="RenderEventArgs"/> instance containing the event data.</param>
        private void openGLControl_OpenGLDraw(object sender, RenderEventArgs e)
        {
            //  Get the OpenGL object.
            OpenGL gl = openGLControl.OpenGL;

            gl.Disable(OpenGL.GL_LIGHTING);

            //  Clear the color and depth buffer.
            gl.Clear(OpenGL.GL_COLOR_BUFFER_BIT | OpenGL.GL_DEPTH_BUFFER_BIT);

            //  Load the identity matrix.
            gl.LoadIdentity();

            //  Rotate around the Y axis.
            gl.Rotate(rotation, 0.0f, 1.0f, 0.0f);

            //  Draw a coloured pyramid.
            draw_mesh(mesh, gl);

            //  Nudge the rotation.
            //rotation += 3.0f;
        }



        /// <summary>
        /// Handles the OpenGLInitialized event of the openGLControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void openGLControl_OpenGLInitialized(object sender, EventArgs e)
        {
            //  TODO: Initialise OpenGL here.

            //  Get the OpenGL object.
            OpenGL gl = openGLControl.OpenGL;

            //  Set the clear color.
            gl.ClearColor(0.5f, 0.5f, 0.5f, 0);

            gl.Enable(OpenGL.GL_LIGHTING);
            gl.Enable(OpenGL.GL_LIGHT0);
            gl.Enable(OpenGL.GL_COLOR_MATERIAL, OpenGL.GL_FRONT_AND_BACK);
        }

        /// <summary>
        /// Handles the Resized event of the openGLControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void openGLControl_Resized(object sender, EventArgs e)
        {
            //  TODO: Set the projection matrix here.

            //  Get the OpenGL object.
            OpenGL gl = openGLControl.OpenGL;

            //  Set the projection matrix.
            gl.MatrixMode(OpenGL.GL_PROJECTION);

            //  Load the identity.
            gl.LoadIdentity();

            //  Create a perspective transformation.
            gl.Perspective(60.0f, (double)Width / (double)Height, 0.01, 100.0);

            //  Use the 'look at' helper function to position and aim the camera.
            gl.LookAt(0, 1, 3, 0, 0, 0, 0, 1, 0);

            //  Set the modelview matrix.
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
        }

        /// <summary>
        /// The current rotation.
        /// </summary>
        private float rotation = 0.0f;




        float fAccumTime = 0;
        float fBrushTime = 0.1f;
        private void brushStrokeButton_Click(object sender, EventArgs e)
        {
            if (brush_timer == null)
            {
                brush_timer = new System.Timers.Timer();
                brush_timer.Elapsed += brush_timer_elapsed;
                brush_timer.Interval = 100.0f;
                brush_timer.Start();

                StoredCommands sc = new StoredCommands();
                sc.AppendBeginToolCommand("volumeBrush");
                mmRemote.ExecuteCommands(sc);
            }
            else
            {
                brush_timer.Stop();
                brush_timer = null;

                StoredCommands sc = new StoredCommands();
                sc.AppendCompleteToolCommand("accept");
                mmRemote.ExecuteCommands(sc);
            }
        }

        private void brush_timer_elapsed(object sender, ElapsedEventArgs e)
        {
            float height = 0.1f;

            fAccumTime += fBrushTime;
            float r0 = 0.1f + fAccumTime * 0.02f;
            float r1 = 0.1f + (fAccumTime + fBrushTime) * 0.02f;
            float t0 = fAccumTime * 2.0f;
            float t1 = (fAccumTime + fBrushTime) * 2.0f;
            float x0 = r0 * (float)Math.Cos(t0); float y0 = r0 * (float)Math.Sin(t0);
            float x1 = r1 * (float)Math.Cos(t1); float y1 = r1 * (float)Math.Sin(t1);

            StoredCommands sc = new StoredCommands();
            vec3f v0 = new vec3f(); v0.x = x0; v0.y = height; v0.z = y0;
            vec3f v1 = new vec3f(); v1.x = x1; v1.y = height; v1.z = y1;
            sc.AppendActionCommand_LinearBrushStroke3D(v0, v1, 16);
            mmRemote.ExecuteCommands(sc);
        }


    }
}
