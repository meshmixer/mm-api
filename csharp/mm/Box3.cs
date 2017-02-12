using System;
using System.Collections.Generic;
using System.Text;

namespace mm
{
    public class Box3
    {
        public Vector3 Min;
        public Vector3 Max;

        public Box3() { SetInvalid(); }
        public Box3(Vector3 a, Vector3 b) { Min = new Vector3(a); Max = new Vector3(b); }

        public void SetInvalid()
        {
            Min = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
            Max = new Vector3(float.MinValue, float.MinValue, float.MinValue);
        }

        public Vector3 Center()
        {
            return new Vector3(0.5f * (Min[0] + Max[0]), 0.5f * (Min[1] + Max[1]), 0.5f * (Min[2] + Max[2]));
        }

        public Vector3 Diagonal()
        {
            return new Vector3((Max[0] - Min[0]), (Max[1] - Min[1]), (Max[2] - Min[2]));
        }

    }
}
