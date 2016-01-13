// LiveMeshDemo.cpp : Defines the entry point for the console application.
//

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <cassert>


#include <StoredCommands.h>
#include <LiveMesh.h>
#include "RemoteControl.h"


/*
 * [RMS] This demo creates a packed mesh file on disk, and then uses the meshmixer API
 *   to load it from the disk file. This is significantly faster than parsing any kind
 *   of more complex mesh file format.
 */
int _tmain(int argc, _TCHAR* argv[])
{
	/*
	 * Create a packed binary mesh (single triangle) and write to disk
	 */

	const char * pWriteBinFilePath = "c:\\scratch\\test_mesh.bin";

	float pVertices[] = {0,0,0,  25,0,25,  25,0,0};
	float pColors[] = {1,0,1,  0,1,1,   1,0,1};
	unsigned int pTriangles[] = {0,1,2};

	mm::PackedLiveMesh mesh;
	mesh.nVertices = 3;
	mesh.pVertices = new float[mesh.nVertices*3];
	for ( unsigned int k = 0; k < mesh.nVertices*3; ++k )
		mesh.pVertices[k] = pVertices[k];

	// optionally include colors
	mesh.nFlagsV |= PackedLiveMesh_HasVertexColorsRGB;
	mesh.pColors = new float[mesh.nVertices*3];
	for ( unsigned int k = 0; k < mesh.nVertices*3; ++k )
		mesh.pColors[k] = pColors[k];

	mesh.nTriangles = 1;
	mesh.pTriangles = new unsigned int[3*mesh.nTriangles];
	for ( unsigned int k = 0; k < mesh.nTriangles*3; ++k )
		mesh.pTriangles[k] = pTriangles[k];

	// write to disk
	mm::LiveMeshUtil::WritePackedLiveMesh(pWriteBinFilePath, mesh);


	/*
	 * open API connection to meshmixer and tell it to open this file
	 */

	mm::RemoteControl remote;
	bool bOK = remote.Connect(0xAFCF, 0xAFDF);		// these are the standard MM ports
	assert(bOK);

	mm::StoredCommands sc;
	mm::StoredCommands::Key k = sc.AppendSceneCommand_AppendPackedMeshFile(pWriteBinFilePath);
	bOK = remote.Send(&sc);
	assert(bOK);

	int nObjectID = -1;
	bOK = sc.GetSceneCommandResult_AppendPackedMeshFile(k, nObjectID);
	assert(bOK);

	remote.Disconnect();


}

