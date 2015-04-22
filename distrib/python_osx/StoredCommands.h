/********************************************************************
 * (C) Copyright 2014 by Autodesk, Inc. All Rights Reserved. By using
 * this code,  you  are  agreeing  to the terms and conditions of the
 * License  Agreement  included  in  the documentation for this code.
 * AUTODESK  MAKES  NO  WARRANTIES,  EXPRESS  OR  IMPLIED,  AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT.  AUTODESK PROVIDES THE CODE ON AN 'AS-IS' BASIS AND EXPLICITLY
 * DISCLAIMS  ANY  LIABILITY,  INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES  FOR ERRORS, OMISSIONS, AND  OTHER  PROBLEMS IN THE  CODE.
 *
 * Use, duplication,  or disclosure by the U.S. Government is subject
 * to  restrictions  set forth  in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) as well as DFAR 252.227-7013(c)(1)(ii)
 * (Rights  in Technical Data and Computer Software),  as applicable.
 *******************************************************************/
#ifndef _MM_STORED_COMMANDS_H__
#define _MM_STORED_COMMANDS_H__


// [RMS] #define USING_MM_COMMAND_API to use this outside of meshmixer-desktop


#include <BinarySerializer.h>

namespace mm {

class MainWindow;


struct vec3f {
	float x,y,z;
};
struct mat3f {
	float data[9];
};
struct frame3f {
	float origin_x, origin_y, origin_z;
	float normal_x, normal_y, normal_z;
	float tan1_x, tan1_y, tan1_z;
	float tan2_x, tan2_y, tan2_z;
};
struct fstring {
	unsigned int nSize;
	char data[4096];
};
struct brush_stamp {
	float x,y,z;
	float fPressure;
};
struct any_result {
	int type;		// 0=float, 1=int, 2=bool, 3=vec3, 4=m
	float f;
	int i;
	bool b;
	float x,y,z;
	float m[9];
	any_result() { type = -1; f = 0; i = 0; b = false; x=y=z=0; m[0]=m[1]=m[2]=m[3]=m[4]=m[5]=m[6]=m[7]=m[8]=0; }
};

// [RMS] these are just return-type structs. we always want to use structs instead of pointers/references
//   to POD types, because SWIG will convert those pointers to arrays, and this is a mess
struct camera_info {
	float horzFOVDegrees;
	int width;
	int height;
};



class StoredCommands
{
public:
	typedef unsigned int Key;		// record-identifier returned by AppendCommand / used for GetResult



public:

	/*
	 * mouse events
	 */
	void AppendMouseDownEvent( bool bLeftDown, bool bMiddleDown, bool bRightDown, float x, float y, bool bAbsolute = true );
	void AppendMouseMoveEvent( float x, float y, bool bAbsolute );
	void AppendMouseUpEvent( bool bLeftUp, bool bMiddleUp, bool bRightUp, float x, float y, bool bAbsolute );


	/*
	 * Camera manipulation
	 */
	void CameraControl_Begin();
	void CameraControl_End();
	void CameraControl_EnableOrbitSnap();
	void CameraControl_DisableOrbitSnap();
	void CameraControl_FreeOrbit(float fAngleDegreesX, float fAngleDegreesY);
	void CameraControl_TurntableOrbit(float fAngleDegreesX, float fAngleDegreesY);
	void CameraControl_Pan(float fDistanceX, float fDistanceY);
	void CameraControl_DollyZoom(float fDistanceZ);
	void CameraControl_RecenterView();
	void CameraControl_RecenterViewAtCursor();
	void CameraControl_SetSpecificView(const vec3f & eye, const vec3f & target, const vec3f & up);

	void CameraControl_SetOrthographicView();
	void CameraControl_SetPerspectiveView();

	//! returned frame is eye=origin, direction=z, left=tan1, up=tan2
	Key CameraControl_QueryCamera();
	bool CameraControl_QueryCameraResult(Key k, frame3f & f, vec3f & target, camera_info & cam_info );

	Key CameraControl_QueryEyeRayAtPixel(int x, int y);
	bool CameraControl_QueryEyeRayAtPixelResult(Key k, vec3f & ray_origin, vec3f & ray_direction);

	// nMode  0=SmoothNormals, 1=FaceNormals, 2=GroupNormals
	void ViewControl_SetSurfaceNormalMode(int nMode);

	// nMode  0=VertexColors, 1=GroupColors, 2=ConstantColor
	void ViewControl_SetTriangleColorMode(int nMode);

	void ViewControl_SetShowWireframe(bool bShow);
	void ViewControl_SetShowBoundaries(bool bShow);
	void ViewControl_SetShowGrid(bool bShow);
	void ViewControl_SetShowPrinterBed(bool bShow);
	void ViewControl_SetTransparentTarget(bool bEnable);

