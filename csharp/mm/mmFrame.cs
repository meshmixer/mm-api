using System;
using System.Collections.Generic;
using System.Text;
using g3;

namespace mm
{

    public class mmFrame
    {
        public Vector3f origin;
        public Vector3f x, y, z;

        public mmFrame()
        {
            origin = new Vector3f(0,0,0);
            x = new Vector3f(1, 0, 0);
            y = new Vector3f(0, 1, 0);
            z = new Vector3f(0, 0, 1);
        }

        public mmFrame(Vector3f o)
        {
            origin = new Vector3f(o);
            x = new Vector3f(1, 0, 0);
            y = new Vector3f(0, 1, 0);
            z = new Vector3f(0, 0, 1);
        }

        public mmFrame(frame3f f)
        {
            origin = new Vector3f(f.origin_x, f.origin_y, f.origin_z);
            x = new Vector3f(f.tan1_x, f.tan1_y, f.tan1_z);
            y = new Vector3f(f.tan2_x, f.tan2_y, f.tan2_z);
            z = new Vector3f(f.normal_x, f.normal_y, f.normal_z);
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
