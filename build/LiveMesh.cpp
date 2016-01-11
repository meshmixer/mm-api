#include "stdafx.h"
#include "LiveMesh.h"

#include <VFTriangleMesh.h>
#include <MeshUtils.h>

using namespace rms;
using namespace mm;


#ifndef USING_MM_COMMAND_API

void LiveMeshUtil::VFMeshToPackedMesh(const VFTriangleMesh * pMeshIn, PackedLiveMesh & p )
{
	VFTriangleMesh m(*pMeshIn);
	VertexMap mapV; TriangleMap mapT;
	m.Compact(mapV, mapT);

	p.nVertices = m.GetMaxVertexID();
	p.pVertices = new float[p.nVertices*3];
	p.pNormals = new float[p.nVertices*3];
	p.pColors = new float[p.nVertices*3];
	p.pUVs = NULL;
	p.nFlagsV = PackedLiveMesh_HasVertexNormals | PackedLiveMesh_HasVertexColorsRGB;
	for ( unsigned int k = 0; k < p.nVertices; ++k ) {
		const Wml::Vector3f & v = m.GetVertex(k);
		const Wml::Vector3f & n = m.GetNormal(k);
		const Wml::ColorRGBA & c = m.GetColor(k);
		for ( int j = 0; j < 3; ++j ) {
			p.pVertices[3*k+j] = v[j];
			p.pNormals[3*k+j] = n[j];
			p.pColors[3*k+j] = c[j];
		}
	}

	p.nTriangles = m.GetMaxTriangleID();
	p.pTriangles = new unsigned int[p.nTriangles*3];
	p.pGroups = new unsigned int[p.nTriangles];
	p.nFlagsT = PackedLiveMesh_HasTriangleGroups;
	for ( unsigned int k = 0; k < p.nTriangles; ++k ) {
		const VertexID * pTri = m.GetTriangle(k);
		p.pTriangles[3*k] = pTri[0]; 
		p.pTriangles[3*k+1] = pTri[1];
		p.pTriangles[3*k+2] = pTri[2];
		p.pGroups[k] = m.GetGroupID(k);
	}
}



void LiveMeshUtil::PackedMeshToVFMesh(const PackedLiveMesh & p, VFTriangleMesh * pMesh)
{
	VFTriangleMesh & m = *pMesh;

	if ( p.pVertices == NULL )
		return;

	//lgDevAssert(p.pVertices != NULL);
	for ( unsigned int k = 0; k < p.nVertices; ++k ) {
		Wml::Vector3f v(p.pVertices[3*k], p.pVertices[3*k+1], p.pVertices[3*k+2]);
		VertexID vID = m.AppendVertex(v);
		lgDevAssert(vID == k);
	}

	if ( (p.nFlagsV & PackedLiveMesh_HasVertexNormals) != 0 ) {
		lgDevAssert(p.pNormals != NULL);
		for ( unsigned int k = 0; k < p.nVertices; ++k ) {
			Wml::Vector3f n(p.pNormals[3*k], p.pNormals[3*k+1], p.pNormals[3*k+2]);
			m.SetNormal(k, n);
		}
	} else
		MeshUtils::EstimateNormals(m);

	if ( (p.nFlagsV & PackedLiveMesh_HasVertexColorsRGB) != 0 ) {
		lgDevAssert(p.pColors != NULL);
		for ( unsigned int k = 0; k < p.nVertices; ++k ) {
			Wml::ColorRGBA c(p.pColors[3*k], p.pColors[3*k+1], p.pColors[3*k+2], 1.0f);
			m.SetColor(k, c);
		}
	}
	if ( (p.nFlagsV & PackedLiveMesh_HasVertexUVs) != 0 ) {
		lgDevAssert(p.pUVs != NULL);
		for ( unsigned int k = 0; k < p.nVertices; ++k ) {
			Wml::Vector2f uv(p.pUVs[2*k], p.pUVs[2*k+1]);
			m.SetVertexUV(k, uv);
		}
	}


	lgDevAssert(p.pTriangles != NULL);
	for ( unsigned int k = 0; k < p.nTriangles; ++k ) {
		//_RMSInfo("APPEND %d %d %d\n", p.pTriangles[3*k], p.pTriangles[3*k+1], p.pTriangles[3*k+2]);
		TriangleID tID = m.AppendTriangle(p.pTriangles[3*k], p.pTriangles[3*k+1], p.pTriangles[3*k+2]);
		lgDevAssert(tID == k);
	}
	if ( (p.nFlagsT & PackedLiveMesh_HasTriangleGroups) != 0 ) {
		lgDevAssert(p.pGroups != NULL);
		for ( unsigned int k = 0; k < p.nTriangles; ++k ) {
			m.SetGroupID(k, p.pGroups[k]);
		}
	}

}


#endif



