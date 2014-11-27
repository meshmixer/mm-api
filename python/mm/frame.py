import mmapi

class mmFrame(object):
    """represents 3D frame with origin and 3 axes"""

    def __init__(self):
        self.origin = (0,0,0)
        self.x = (1,0,0)
        self.y = (0,1,0)
        self.z = (0,0,1)

    def set(self, o, x, y, z):
        self.origin = (o[0],o[1],o[2]);
        self.x = (x[0],x[1],x[2]);
        self.y = (y[0],y[1],y[2]);
        self.z = (z[0],z[1],z[2]);

    def setFromMM(self, f):
        self.origin = (f.origin_x, f.origin_y, f.origin_z);
        self.x = (f.tan1_x, f.tan1_y, f.tan1_z);
        self.y = (f.tan2_x, f.tan2_y, f.tan2_z);
        self.z = (f.normal_x, f.normal_y, f.normal_z);

    def get_matrix(self):
        return (self.x[0], self.x[1], self.x[2], self.y[0],self.y[1],self.y[2], self.z[0],self.z[1],self.z[2])