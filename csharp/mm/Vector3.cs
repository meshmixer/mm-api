using System;
using System.Collections.Generic;
using System.Text;

namespace mm
{
    public class Vector3
    {
        public float[] v = { 0, 0, 0 };

        public Vector3() {}
        public Vector3(float f) { v[0] = v[1] = v[2] = f; }
        public Vector3(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }
        public Vector3(float[] v2) { v[0] = v2[0]; v[1] = v2[1]; v[2] = v2[2]; }
        public Vector3(floatArray f) { v[0] = f.getitem(0); v[1] = f.getitem(1); v[2] = f.getitem(2); }
        public Vector3(vec3f f) { v[0] = f.x; v[1] = f.y; v[2] = f.z; }
        public Vector3(Vector3 v2) { v[0] = v2[0]; v[1] = v2[1]; v[2] = v2[2]; }

        public void Set(floatArray f) { v[0] = f.getitem(0); v[1] = f.getitem(1); v[2] = f.getitem(2); }

        public void Add(Vector3 v2) { v[0] += v2[0]; v[1] += v2[1]; v[2] += v2[2]; }
        public void Subtract(Vector3 v2) { v[0] -= v2[0]; v[1] -= v2[1]; v[2] -= v2[2]; }
        public void Multiply(float f) { v[0] *= f;  v[1] *= f;  v[2] *= f; }

        public Vector3 Sum(Vector3 v2) { return new Vector3(v[0] + v2[0], v[1] + v2[1], v[2] + v2[2]); }
        public Vector3 Product(float f) { return new Vector3(f*v[0], f*v[1], f*v[2]); }

        public float LengthSqr()
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }
        public float Length()
        {
            return (float)Math.Sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

        public float DistanceSqr(Vector3 v2)
        {
            return (v[0] - v2.v[0]) * (v[0] - v2.v[0]) +
                (v[1] - v2.v[1]) * (v[1] - v2.v[1]) +
                (v[2] - v2.v[2]) * (v[2] - v2.v[2]);
        }
        public float Distance(Vector3 v2)
        {
            return (float)Math.Sqrt(DistanceSqr(v2));
        }

        static public Vector3 Lerp(Vector3 v0, Vector3 v1, float t)
        {
            return new Vector3((1 - t) * v0.v[0] + t * v1.v[0], (1 - t) * v0.v[1] + t * v1.v[1], (1 - t) * v0.v[2] + t * v1.v[2]);
        }

        public float this[int key]
        {
            get { return v[key]; }
            set { v[key] = value; }
        }

        public floatArray toFloatArray()
        {
            floatArray f = new floatArray(3);
            f.setitem(0, v[0]); f.setitem(1, v[1]); f.setitem(2, v[2]);
            return f;
        }
        public vec3f toVec3f()
        {
            vec3f vec = new vec3f();
            vec.x = v[0]; vec.y = v[1]; vec.z = v[2];
            return vec;
        }

        static public readonly Vector3 AxisX = new Vector3(1, 0, 0);
        static public readonly Vector3 AxisY = new Vector3(0, 1, 0);
        static public readonly Vector3 AxisZ = new Vector3(0, 0, 1);
        static public readonly Vector3 Zero = new Vector3(0, 0, 0);
        static public readonly Vector3 Invalid = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
    }



    public class Frame3
    {
        public Vector3 origin;
        public Vector3 x, y, z;

        public Frame3()
        {
            origin = new Vector3(0,0,0);
            x = new Vector3(1, 0, 0);
            y = new Vector3(0, 1, 0);
            z = new Vector3(0, 0, 1);
        }

        public Frame3(Vector3 o)
        {
            origin = new Vector3(o);
            x = new Vector3(1, 0, 0);
            y = new Vector3(0, 1, 0);
            z = new Vector3(0, 0, 1);
        }

        public Frame3(frame3f f)
        {
            origin = new Vector3(f.origin_x, f.origin_y, f.origin_z);
            x = new Vector3(f.tan1_x, f.tan1_y, f.tan1_z);
            y = new Vector3(f.tan2_x, f.tan2_y, f.tan2_z);
            z = new Vector3(f.normal_x, f.normal_y, f.normal_z);
        }

        public frame3f toFrame3f()
        {
            frame3f f = new frame3f();
            f.origin_x = origin[0]; f.origin_y = origin[1]; f.origin_z = origin[2];
            f.tan1_x = x[0]; f.tan1_y = x[1]; f.tan1_z = x[2];
            f.tan2_x = y[0]; f.tan2_y = y[1]; f.tan2_z = y[2];
            f.normal_x = z[0]; f.normal_y = z[1]; f.normal_z = z[2];
            return f;
        }
    }
}