	void ViewControl_SetDefaultShader();
	void ViewControl_SetXRayShader();
	void ViewControl_SetTextureShader();
	void ViewControl_SetUVShader();
	void ViewControl_SetOverhangShader();

	void ViewControl_ShowObjectBrowser();
	void ViewControl_HideObjectBrowser();

	/*
	 * INTERNAL MM TOOL API
	 */

	/*
	 * [RMS] These commands just result in calls into Qt signal handlers, IE each of these basically
	 *   mimics a GUI button-click. They may not make sense to do remotely...
	 */

	/*
	 * BeginTool command for initializing a tool. Valid tool names are:
	 *
		"select"			- start face selection tool

		"volumeBrush"		- start volume brush tool
		"surfaceBrush"		- start surface brush tool
		"stamp"				- start stamp tool

		"replace"			- Replace tool
		"erase"				- Erase/Fill tool
		"discard"			- Discard tool
		"reduce"			- Reduce tool
		"remesh"			- Remesh tool
		"extrude"			- Extrude tool
		"extract"			- Extract tool
		"offset"			- Offset tool
		"handle"			- Handle tool
		"bridge"			- Bridge tool
		"join"				- Join tool
		"weldBoundaries"	- Weld Boundaries tool
		"separate"			- Separate tool
		"planeCut"			- Plane Cut tool (with or without selection)
		"attractToTarget"	- Attract tool
		"flipNormals"		- Flip Normals tool
		"fitPrimitive"		- Fit Primitive tool

		"makePart"			- Convert (selection) To Open Part
		"makeSolidPart"		- Convert (selection) To Solid Part
		"makePolygon"		- Convert (selection) to Stamp polygon

		"smooth"			- Smooth tool
		"faceTransform"		- Transform tool
		"softTransform"		- Soft Transform tool
		"warp"				- Warp tool

		"createFaceGroup"	- Create new FaceGroup for selection
		"clearFaceGroup"	- Clear FaceGroup for selection
		"smoothBoundary"	- Smooth Boundary tool

		"mirror"				- Mirror tool
		"duplicate"				- Duplicate tool
		"transform"				- Transform tool
		"align"					- Align tool
		"closeCracks"			- Close Cracks tool
		"generateFaceGroups"	- Generate Face Groups tool
		"makeSolid"				- Make Solid tool
		"hollow"				- Hollow tool
		"makePattern"			- Make pattern tool
		"makeSlices"			- Make Slices tool
		"separateShells"		- Separate Shells tool
		"addTube"				- Add Tube tool

		"combine"				- Combine tool (multiple selected objects)
		"union"					- Boolean Union 
		"difference"			- Boolean Difference
		"intersection"			- Boolean Intersection

		"inspector"				- Inspector tool
		"units"					- Units/Dimensions tool
		"measure"				- Measure tool
		"stability"				- Stability analysis tool
		"strength"				- Strength analysis tool
		"overhangs"				- Overhangs/Support-Generation tool
		"slicing"				- Slicing analysis tool
		"thickness"				- Thickness analysis tool
		"orientation"			- Orientation optimization tool
		"layout"				- Print Bed layout/packing tool
		"deviation"				- Deviation measurement between two selected meshes
		"clearance"				- Clearance measurement between two selected meshes
	 *
	 */
	void AppendBeginToolCommand( std::string toolName );

	/*
	 * CompleteToolCommand for finishing a tool. Valid strings are:
	 *   "cancel"
	 *   "accept"
	 */
	void AppendCompleteToolCommand( std::string command );

