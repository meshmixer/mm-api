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

def crossv3(a,b):
    """cross product of 3-vectors a and b"""
    return (a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0])
def normalizev3(a):
    """return normalized 3-vector"""
    n = math.sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2])
    return (a[0]/n, a[1]/n, a[2]/n)

def lenv3(a):
    """compute length of 3-vector"""
    return math.sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2])
def sqrlenv3(a):
    """compute squared length of 3-vector"""
    return a[0]*a[0] + a[1]*a[1] + a[2]*a[2]

def float_epsilon():
    """smallest difference between numbers for 32-bit floats"""
    return 0.000000119


def deg_to_rad(angle):
    """convert angle from degrees to radians"""
    return angle * math.pi / 180.0
def rad_to_deg(angle):
    """convert angle from radians to degrees"""
    return angle * 180.0 / math.pi


def identity_matrix():
    """return identity matrix as 9-element tuple!"""
    return (1,0,0, 0,1,0, 0,0,1)

def transpose(matrix):
    """return the transpose of a matrix stored as a 9-element tuple"""
    return (matrix[0], matrix[3], matrix[6],
            matrix[1], matrix[4], matrix[7],
            matrix[2], matrix[5], matrix[8])

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

def make_perp_vectors(a):
    """construct two perpendicular vectors to input vector, return as a pair of 3-tuples"""
    if math.fabs(a[0]) > math.fabs(a[1]) & math.fabs(a[0]) > math.fabs(a[2]) :
        out1 = (-a[1], a[0], 0.0)
    else:
        out1 = (0,a[2],-a[1])
    out1 = normalizev3(out1)
    out2 = crossv3(a, out1)
    return (out1, out2)

def make_axis_angle_matrix(axis, angle):
    """construct a matrix that rotates around axis by angle (in radians)"""
    #[RMS] ported from WildMagic4
    fCos = math.cos(angle)
    fSin = math.sin(angle)
    fX2 = axis[0]*axis[0]
    fY2 = axis[1]*axis[1]
    fZ2 = axis[2]*axis[2]
    fXYM = axis[0]*axis[1]*(1-fCos)
    fXZM = axis[0]*axis[2]*(1-fCos)
    fYZM = axis[1]*axis[2]*(1-fCos)
    fXSin = axis[0]*fSin
    fYSin = axis[1]*fSin
    fZSin = axis[2]*fSin
    return ( fX2*(1-fCos)+fCos, fXYM-fZSin, fXZM+fYSin, fXYM+fZSin, fY2*(1-fCos)+fCos, fYZM-fXSin, fXZM-fYSin, fYZM+fXSin, fZ2*(1-fCos)+fCos )

def make_align_axis_matrix(initialAxis, alignWithAxis):
    """construct a matrix that rotates initialAxis to alignWithAxis. Assumes axes are normalized"""
    axisDot = dotv3(alignWithAxis, initialAxis)     # cos(angle between vectors)
    axisCross = crossv3(initialAxis, alignWithAxis) # rotation axis
    if ( sqrlenv3(axisCross) > float_epsilon() ):
        axisCross = normalizev3(axisCross)
        fAngle = math.acos(axisDot)
        return make_axis_angle_matrix(axisCross, fAngle)
    elif ( axisDot < 0 ):
        (perp1,perp2) = make_perp_vectors(initialAxis)
        return make_axis_angle_matrix(perp1, 3.14159265)
    else:
        return identity_matrix()