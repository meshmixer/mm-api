import mmapi

class mmFrame(object):
    """
    This class represents a 3D frame with an origin (.origin)
    and .x/.y/.z ortho-normal axes. 
    """

    def __init__(self):
        """initialize to origin and identity matrix"""
        self.origin = (0,0,0)
        self.x = (1,0,0)
        self.y = (0,1,0)
        self.z = (0,0,1)

    def set(self, o, x, y, z):
        """initialize with origin and X/Y/Z axes"""
        self.origin = (o[0],o[1],o[2]);
        self.x = (x[0],x[1],x[2]);
        self.y = (y[0],y[1],y[2]);
        self.z = (z[0],z[1],z[2]);

    def set_frame3f(self, f):
        """initialize with wrapped C++ frame3f object, see StoredCommands.h"""
        self.origin = (f.origin_x, f.origin_y, f.origin_z);
        self.x = (f.tan1_x, f.tan1_y, f.tan1_z);
        self.y = (f.tan2_x, f.tan2_y, f.tan2_z);
        self.z = (f.normal_x, f.normal_y, f.normal_z);

    def get_frame3f(self):
        """return mmapi.frame3f version of this frame"""
        f = mmapi.frame3f()
        f.origin_x = self.origin[0]
        f.origin_y = self.origin[1]
        f.origin_z = self.origin[2]
        f.tan1_x = self.x[0]
        f.tan1_y = self.x[1]
        f.tan1_z = self.x[2]
        f.tan2_x = self.y[0]
        f.tan2_y = self.y[1]
        f.tan2_z = self.y[2]
        f.normal_x = self.z[0]
        f.normal_y = self.z[1]
        f.normal_z = self.z[2]
        return f

    def get_matrix(self):
        """return orientation as 9-element tuple"""
        return (self.x[0], self.x[1], self.x[2], self.y[0],self.y[1],self.y[2], self.z[0],self.z[1],self.z[2])