	/*
	 * ToolParameterCommand for modifying a tool parameter. 
	   Notes:   vector3f is a 3-float list, matrix3f is a 9-float 3x3 row-major matrix

		[select]			- start face selection tool
			"size" : float 
			"brushType" : integer 
			"symmetry" : boolean 
		[volumeBrush]		- start volume brush tool
			"strength" : float  range [0,1]
			"size" : float 
			"depth" : float          range [-1,1]
			"lazyness" : float       range [0,1]
			"refine" : float         range [0,1]
			"reduce" : float         range [0,1]
			"refineSmooth" : float   range [0,1]
			"attractStrength" : float 
			"volumetric" : boolean 
			"symmetric" : boolean 
			"surfSnap" : boolean 
			"continuous" : boolean 
			"enableRefinement" : boolean 
			"restrictToGroup" : boolean 
			"holdBoundary" : boolean 
			"preserveGroups" : boolean 
			"brushOnTarget" : boolean 
			"adaptiveRefinement" : boolean 
		[surfaceBrush]		- start surface brush tool
			"restrictToGroup" : boolean 
			"holdBoundary" : boolean 
			"preserveGroups" : boolean 
		[stamp]				- start stamp tool
			"snapDimension" : boolean 

		[erase]				- Erase/Fill tool
			"density" : float 
			"smooth" : float 
			"scale" : float 
			"boundaryRotate" : float 
			"replaceType" : integer   values:  MinimalFill = 0, FlatRefinedMesh = 1, MVCPlanarDeformation = 2
		[discard]			- Discard tool
		[reduce]			- Reduce tool
			"percentage" : float 
			"maxDeviation" : float 
			"maxDeviationWorld" : float 
			"smooth" : float 
			"normalThreshold" : float 
			"goalType" : integer			Percentage = 0, TriangleBudget = 1,	MaxDeviation = 2
			"triangleCount" : integer 
			"reduceMetric" : integer		Uniform = 0, QuadricError = 1
			"preserveGroups" : boolean 
			"preserveBoundary" : boolean 
			"adaptive" : boolean 
		[remesh]			- Remesh tool
			"density" : float 
			"smooth" : float 
			"normalThreshold" : float 
			"boundaryMode" : integer 
			"remeshType" : integer  values: Uniform = 0,	 Adaptive_Normal = 1
			"preserveGroups" : boolean 
			"adaptive" : boolean 
		[extrude]			- Extrude tool
			"offset" : float   range [-inf, inf]
			"harden" : float   range [0,1]
			"density" : float  range [0,1]
			"endType" : integer   values: Offset = 0, Flat = 1
			"directionType" : integer  values: Normal = 0, Constant = 1, XAxis = 2, YAxis = 3,	ZAxis = 4
		[extract]			- Extract tool
			"offset" : float 
			"endType" : integer  values: OffsetDistance = 0,	 PlanarFlat = 1
			"directionType" : integer  values: NormalDirection = 0,	 ConstantDirection = 1,	 UnitXDirection = 2,	 UnitYDirection = 3,	 UnitZDirection = 4
		[offset]			- Offset tool
			"offset" : float 
			"offsetWorld" : float 
			"soften" : float 
			"softenWorld" : float 
			"connected" : boolean 
			"fixedBoundary" : boolean 
		[handle]			- Handle tool
			"smooth" : float 
			"refine" : float 
			"flip" : boolean 
		[bridge]			- Bridge tool
			"refine" : float 
		[join]				- Join tool
		[weldBoundaries]	- Weld Boundaries tool
		[separate]			- Separate tool
		[planeCut]			- Plane Cut tool (with or without selection)
			"cutType" : integer  values: 	PreciseGeometricCut = 0, PreciseGeometricSlice = 1,	NearestPointAttach = 2,	RefinedICPAttach = 3   (default=0)
			"fillType" : integer  values: 	NoFill = 0,	CentroidRefinedFill = 1, DelaunayFill = 2,	DelaunayRefinedFill = 3  (default=3)
			"hardEdge" : boolean 
			"origin" : vector3f 
			"normal" : vector3f 
			"rotation" : matrix3f 
		[attractToTarget]	- Attract tool
			"density" : float 
			"smooth" : float 
			"offset" : float 
			"offsetWorld" : float 
			"normalThreshold" : float 
			"remeshType" : integer 
			"preserveGroups" : boolean 
			"preserveBoundary" : boolean 
			"findSharpEdges" : boolean 
			"adaptive" : boolean 
		[flipNormals]		- Flip Normals tool
		[fitPrimitive]		- Fit Primitive tool
			"primitiveType" : integer 
			"singlePrimitive" : boolean 
			"createNewObjects" : boolean 

		[makePart]			- Convert (selection) To Open Part
		[makeSolidPart]		- Convert (selection) To Solid Part
		[makePolygon]		- Convert (selection) to Stamp polygon

		[smooth]			- Smooth tool
			"smooth" : float 
			"scale" : float 
			"reduceType" : integer 
		[faceTransform]		- Transform tool
			"harden" : float 
			"pivotFrameMode" : integer 
			"sharedFrame" : boolean 
			"origin" : vector3f 
			"translation" : vector3f 
			"scale" : vector3f 
			"rotation" : matrix3f 
		[softTransform]		- Soft Transform tool
			"falloffDistance" : float 
			"falloffDistanceWorld" : float 
			"pivotFrameMode" : integer 
			"boundaryContinuity" : integer 
			"transitionType" : integer 
			"sharedFrame" : boolean 
			"refineMesh" : boolean 
			"holdBoundary" : boolean 
			"falloffFixed" : boolean 
			"origin" : vector3f 
			"translation" : vector3f 
			"scale" : vector3f 
			"rotation" : matrix3f  
		[warp]				- Warp tool

		[createFaceGroup]	- Create new FaceGroup for selection
		[clearFaceGroup]	- Clear FaceGroup for selection
		[smoothBoundary]	- Smooth Boundary tool
			"smoothness" : float 
			"preserveShape" : float 
			"iterations" : integer 
			"expandLoops" : integer 
			"preserveBoundary" : boolean 
		[mirror]				- Mirror tool
			"origin" : vector3f 
			"normal" : vector3f 
			"rotation" : matrix3f 
		[duplicate]				- Duplicate tool
		[transform]				- Transform tool
			"pivotFrameMode" : integer 
			"origin" : vector3f 
			"translation" : vector3f 
			"scale" : vector3f 
			"rotation" : matrix3f 
		[align]					- Align tool
		[closeCracks]			- Close Cracks tool
		[generateFaceGroups]	- Generate Face Groups tool
			"angleThreshold" : float 
		[makeSolid]				- Make Solid tool
			"offsetDistance" : float 
			"offsetDistanceWorld" : float 
			"minThickness" : float 
			"minThicknessWorld" : float 
			"edgeCollapseThresh" : float 
			"solidType" : integer 
			"solidResolution" : integer 
			"meshResolution" : integer 
			"closeHoles" : boolean 
			"transferFaceGroups" : boolean
		[hollow]				- Hollow tool
			"offsetDistance" : float 
			"offsetDistanceWorld" : float 
			"holeRadiusWorld" : float 
			"holeTaperWorld" : float 
			"hollowType" : integer 
			"solidResolution" : integer 
			"meshResolution" : integer 
			"holesPerComponent" : integer 
		[makePattern]			- Make pattern tool
			"gridResolutionScale" : float 
			"meshResolutionScale" : float 
			"dimension1" : float 
			"spacing1" : float 
			"offsetDistance" : float 
			"offsetDistanceWorld" : float 
			"edgeCollapseThresh" : float 
			"gradientValue0" : float 
			"gradientValue1" : float 
			"pattern" : integer 
			"tiling" : integer 
			"compositionMode" : integer 
			"smoothingIters" : integer 
			"dimensionGradient" : integer 
			"clipToSurface" : boolean 
			"gradientPosition0" : vector3f 
			"gradientPosition1" : vector3f 
			"origin" : vector3f 
			"translation" : vector3f 
			"rotation" : matrix3f
		[makeSlices]			- Make Slices tool
		[separateShells]		- Separate Shells tool
		[addTube]				- Add Tube tool
			"startRadius" : float 
			"startRadiusWorld" : float 
			"endRadius" : float 
			"endRadiusWorld" : float 
			"startTanLenFactor" : float 
			"lengthScale" : float 
			"collisionRadius" : float 
			"collisionRadiusWorld" : float 
			"curveType" : integer 
			"operationType" : integer 
			"directionConstraint" : integer 
			"solveIterations" : integer 
		[combine]				- Combine tool (multiple selected objects)
		[union]					- Boolean Union 
		[difference]			- Boolean Difference
		[intersection]			- Boolean Intersection

		[inspector]				- Inspector tool
			"smallComponentThreshold" : float 
			"replaceType" : integer 
		[units]					- Units/Dimensions tool
			"worldX" : float 
			"worldY" : float 
			"worldZ" : float 
		[measure]				- Measure tool
		[stability]				- Stability analysis tool
			"contactTolerance" : float 
		[strength]				- Strength analysis tool
			"showSections" : boolean 
		[overhangs]				- Overhangs/Support-Generation tool
			"overhangPreset" : integer 
			"overhangAngleTolerance" : float 
			"contactTolerance" : float 
			"verticalOffset" : float 
			"maxDraftAngle" : float 
			"density" : float 
			"layerHeight" : float 
			"postBaseSize" : float 
			"postTopSize" : float 
			"postTipSize" : float 
			"postTipHeight" : float 
			"postDiscSize" : float 
			"postDiscHeight" : float 
			"strutDensity" : float 
			"solidMinOffset" : float 
			"postResolution" : integer 
			"optimizeRounds" : integer
			"allowTopSupport" : boolean 
		[slicing]				- Slicing analysis tool
			"sliceHeight" : float 
			"minFeatureSize" : float 
		[thickness]				- Thickness analysis tool
			"minThickness" : float 
			"minThicknessWorld" : float 
		[orientation]			- Orientation optimization tool
			"overhangAngleDeg" : float 
			"strengthWeight" : float 
			"supportVolumeWeight" : float 
			"supportSurfaceAreaWeight" : float 
			"selectedOrientation" : integer 
			"origin" : vector3f 
			"translation" : vector3f 
			"rotation" : matrix3f 
		[layout]				- Print Bed layout/packing tool
			"borderWidth" : float 
			"transformationType" : integer  Translate2D=0, Translate2D_Rotate2D=1
			"packingQuality" : integer		Fastest = 0, Medium_Quality = 1, Max_Quality = 2
			"bedOrigin" : integer			RearLeftCorner = 0, Centered = 1
			"packMetric" : integer		    Circular=0, Square=2, Left-to-Right=3
		[deviation]				- Deviation measurement between two selected meshes
			"maxDeviationWorld" : float 
		[clearance]				- Clearance measurement between two selected meshes
	 */
	void AppendToolParameterCommand( std::string paramName, float fValue );
	void AppendToolParameterCommand( std::string paramName, int nValue );
	void AppendToolParameterCommand( std::string paramName, bool bValue );
	void AppendToolParameterCommand( std::string paramName, float x, float y, float z );
	void AppendToolParameterCommand( std::string paramName, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22 );

