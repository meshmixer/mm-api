import math

def addvs(vec,s):
    return map(lambda x: x+s, vec) 
def subvs(vec,s):
    return map(lambda x: x-s, vec) 
def mulvs(vec,s):
    return map(lambda x: x*s, vec) 
def divvs(vec,s):
    return map(lambda x: x/s, vec) 

def addv3s(vec, scalar):
    return (vec[0]+scalar, vec[1]+scalar, vec[2]+scalar)
def subv3s(vec, scalar):
    return (vec[0]-scalar, vec[1]-scalar, vec[2]-scalar)
def mulv3s(vec, scalar):
    return (vec[0]*scalar, vec[1]*scalar, vec[2]*scalar)
def divv3s(vec, scalar):
    return (vec[0]/scalar, vec[1]/scalar, vec[2]/scalar)

def negv3(a):
    return (-a[0], -a[1], -a[2])
def subv3(a, b):
    return (a[0]-b[0], a[1]-b[1], a[2]-b[2])
def addv3(a, b):
    return (a[0]+b[0], a[1]+b[1], a[2]+b[2])
def dotv3(a,b):
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
def lerpv3(a,b,t):
    return ( (1-t)*a[0] + (t)*b[0],  (1-t)*a[1] + (t)*b[1], (1-t)*a[2] + (t)*b[2] )


def deg_to_rad(angle):
    return angle * math.pi / 180.0
def rad_to_deg(angle):
    return angle * 180.0 / math.pi


def make_rotX_matrix(angle):
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( 1, 0, 0, 0, c0, -s0, 0, s0, c0 )
def make_rotY_matrix(angle):
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( c0, 0, s0, 0, 1, 0, -s0, 0, c0 )
def make_rotZ_matrix(angle):
    c0 = math.cos(angle)
    s0 = math.sin(angle)
    return ( c0, -s0, 0, s0, c0, 0, 0, 0, 1 )

def make_matrix_from_axes(x,y,z):
    return ( x[0], y[0], z[0],
             x[1], y[1], z[1],
             x[2], y[2], z[2] )

def transpose(matrix):
    return (matrix[0], matrix[3], matrix[6],
            matrix[1], matrix[4], matrix[7],
            matrix[2], matrix[5], matrix[8])