bool LiveMeshUtil::WritePackedLiveMesh(const char * pFilename, const PackedLiveMesh & m)
{
	boost::filesystem::ofstream out(pFilename, std::ios_base::binary);
	if (! out )
		return false;
	bool bOK = WritePackedLiveMesh(out, m);
	out.flush();
	out.close();
	return bOK;
}
bool LiveMeshUtil::WritePackedLiveMesh(boost::filesystem::ofstream & o, const PackedLiveMesh & m)
{
	o.write((char *)&m.nVersion, sizeof(unsigned int));
	o.write((char *)&m.nVertices, sizeof(unsigned int));
	o.write((char *)&m.nFlagsV, sizeof(unsigned int));

	lgDevAssert(m.pVertices != NULL);
	o.write( (char *)m.pVertices, 3*m.nVertices*sizeof(float) );

	if ( (m.nFlagsV & PackedLiveMesh_HasVertexNormals) != 0 ) {
		lgDevAssert(m.pNormals != NULL);
		o.write( (char *)m.pNormals, 3*m.nVertices*sizeof(float) );
	}
	if ( (m.nFlagsV & PackedLiveMesh_HasVertexColorsRGB) != 0 ) {
		lgDevAssert(m.pColors);
		o.write( (char *)m.pColors, 3*m.nVertices*sizeof(float) );
	}
	if ( (m.nFlagsV & PackedLiveMesh_HasVertexUVs) != 0 ) {
		lgDevAssert(m.pUVs);
		o.write( (char *)m.pUVs, 2*m.nVertices*sizeof(float) );
	}

	o.write((char *)&m.nTriangles, sizeof(unsigned int));
	o.write((char *)&m.nFlagsT, sizeof(unsigned int));

	lgDevAssert(m.pTriangles != NULL);
	o.write( (char *)m.pTriangles, 3*m.nTriangles*sizeof(unsigned int) );

	if ( (m.nFlagsT & PackedLiveMesh_HasTriangleGroups) != 0 ) {
		lgDevAssert(m.pGroups != NULL);
		o.write( (char *)m.pGroups, m.nTriangles*sizeof(unsigned int) );
	}

	return true;
}



bool LiveMeshUtil::ReadPackedLiveMesh(const char * pFilename, PackedLiveMesh & m)
{
	boost::filesystem::ifstream in(pFilename, std::ios_base::binary);
	if (! in )
		return false;
	bool bOK = ReadPackedLiveMesh(in, m);
	in.close();
	return bOK;
}
bool LiveMeshUtil::ReadPackedLiveMesh(boost::filesystem::ifstream & in, PackedLiveMesh & m)
{
	if ( in.eof() )	return false;

	m.Clear();

	in.read( (char *)&m.nVersion, sizeof(unsigned int));
	_RMSInfo("[ReadPackedLiveMesh] version %d\n", m.nVersion);
	
	m.nVertices = 0;
	in.read( (char *)&m.nVertices, sizeof(unsigned int));
	if ( m.nVertices == 0 )
		return false;
	_RMSInfo("[ReadPackedLiveMesh] header says %d vertices, flags %x...\n", m.nVertices, m.nFlagsV);

	in.read( (char *)&m.nFlagsV, sizeof(unsigned int));
	if ( in.eof() ) return false;

	m.pVertices = new float[m.nVertices*3];
	in.read( (char *)m.pVertices, m.nVertices * 3 * sizeof(float) );
	if ( in.eof() ) return false;
	
	if ( (m.nFlagsV & PackedLiveMesh_HasVertexNormals) != 0 ) {
		m.pNormals = new float[m.nVertices*3];
		in.read( (char *)m.pNormals, m.nVertices * 3 * sizeof(float) );
		if ( in.eof() ) return false;
	}
	if ( (m.nFlagsV & PackedLiveMesh_HasVertexColorsRGB) != 0 ) {
		m.pColors = new float[m.nVertices*3];
		in.read( (char *)m.pColors, m.nVertices * 3 * sizeof(float) );
		if ( in.eof() ) return false;
	}
	if ( (m.nFlagsV & PackedLiveMesh_HasVertexUVs) != 0 ) {
		m.pUVs = new float[m.nVertices*2];
		in.read( (char *)m.pColors, m.nVertices * 2 * sizeof(float) );
		if ( in.eof() ) return false;
	}

	in.read( (char *)&m.nTriangles, sizeof(unsigned int));
	in.read( (char *)&m.nFlagsT, sizeof(unsigned int));
	if ( in.eof() ) return false;

	_RMSInfo("[ReadPackedLiveMesh] header says %d triangles, flags %x...\n", m.nTriangles, m.nFlagsT);


	m.pTriangles = new unsigned int[m.nTriangles*3];
	in.read( (char *)m.pTriangles, m.nTriangles * 3 * sizeof(unsigned int) );
	if ( in.eof() ) return false;

	if ( (m.nFlagsT & PackedLiveMesh_HasTriangleGroups) != 0 ) {
		m.pGroups = new unsigned int[m.nTriangles];
		in.read( (char *)m.pGroups, m.nTriangles * sizeof(unsigned int) );
	}

	_RMSInfo("[ReadPackedLiveMesh] have %d vertices, %d triangles\n", m.nVertices, m.nTriangles);

	return true;
}

