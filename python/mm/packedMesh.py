from mm.mm_math import *
import struct 


# [RMS] these are the supported vertex flag bits in MM right now, 
#   in the order they have to appear after the vertex buffer
#   (not sure how to construct a bitmask in python, though)
#
#define PackedLiveMesh_HasVertexNormals    (1<<2)
#   3 floats per vertex
#
#define PackedLiveMesh_HasVertexColorsRGB  (1<<3)
#   3 floats per vertex
#
#define PackedLiveMesh_HasVertexUVs        (1<<4)
#   2 floats per vertex
#
# And these are the face flag bits
#
#define PackedLiveMesh_HasTriangleGroups   (1<<2)
#   1 32-bit uint per triangle

class packedMesh(object):
    """
    This class represents a packed mesh
    """

    def __init__(self):
        """initialize empty mesh"""
        self.vertices = []
        self.triangles = []
        self.normals = []
        self.colors = []

    def appendVertex(self, pos, normal=(), color=() ):
        vID = len(self.vertices)
        self.vertices.append(pos)
        if len(normal) > 0:
            self.normals.append(normal)
        if len(color) > 0:
            self.colors.append(color)
        return vID

    def appendTriangle(self, tri):
        tID = len(self.triangles)
        self.triangles.append(tri)
        return tID;

    def writeOBJ(self, path):
        with open(path, 'w') as f:
            for v in self.vertices:
                f.write('v %f %f %f\n' %v)
            for t in self.triangles:
                t2 = tuple(addvs(t,1))
                f.write( "f %d %d  %d " %t2 )
        f.close()

    def write(self, path):
        with open(path, 'wb') as f:
            have_colors = False
            if len(self.colors) > 0:
                have_colors = True
            have_normals = False
            if len(self.normals) == len(self.vertices):
                have_normals = True

            # write version header
            version = 1
            f.write( struct.pack("i", version) )
            # write vertex count
            f.write( struct.pack("i", len(self.vertices) ) )

            # write vertex flags - currently don't have any
            vertex_flags = 0
            if have_normals:
                vertex_flags = vertex_flags | (1<<2)
            if have_colors:
                vertex_flags = vertex_flags | (1<<3)
            f.write( struct.pack("i", vertex_flags) )

            # write vertex data
            for v in self.vertices:
                f.write( struct.pack("f", v[0]) )
                f.write( struct.pack("f", v[1]) )
                f.write( struct.pack("f", v[2]) )
                
            if have_normals:
                for n in self.normals:
                    f.write( struct.pack("f", n[0]) )
                    f.write( struct.pack("f", n[1]) )
                    f.write( struct.pack("f", n[2]) )

            if have_colors:
                for c in self.colors:
                    f.write( struct.pack("f", c[0]) )
                    f.write( struct.pack("f", c[1]) )
                    f.write( struct.pack("f", c[2]) )


            # write triangle count
            f.write( struct.pack("i", len(self.triangles) ) )
            # write triangle flags - currently don't have any
            f.write( struct.pack("i", 0) )
            # write triangle data
            for t in self.triangles:
                f.write( struct.pack("i", t[0]) )
                f.write( struct.pack("i", t[1]) )
                f.write( struct.pack("i", t[2]) )
        f.close()
