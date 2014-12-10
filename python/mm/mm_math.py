import math

def addvs(vec,s):
    """add scalar to elements of vector"""
    return map(lambda x: x+s, vec)

def subvs(vec,s):
    """subtract scalar from elements of vector"""
    return map(lambda x: x-s, vec) 

def mulvs(vec,s):
    """multiply elements of vector by scalar"""
    return map(lambda x: x*s, vec) 
def divvs(vec,s):
    """divide elements of vector by scalar"""
    return map(lambda x: x/s, vec) 

def addv3s(vec, scalar):
    """add scalar to elements of 3-vector"""
    return (vec[0]+scalar, vec[1]+scalar, vec[2]+scalar)
def subv3s(vec, scalar):
    """subtract scalar from elements of 3-vector"""
    return (vec[0]-scalar, vec[1]-scalar, vec[2]-scalar)
def mulv3s(vec, scalar):
    """multiply elements of 3-vector by scalar"""
    return (vec[0]*scalar, vec[1]*scalar, vec[2]*scalar)
def divv3s(vec, scalar):
    """divide elements of 3-vector by scalar"""
    return (vec[0]/scalar, vec[1]/scalar, vec[2]/scalar)

def negv3(a):
    """negate 3-vector"""
    return (-a[0], -a[1], -a[2])
def subv3(a, b):
    """subtract 3-vector b from a"""
    return (a[0]-b[0], a[1]-b[1], a[2]-b[2])
def addv3(a, b):
    """add 3-vector b to a"""
    return (a[0]+b[0], a[1]+b[1], a[2]+b[2])
def dotv3(a,b):
    """dot product of 3-vectors a and b"""
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
def lerpv3(a,b,t):
    """linear interplation (1-t)*a + (t)*b"""
    return ( (1-t)*a[0] + (t)*b[0],  (1-t)*a[1] + (t)*b[1], (1-t)*a[2] + (t)*b[2] )


def deg_to_rad(angle):
    """convert angle from degrees to radians"""
    return angle * math.pi / 180.0
def rad_to_deg(angle):
    """convert angle from radians to degrees"""
    return angle * 180.0 / math.pi


def make_rotX_matrix(angle):
    """construct matrix that rotates around X axis by angle (in radians)"""
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( 1, 0, 0, 0, c0, -s0, 0, s0, c0 )
def make_rotY_matrix(angle):
    """construct matrix that rotates around Y axis by angle (in radians)"""
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( c0, 0, s0, 0, 1, 0, -s0, 0, c0 )
def make_rotZ_matrix(angle):
    """construct matrix that rotates around Z axis by angle (in radians)"""
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( c0, -s0, 0, s0, c0, 0, 0, 0, 1 )

def make_matrix_from_axes(x,y,z):
    """construct orientation matrix from basis vectors"""
    return ( x[0], y[0], z[0],
             x[1], y[1], z[1],
             x[2], y[2], z[2] )

def transpose(matrix):
    """return the transpose of a matrix stored as a 9-element tuple"""
    return (matrix[0], matrix[3], matrix[6],
            matrix[1], matrix[4], matrix[7],
            matrix[2], matrix[5], matrix[8])