	Key AppendGetToolParameterCommand( std::string paramName );

	bool GetToolParameterCommandResult( Key k, any_result & value );
	bool GetToolParameterCommandResult( Key k, float & fValue );
	bool GetToolParameterCommandResult( Key k, int & nValue );
	bool GetToolParameterCommandResult( Key k, bool & bValue );
	bool GetToolParameterCommandResult( Key k, float & x, float & y, float & z );
	bool GetToolParameterCommandResult( Key k, float & m00, float & m01, float & m02, float & m10, float & m11, float & m12, float & m20, float & m21, float & m22 );

	// [RMS] not yet implemented for most Tools...
	Key AppendToolQuery_NewGroups();
	bool GetToolQueryResult_NewGroups(Key k, std::vector<int> & vObjects);

	/*
	 * [RMS] handle cases where tool has explicit operation, like makeSolid update
	 *   [makeSolid]
			"update"
	 *   [makePattern]
			"update"
	 *   [inspector]
			"repairAll"
	 *   [hollow]
			"update"
			"generateHoles"
			"removeHoles"
	 *   [orientation]
			"update"
	 *   [layout]
			"update"
	 *   [overhangs]
			"generateSupport"
			"removeSupport"
			"convertToSolid" :   NewObject=0, ReplaceExisting=1
	 *   [volumeBrush]
			"setPrimary":    "drag","draw","draw2","flatten","inflate","pinch","move","spikes","paintVertex","attract",
						     "bubbleSmooth","shrinkSmooth","robustSmooth",
						     "refine","reduce","adaptiveReduce","zipperEdge"
			"setSecondary":  "bubbleSmooth","shrinkSmooth","robustSmooth"
	 */
	void AppendToolUtilityCommand( std::string commandName );
	void AppendToolUtilityCommand( std::string commandName, int nValue );
	void AppendToolUtilityCommand( std::string commandName, std::string sValue );




