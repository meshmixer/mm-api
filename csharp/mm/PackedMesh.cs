using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace mm
{
    public class PackedMesh : ICloneable
    {
        // current as of [7/30/2015]

        // vertex flags
        const int PackedMesh_HasVertexNormals   = (1<<2);
        const int PackedMesh_HasVertexColorsRGB = (1<<3);
        const int PackedMesh_HasVertexUVs       = (1<<4);

        // triangle flags
        const int PackedMesh_HasTriangleGroups  = (1<<2);

        // currently unused
        public int VersionFlags;

        public int VertexFlags;
        public float[] Vertices;
        public float[] Normals;
        public float[] Colors;
        public float[] UVs;

        public int TriangleFlags;
        public int[] Triangles;
        public int[] FaceGroups;

        public PackedMesh()
        {
            Clear();
        }


        float[] fast_copy(float[] from, float[] to) {
            if (from == null)
                return null;
            if ( to.Length != from.Length )
                to = new float[from.Length];
            Buffer.BlockCopy(from, 0, to, 0, from.Length*sizeof(float));
            return to;
        }
        int[] fast_copy(int[] from, int[] to)
        {
            if (from == null)
                return null;
            if (to.Length != from.Length)
                to = new int[from.Length];
            Buffer.BlockCopy(from, 0, to, 0, from.Length * sizeof(int));
            return to;
        }

        public void CopyTo(PackedMesh mTo) {
            mTo.VersionFlags = this.VersionFlags;
            mTo.VertexFlags = this.VertexFlags;
            mTo.Vertices = fast_copy(this.Vertices, mTo.Vertices);
            mTo.Normals = fast_copy(this.Normals, mTo.Normals);
            mTo.Colors = fast_copy(this.Colors, mTo.Colors);
            mTo.UVs = fast_copy(this.UVs, mTo.UVs);
            mTo.TriangleFlags = this.TriangleFlags;
            mTo.Triangles = fast_copy(this.Triangles, mTo.Triangles);
            mTo.FaceGroups = fast_copy(this.FaceGroups, mTo.FaceGroups);
        }

        public object Clone()
        {
            PackedMesh mTo = new PackedMesh();
            this.CopyTo(mTo);
            return mTo;
        }


        public void Clear()
        {
            VersionFlags = 0;

            VertexFlags = 0;
            Vertices = new float[0];
            Normals = new float[0];
            Colors = new float[0];
            UVs = new float[0];

            TriangleFlags = 0;
            Triangles = new int[0];
            FaceGroups = new int[0];
        }


        public void InitializeToTestMesh()
        {
            VertexFlags = 0;
            Vertices = new float[] { 0, 0, 0, 1, 0, 0, 1, 0, 1 };
            TriangleFlags = 0;
            Triangles = new int[] { 0, 1, 2 };
        }

        public int VertexCount
        {
            get { return Vertices.Length / 3; }
        }
        public int TriangleCount
        {
            get { return Triangles.Length / 3; }
        }


        public bool HasNormals
        {
            get { return (VertexFlags & PackedMesh_HasVertexNormals) != 0; }
        }
        public bool HasColors
        {
            get { return (VertexFlags & PackedMesh_HasVertexColorsRGB) != 0; }
        }

        public float[] GetVertex(int i)
        {
            return new[] { Vertices[3*i], Vertices[3*i+1], Vertices[3*i+2] };
        }
        public float[] GetNormal(int i)
        {
            return new[] { Normals[3 * i], Normals[3 * i + 1], Normals[3 * i + 2] };
        }
        public float[] GetColor(int i)
        {
            return new[] { Colors[3 * i], Colors[3 * i + 1], Colors[3 * i + 2] };
        }
        public float[] GetUV(int i)
        {
            return new[] { UVs[2 * i], UVs[2 * i + 1], UVs[2 * i + 2] };
        }

        public int[] GetTriangle(int i)
        {
            return new[] { Triangles[3*i], Triangles[3*i+1], Triangles[3*i+2] };
        }
        public int GetFaceGroup(int i)
        {
            return FaceGroups[i];
        }

        public void Write(string sPath)
        {
            using (BinaryWriter writer = new BinaryWriter(File.Open(sPath, FileMode.Create)))
            {
                writer.Write(VersionFlags);

                writer.Write(Vertices.Length / 3);
                writer.Write(VertexFlags);

                var v_buffer = new byte[Vertices.Length * 4];
                Buffer.BlockCopy(Vertices, 0, v_buffer, 0, v_buffer.Length);
                writer.Write(v_buffer);

                writer.Write(Triangles.Length / 3);
                writer.Write(TriangleFlags);

                var t_buffer = new byte[Triangles.Length * 4];
                Buffer.BlockCopy(Triangles, 0, t_buffer, 0, t_buffer.Length);
                writer.Write(t_buffer);
            }
        }


        public void Read(string sPath)
        {
            Clear();

            using (BinaryReader reader = new BinaryReader(File.Open(sPath, FileMode.Open)))
            {
                byte[] version_flags = new byte[4];
                reader.Read(version_flags, 0, 4);
                VersionFlags = BitConverter.ToInt32(version_flags, 0);

                byte[] vertex_count = new byte[4];
                reader.Read(vertex_count, 0, 4);
                int nVertices = BitConverter.ToInt32(vertex_count, 0);

                byte[] vertex_flags = new byte[4];
                reader.Read(vertex_flags, 0, 4);
                VertexFlags = BitConverter.ToInt32(vertex_flags, 0);

                bool bHaveNormals = (VertexFlags & PackedMesh_HasVertexNormals) != 0;
                bool bHaveColors = (VertexFlags & PackedMesh_HasVertexColorsRGB) != 0;
                bool bHaveUVs = (VertexFlags & PackedMesh_HasVertexUVs) != 0;

                // [RMS] best we can do here is read into a byte array and
                //  then BlockCopy it to a float array. C# does not allow
                //  reading a binary block directly into a float[] array (wtf!)

                byte[] vertex_buf = new byte[nVertices * 3 * 4];
                reader.Read(vertex_buf, 0, vertex_buf.Count());
                this.Vertices = new float[nVertices * 3];
                Buffer.BlockCopy(vertex_buf, 0, this.Vertices, 0, vertex_buf.Length);

                if ( bHaveNormals ) {
                    reader.Read(vertex_buf, 0, vertex_buf.Count());
                    this.Normals = new float[nVertices * 3];
                    Buffer.BlockCopy(vertex_buf, 0, this.Normals, 0, vertex_buf.Length);
                }

                if ( bHaveColors ) {
                    reader.Read(vertex_buf, 0, vertex_buf.Count());
                    this.Colors = new float[nVertices * 3];
                    Buffer.BlockCopy(vertex_buf, 0, this.Colors, 0, vertex_buf.Length);
                }

                if ( bHaveUVs) {
                    byte[] uv_buf = new byte[nVertices*2*4];
                    reader.Read(uv_buf, 0, uv_buf.Count());
                    this.UVs = new float[nVertices * 2];
                    Buffer.BlockCopy(uv_buf, 0, this.UVs, 0, vertex_buf.Length);
                }

                byte[] triangle_count = new byte[4];
                reader.Read(triangle_count, 0, 4);
                int nTriangles = BitConverter.ToInt32(triangle_count, 0);

                byte[] triangle_flags = new byte[4];
                reader.Read(triangle_flags, 0, 4);
                TriangleFlags = BitConverter.ToInt32(triangle_flags, 0);

                bool bHaveGroups = (TriangleFlags & PackedMesh_HasTriangleGroups) != 0;

                byte[] triangle_buf = new byte[nTriangles*3*4];
                reader.Read(triangle_buf, 0, triangle_buf.Count());
                this.Triangles = new int[nTriangles * 3];
                Buffer.BlockCopy(triangle_buf, 0, this.Triangles, 0, triangle_buf.Length);

                if ( bHaveGroups ) {
                    byte[] groups_buf = new byte[nTriangles*4];
                    reader.Read(groups_buf, 0, groups_buf.Count());
                    this.FaceGroups = new int[nTriangles];
                    Buffer.BlockCopy(groups_buf, 0, this.FaceGroups, 0, groups_buf.Length);
                }

             }
        }

        bool IsBitSet(byte b, int pos) {
            return (b & (1 << pos)) != 0;
        }


        // [RMS] this is convenience stuff...
        public void Translate(float tx, float ty, float tz)
        {
            int c = VertexCount;
            for (int i = 0; i < c; ++i) {
                this.Vertices[3 * i] += tx;
                this.Vertices[3 * i + 1] += ty;
                this.Vertices[3 * i + 2] += tz;
            }
        }
        public void Scale(float sx, float sy, float sz)
        {
            int c = VertexCount;
            for (int i = 0; i < c; ++i)
            {
                this.Vertices[3 * i] *= sx;
                this.Vertices[3 * i + 1] *= sy;
                this.Vertices[3 * i + 2] *= sz;
            }
        }
        public void Scale(float s)
        {
            Scale(s, s, s);
        }

    }

}
