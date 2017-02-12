using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using g3;

namespace mm
{
    public static class g3Extensions
    {
        public static Vector3f ToVector3f(floatArray f)
        {
            return new Vector3f(f.getitem(0), f.getitem(1), f.getitem(2));
        }
        public static Vector3f ToVector3f(vec3f f)
        {
            return new Vector3f(f.x, f.y, f.z);
        }

        public static void Set(this Vector3f v, floatArray f) {
            v.x = f.getitem(0); v.y = f.getitem(1); v.z = f.getitem(2);
        }


        public static floatArray toFloatArray(this Vector3f v)
        {
            floatArray f = new floatArray(3);
            f.setitem(0, v[0]); f.setitem(1, v[1]); f.setitem(2, v[2]);
            return f;
        }
        public static vec3f toVec3f(this Vector3f v)
        {
            vec3f vec = new vec3f();
            vec.x = v[0]; vec.y = v[1]; vec.z = v[2];
            return vec;
        }


        public static Index3i ToIndex3i(vec3i ijk)
        {
            return new Index3i(ijk.i, ijk.j, ijk.k);
        }
    }
}