	/*
	 * SCENE COMMANDS
	 */
	// generic "return true/false" check for scene commands that return a Key
	bool GetSceneCommandResult_IsOK(Key k);

	// write out screenshot at pFilename
	void AppendSceneCommand_SaveScreenShot(const char * pFilename);

	// open a .mix file (replaces existing file)
	Key AppendSceneCommand_OpenMixFile( const char * pFilename );
	// save current scene in a .mix file
	Key AppendSceneCommand_ExportMixFile( const char * pFilename );

	// append objects in mesh file to current scene
	Key AppendSceneCommand_AppendMeshFile( const char * pFilename );
		bool GetSceneCommandResult_AppendMeshFile( Key k, std::vector<int> & vObjects );

	// append objects in mesh file to current scene as reference objects
	Key AppendSceneCommand_AppendMeshFileAsReference( const char * pFilename );
		bool GetSceneCommandResult_AppendMeshFileAsReference( Key k, std::vector<int> & vObjects );

	Key AppendSceneCommand_ExportMeshFile_CurrentSelection( const char * pFilename );

    // create pivot in scene
    Key AppendSceneCommand_CreatePivot( frame3f f );
        bool GetSceneCommandResult_CreatePivot( Key k, int & nObjectID );
    
    // link pivot to object. If object ID is invalid, pivot is unlinked
    Key AppendSceneCommand_LinkPivot( int nPivotID, int nLinkToID );
    Key AppendSceneCommand_UnlinkPivot( int nPivotID );
    
	// remove all objects from current scene
	void AppendSceneCommand_Clear();

	// list objects in scene (by ID)
	Key AppendSceneCommand_ListObjects();
		bool GetSceneCommandResult_ListObjects( Key k, std::vector<int> & vObjects );

	// list objects in scene (by ID)
	Key AppendSceneCommand_ListSelectedObjects();
		bool GetSceneCommandResult_ListSelectedObjects( Key k, std::vector<int> & vObjects );

	// select objects by ID. Result returns objects that were actually selected
	Key AppendSceneCommand_SelectObjects(const std::vector<int> & vObjects);
		bool GetSceneCommandResult_SelectObjects( Key k, std::vector<int> & vObjects );

	Key AppendSceneCommand_DeleteSelectedObjects();

	// target controls
	Key AppendSceneCommand_SetAsTarget();
	Key AppendSceneCommand_ClearTarget();

