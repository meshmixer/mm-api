using System;
using System.Collections.Generic;
using System.Text;

namespace mm
{
    public class mmMatrix
    {
        public float[] m = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

        public mmMatrix() {}
        public mmMatrix(float[] m2) { m2.CopyTo(m, 0); }
        public mmMatrix(mmMatrix v2) { v2.m.CopyTo(m, 0); }
        public mmMatrix(floatArray f) { for (int k = 0; k < 9; ++k) m[k] = f.getitem(k); }
        public mmMatrix(mmFrame f)
        {
            m[0] = f.x[0]; m[1] = f.x[1]; m[2] = f.x[2];
            m[3] = f.y[0]; m[4] = f.y[1]; m[5] = f.y[2];
            m[6] = f.z[0]; m[7] = f.z[1]; m[8] = f.z[2];
        }


        public float this[int key]
        {
            get { return m[key]; }
            set { m[key] = value; }
        }

        public floatArray toFloatArray()
        {
            floatArray f = new floatArray(9);
            for (int k = 0; k < 9; ++k)
                f.setitem(k, m[k]);
            return f;
        }

        public void ToTranspose()
        {
            float tmp = m[3]; m[3] = m[1]; m[1] = tmp;
            tmp = m[2]; m[2] = m[6]; m[6] = tmp;
            tmp = m[5]; m[5] = m[7]; m[7] = tmp;
        }
        public mmMatrix Transpose()
        {
            mmMatrix t = new mmMatrix(this);
            t.ToTranspose();
            return t;
        }

    }

}
