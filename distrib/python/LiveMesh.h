#ifndef __MM_LIVE_MESH_H__
#define __MM_LIVE_MESH_H__


// [RMS] You must #define USING_MM_COMMAND_API to use this outside of meshmixer-desktop
//#define USING_MM_COMMAND_API

#include <fstream>
#include <string>

#define PackedLiveMesh_HasVertexNormals    (1<<2)
#define PackedLiveMesh_HasVertexColorsRGB  (1<<3)
#define PackedLiveMesh_HasVertexUVs        (1<<4)
#define PackedLiveMesh_HasTriangleGroups   (1<<2)

namespace rms {
	class VFTriangleMesh;
}

namespace mm {

class PackedLiveMesh
{
public:
	unsigned int nVersion;
	unsigned int nVertices;
	unsigned int nFlagsV;
	float * pVertices;
	float * pNormals;
	float * pColors;
	float * pUVs;

	unsigned int nTriangles;
	unsigned int nFlagsT;
	unsigned int * pTriangles;
	unsigned int * pGroups;

	PackedLiveMesh() { 
		nVersion = 1;
		nVertices = 0;
		nFlagsV = 0;
		pVertices = NULL;
		pNormals = NULL;
		pColors = NULL;
		pUVs = NULL;
		nFlagsT = 0;
		pTriangles = NULL;
		pGroups = NULL;
	};
	void Clear() { 
		if ( pVertices )	{ delete [] pVertices; pVertices = NULL; }
		if ( pNormals )		{ delete [] pNormals; pNormals = NULL; }
		if ( pColors )		{ delete [] pColors; pColors = NULL; }
		if ( pUVs ) 		{ delete [] pUVs; pUVs = NULL; }
		if ( pTriangles )	{ delete [] pTriangles; pTriangles = NULL; }
		if ( pGroups )		{ delete [] pGroups; pGroups = NULL; }
	}
	~PackedLiveMesh() {
		Clear();
	}
};

#ifndef USING_MM_COMMAND_API
typedef boost::shared_ptr<PackedLiveMesh> PackedLiveMeshPtr;
#endif


class LiveMeshUtil
{
public:
#ifndef USING_MM_COMMAND_API
	static void VFMeshToPackedMesh(const rms::VFTriangleMesh * pMeshIn, PackedLiveMesh & p, const void * pXForm = NULL );
	static void PackedMeshToVFMesh(const PackedLiveMesh & p, rms::VFTriangleMesh * pMesh, const void * pXForm = NULL);
#endif

	static bool WritePackedLiveMesh(std::ofstream & o, const PackedLiveMesh & m);
	static bool WritePackedLiveMesh(const char * pFilename, const PackedLiveMesh & m);
	static bool WritePackedLiveMesh(const std::string & filename, const PackedLiveMesh & m) { return WritePackedLiveMesh(filename.c_str(), m); }

	static bool ReadPackedLiveMesh(std::ifstream & in, PackedLiveMesh & m);
	static bool ReadPackedLiveMesh(const char * pFilename, PackedLiveMesh & m);
	static bool ReadPackedLiveMesh(const std::string & filename, PackedLiveMesh & m) { return ReadPackedLiveMesh(filename.c_str(), m); }
};



}  // end namespace mm


#endif // __MM_LIVE_MESH_H__