	// object info
	Key AppendSceneCommand_GetObjectName(int nObjectID);
		bool GetSceneCommandResult_GetObjectName( Key k, std::string & objectName );
		bool GetSceneCommandResult_GetObjectName( Key k, std::vector<unsigned char> & objectName );		// [RMS] for SWIG
	Key AppendSceneCommand_SetObjectName(int nObjectID, const std::string & objectName );

	Key AppendSceneCommand_FindObjectByName(const std::string & objectName);
		bool GetSceneCommandResult_FindObjectByName( Key k, int & nObjectID );
		bool GetSceneCommandResult_FindObjectByName( Key k, any_result & nObjectID );

	// visibility
	Key AppendSceneCommand_SetVisible( int nObjectID );
	Key AppendSceneCommand_SetHidden( int nObjectID );
	Key AppendSceneCommand_ShowAll();


	/*
	 * SPATIAL QUERY COMMANDS
	 */

	Key AppendQueryCommand_ConvertScalarToWorld(float f);
		bool GetQueryResult_ConvertScalarToWorld(Key k, float * pResult);
	Key AppendQueryCommand_ConvertScalarToScene(float f);
		bool GetQueryResult_ConvertScalarToScene(Key k, float * pResult);
	Key AppendQueryCommand_ConvertPointToWorld(float fPoint[3]);
		bool GetQueryResult_ConvertPointToWorld(Key k, float * pResult);
	Key AppendQueryCommand_ConvertPointToScene(float fPoint[3]);
		bool GetQueryResult_ConvertPointToScene(Key k, float * pResult);

	// get bounding box
	Key AppendQueryCommand_GetBoundingBox();
		bool GetQueryResult_GetBoundingBox( Key k, float fMin[3], float fMax[3] );

	Key AppendQueryCommand_GetSelectedFacesBoundingBox();
		bool GetQueryResult_GetSelectedFacesBoundingBox( Key k, float fMin[3], float fMax[3] );
	Key AppendQueryCommand_GetSelectedFacesCentroid();
		bool GetQueryResult_GetSelectedFacesCentroid( Key k, float fCentroid[3] );

	// find nearest point on selected object
	Key AppendQueryCommand_FindRayIntersection( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendQueryCommand_FindRayIntersection( const vec3f & o, const vec3f & d );
		bool GetQueryResult_FindRayIntersection( Key k, frame3f * pFrame );


	// find nearest point on selected object
	Key AppendQueryCommand_FindNearestPoint( float x, float y, float z );
	Key AppendQueryCommand_FindNearestPoint( const vec3f & p );
		bool GetQueryResult_FindNearestPoint( Key k, frame3f * pFrame );


	/*
	 *  SELECTION COMMANDS
	 */

	void AppendSelectCommand_All( );

	// commands:
	//  "selectVisible"    nArgument 0=Replace 1=Append 2=Remove
	//  "expandToConnected"
	//  "expandToGroups"
	//  "expandByOneRing"
	//  "contractByOneRing"
	//  "invert"
	//  "invertConnected"
	//  "optimizeBoundary"
	void AppendSelectUtilityCommand( std::string commandName );
	void AppendSelectUtilityCommand( std::string commandName, int nArgument );


	// parameter is 3D point
	Key AppendSelectCommand_NearestComponent( float cx, float cy, float cz );
	Key AppendSelectCommand_ContainingComponent( float cx, float cy, float cz );

	// parameters are ray origin & direction
	Key AppendSelectCommand_FirstComponentIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_AllComponentsIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );

	// parameter is 3D point
	Key AppendSelectCommand_NearestTriangle( float cx, float cy, float cz );

	// parameters are ray origin & direction
	Key AppendSelectCommand_FirstTriangleIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_AllTrianglesIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );

	// parameter is sphere center/radius
	Key AppendSelectCommand_InsideSphere( float cx, float cy, float cz, float r );
		bool GetSelectCommandResult_InsideSphere( Key k );

	Key AppendSelectCommand_ByFaceGroups( const std::vector<int> & vGroupIDs );
		bool GetSelectCommandResult_ByFaceGroups( Key k );

	Key AppendSelectCommand_ListSelectedFaceGroups();
		bool GetSelectCommandResult_ListSelectedFaceGroups( Key k, std::vector<int> & vGroupIDs );


	

	/*
	 *  ACTION COMMANDS
	 */

	Key AppendActionCommand_BrushStroke3D( const std::vector<brush_stamp> & vPoints );


