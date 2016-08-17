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

        public void Set(floatArray f) { v[0] = f.getitem(0); v[1] = f.getitem(1); v[2] = f.getitem(2); }


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

        public vec3f toVec3()
        {
            vec3f f = new vec3f();
            f.x = v[0]; f.y = v[1]; f.z = v[2];
            return f;
        }
    }
}