	/* Part Drop. During interactive part drop you can also use AppendToolParameterCommand() with following parameters:
			"radius" : float range [0, inf]
			"angle" : float range [0, 2pi]
			"position" : 3-float point   *** must be point on surface ***

			"deformType" : values  COILS = 0, RotInvCoord = 1  (default=0)
			"optimize" : boolean
			"bend" : float range [0, inf]
			"bulge" : float range [-180,180]
			"offset" : float range [-inf, inf]
			"scale" : float range [0.0001, inf]
			"scaleFalloff" : float range [0.0001, 1]

			"smoothRadius" : float range [0, inf]
			"tweakRaduis" : float range [0.0001, inf]
	*/
	Key AppendActionCommand_DropPartAtPoint( const char * pPartPath, const frame3f & f, float fRadius, bool bInteractive = false );
	Key AppendActionCommand_UpdateDropPart( const frame3f & f, float fRadius, bool bMinimizeRotation );
	Key AppendActionCommand_AcceptDropPart( );
	// [RMS] for any of above, returns whether drop was successful or not. Only returns new GroupIDs when drop is completed
	bool GetActionCommandResult_DropPart( Key k, std::vector<int> & vNewGroups );


	// [RMS] TODO drop solid part (or also handle with above?)

	Key AppendActionCommand_InsertPolygon( float x, float y, float z, float fRadius );
	bool GetActionCommandResult_InsertPolygon( Key k, int & nNewGroupID );





	/*
	 * Serialization / Execution
	 */

	void Store(rms::BinarySerializer & s);
	void Restore(rms::BinarySerializer & s);
	void Restore(unsigned char * pBytes, size_t nSize);

	void Execute(MainWindow * pMainWin);

	void Store_Results(rms::BinarySerializer & s);
	void Restore_Results(rms::BinarySerializer & s);
	void Restore_Results(unsigned char * pBytes, size_t nSize);

	void Store_Internal();
	size_t Store_GetSize();
	void Store_GetBuffer(unsigned char * pBytes, size_t nSize );


public:
	struct vector_int {
		unsigned int nElements;
		int data[4096];
		vector_int & append(int i);
	};
	struct vector_float {
		unsigned int nElements;
		float data[4096];
		vector_float & append(float f);
	};

private:

	// [RMS] API clients do not need to touch this, but mm-internals do
	enum CommandType {
		MouseEventCommand,
		CameraControlCommand,

		ToolParameterChangeCommand,
		BeginToolCommand,
		CompleteToolCommand,
		ToolParameterCommand,

		SceneCommand,
		SelectCommand,

		BrushCommand,
		PartCommand,
		StampCommand,

		SpatialQueryCommand,
		GenericQueryCommand
	};


	enum MouseEventType {
		MouseDown, MouseMove, MouseUp
	};
	struct MouseEvent {
		MouseEventType eType;
		bool bLeft, bRight, bMiddle;
		float x,y;
		bool bCoordIsAbsolute;

		// cannot have constructor because we are putting this in union...
		void init() { eType = MouseMove; bLeft = bRight = bMiddle = false; x = y = 0; bCoordIsAbsolute = true; }
	};



	enum CameraCmdType {
		CamManip, CamToggleSnap, CamOrbit, CamTurntable, CamPan, CamDolly, CamRecenter, CamSet, CamQuery, CamGetRay, 
		SetViewNormalMode, SetViewColorMode, CamOrthographic, CamPerspective, 	
		SetShowWireframe, SetShowBoundaries, SetShowGrid, SetShowPrinterBed, SetTransparentTarget,
		SetShader_Default, SetShader_XRay, SetShader_Texture, SetShader_UV, SetShader_Overhang,
		ShowObjectBrowser, HideObjectBrowser
	};
	struct CameraCmd {
		CameraCmdType eType;
		float fVal1, fVal2;
		vec3f eye, target, up;
		bool bFlag;
		int nx, ny;
	};
	struct CameraCmdResult {
		frame3f f;
		vec3f target;
		float fov;
		int nx, ny;
	};



	struct ToolStringCmd {
		char name[32];
	};

	enum ToolParamType {
		ToolParam_Float = 0,
		ToolParam_Int = 1,
		ToolParam_Bool = 2,
		ToolParam_Vec3 = 3,
		ToolParam_Mat3 = 4,
		ToolParam_String = 5,

		ToolParam_Get = 9,
		ToolParam_Get_Float = 10,
		ToolParam_Get_Int = 11,
		ToolParam_Get_Bool = 12,
		ToolParam_Get_Vec3 = 13,
		ToolParam_Get_Mat3 = 14,


		ToolParam_Utility = 20,
		ToolParam_Utility_Float = 21,
		ToolParam_Utility_Int = 22,
		ToolParam_Utility_Bool = 23,
		ToolParam_Utility_Vec3 = 24,
		ToolParam_Utility_Mat3 = 25,
		ToolParam_Utility_String = 26,
	};
	struct ToolParamValueCmd {
		char name[32];
		ToolParamType eType;
		union {
			float f;
			int i;
			vec3f vec;
			mat3f mat3;
			fstring str;
		} v;
	};
	struct ToolParamValueCmdResult {
		int OK;
		char name[32];
		ToolParamType eType;
		union {
			float f;
			int i;
			vec3f vec;
			mat3f mat3;
		} v;
	};


	enum SceneCmdType {
		ClearScene,
		AppendMeshFile,
		OpenMixFile,
		ExportMixFile,
		ExportMeshFile_SelectedObjects,
		ListObjects,
		ListSelectedObjects,
		SelectObjects,
		SetAsTarget,
		ClearTarget,
		DeleteSelected,
		GetObjectName,
		SetObjectName,
		FindObjectByName,
		SaveScreenShot,
		SetVisible,
		SetHidden,
		ShowAll,
		AppendMeshFileAsReference,
        CreatePivot,
        LinkPivot
	};
	struct SceneCmd {
		SceneCmdType eType;
		fstring str;
		vector_int nObjectIDs;
        frame3f f;
    };
	struct SceneCmdResult {
		int OK;
		fstring str;
		vector_int nObjectIDs;
	};



	enum SelectCmdType {
		SelectAll,

		SelectNearestComponent,
		SelectContainingComponent,
		SelectFirstComponentIntersectingRay,
		SelectAllComponentsIntersectingRay,
		SelectNearestTriangle,
		SelectFirstTriangleIntersectingRay,
		SelectAllTrianglesIntersectingRay,

		SelectInsideSphere,
		SelectFaceGroups,

		ListSelectedFaceGroups,

		SelectUtility
	};
	struct SelectCmd {
		SelectCmdType eType;
		vec3f p;
		vec3f d;
		float r;
		int n;
		char str[32];
		vector_int vGroups;
	};
	struct SelectCmdResult {
		int OK;
		vector_int vGroups;
	};


	enum BrushCmdType {
		Stroke3D
	};
	struct BrushCmd {
		BrushCmdType eType;
		vector_float vStamps;
	};



	enum PartCmdType {
		DropPart, UpdatePart, AcceptPart
	};
	struct PartCmd {
		PartCmdType eType;
		fstring filename;
		frame3f f;
		float r;
		bool bFlag;
	};
	struct PartCmdResult {
		int OK;
		vector_int vNewGroups;
	};


	enum StampCmdType {
		InsertPolygonStamp
	};
	struct StampCmd {
		StampCmdType eType;
		float x,y,z;
		float r;
	};
	struct StampCmdResult {
		int OK;
		int nNewGroupID;
	};


	enum SpatialQueryType {
		SelectedObjectsBoundingBoxQuery,
		NearestPointSpatialQuery,
		RayIntersectionSpatialQuery,
		SelectedFacesBoundingBoxQuery,
		SelectedFacesCentroidQuery
	};
	struct SpatialQueryCmd {
		SpatialQueryType eType;
		vec3f p;
		vec3f d;
	};
	struct SpatialQueryResult {
		int OK;
		vector_float v;
	};



	enum GenericQueryCmdType {
		ToolManager_NewGroups,
		ToolManager_ScalarToWorld,
		ToolManager_ScalarToScene,
		ToolManager_PointToWorld,
		ToolManager_PointToScene
	};
	struct GenericQueryCmd {
		GenericQueryCmdType eType;
		int n;
		vec3f p;
		fstring str;
	};
	struct GenericQueryCmdResult {
		int OK;
		vec3f p;
		vector_int vList;
	};



private:
	struct Command {
		CommandType eType;
		union {
			MouseEvent mouse;
			CameraCmd camera;

			ToolStringCmd toolctrl;
			ToolParamValueCmd toolparam;

			SceneCmd scene;
			SelectCmd select;

			BrushCmd brush;
			PartCmd part;
			StampCmd stamp;

			SpatialQueryCmd spatial;
			GenericQueryCmd generic_query;
		} c;
		union {
			CameraCmdResult camera;

			ToolParamValueCmdResult toolparam;

			SceneCmdResult scene;
			SelectCmdResult select;

			PartCmdResult part;
			StampCmdResult stamp;

			SpatialQueryResult spatial;
			GenericQueryCmdResult generic_query;
		} r;

		void init();			// clear all data files
	};
	std::vector<Command> m_vCommands;
	Key append_command(Command & c) { Key k = (unsigned int)m_vCommands.size(); m_vCommands.push_back(c); return k; }


	bool Execute_IO(MainWindow * pMainWin, unsigned int k);
	bool Execute_Scene(MainWindow * pMainWin, unsigned int k);
	bool Execute_Select(MainWindow * pMainWin, unsigned int k);
	bool Execute_Tool(MainWindow * pMainWin, unsigned int k);
		bool Execute_Tool_Utility(MainWindow * pMainWin, unsigned int k);
	bool Execute_Query(MainWindow * pMainWin, unsigned int k);

	rms::BinarySerializer m_internalStore;
};





}   // end namespace mm

#endif // _MM_STORED_COMMANDS_H__