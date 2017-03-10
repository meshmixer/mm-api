/********************************************************************
 * (C) Copyright 2016 by Autodesk, Inc. All Rights Reserved. By using
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
//#define USING_MM_COMMAND_API

#include <BinarySerializer.h>

namespace mm {

class MainWindow;

template <typename Type, size_t N>
struct wrapped_array {
	Type data[N];
};


struct vec3f {
	float x,y,z;
};
struct vec3i {
	int i,j,k;
};
struct mat3f {
	wrapped_array<float, 9> m;
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

	void ViewControl_TakeFocus();

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
		"alignToTarget"		- Align to Target tool
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
        "createPivot"           - Create Pivot tool
		"unwrap"				- Unwrap tool
		"generateComplex"		- Generate Complex tool
		"splitComplex"			- Split Complex tool
		"filterComplex"			- Filter Complex tool

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
		"meshQuery"				- display/measure various properties of selected mesh

		"exportSVG"				- export selected objects as SVG
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
			"radiusWorld" : float 
			"creaseAngleThreshold" : float 
			"brushType" : integer			SphereVolume = 0, SphereDisc = 1, SurfaceUV = 2
			"expandMode" : integer			GeodesicDistance = 0, LocalCreaseAngleDeviation = 1, ColorSimilarity = 2
			"symmetry" : boolean 
			"allowBackFaces" : boolean 
			"restrictToExterior" : boolean 
		[volumeBrush]		- start volume brush tool
			"strength" : float  range [0,1]
			"size" : float 
			"depth" : float          range [-1,1]
			"lazyness" : float       range [0,1]
			"refine" : float         range [0,1]
			"reduce" : float         range [0,1]
			"refineSmooth" : float   range [0,1]
            "refineAdaptive" : float range [0,1]
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
			"createNewGroups" : boolean 
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
             "edgeLength" : float
             "edgeLengthWorld" : float
             "smooth" : float
             "transitionWidth" : float
             "transitionWidthWorld" : float
             "normalThreshold" : float
			 "sharpThreshold" : float 
             "goalType" : integer
                    RelativeDensity = 0, AdaptiveDensity = 1, TargetEdgeLength = 2, LinearSubdivision = 3
             "iterations" : integer
             "boundaryMode" : integer
                    FreeBoundary = 0, FixedBoundary = 1, RefinedFixedBoundary = 2
             "preserveGroups" : boolean
			"smoothAcrossGroups" : boolean 
			"findSharpEdges" : boolean 

		[extrude]			- Extrude tool
			"offset" : float   range [-inf, inf]
			"harden" : float   range [0,1]
			"density" : float  range [0,1]
			"endType" : integer   values: Offset = 0, Flat = 1
			"directionType" : integer  values: Normal = 0, Constant = 1, XAxis = 2, YAxis = 3,	ZAxis = 4
			"preserveGroups" : boolean 
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
			"preserveGroups" : boolean 
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
             "enableRefinement" : boolean
             "preserveGroups" : boolean
             "preserveBoundary" : boolean
             "findSharpEdges" : boolean
		[alignToTarget]
			"toleranceWorld" : float 
			"sourceType" : integer 
			"transformMode" : integer 
			"maxIterations" : integer 
		[flipNormals]		- Flip Normals tool
		[fitPrimitive]		- Fit Primitive tool
			"primitiveType" : integer
                    Plane_Square = 0, Plane_Rectangle = 1, Cylinder = 2, Sphere_Centroid = 3, Sphere_LeastSquares = 4,
                    Disc_Centroid = 5, Linear_Sweep = 6, Convex_Hull = 7
			"singlePrimitive" : boolean
			"createNewObjects" : boolean 
			"meshDensity" : integer 
		[makePart]			- Convert (selection) To Open Part
		[makeSolidPart]		- Convert (selection) To Solid Part
		[makePolygon]		- Convert (selection) to Stamp polygon

		[smooth]			- Smooth tool
			"smooth" : float 
			"scale" : float 
			"smoothType" : integer					Cotan = 0, Uniform = 1, Membrane = 2
			"constraintRings" : integer 
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
			"preserveGroupBorders" : boolean 
			"projectToTarget" : boolean 
			"createNewGroups" : boolean
		[mirror]				- Mirror tool
			"origin" : vector3f 
			"normal" : vector3f 
			"rotation" : matrix3f 
		[duplicate]				- Duplicate tool
		[transform]				- Transform tool
			"pivotFrameMode" : integer
                WorldFrame = 0, LocalFrame = 1
			"origin" : vector3f
            "originWorld" : vector3f
			"translation" : vector3f
            "translationWorld" : vector3f
			"scale" : vector3f 
			"dimensions" : vector3f 
			"rotation" : matrix3f 
			"enableSnapping" : boolean 
			"absoluteSnapping" : boolean 
			"snapStepSize" : float 
            "initialFrameOrigin" : vector3f
            "initialFrameOrientation" : matrix3f
		[align]					- Align tool
             "sourceType" : integer
                    Source_ObjectBaseCenter = 1, Source_BoundingBoxCenter = 0, Source_SingleSurfacePoint = 10, 
                    Source_SurfaceSamples = 11, Source_SurfaceDisc = 12, Source_Pivot = 13, Source_LastToolFrame = 20
             "targetType" : integer
                    Target_WorldOrigin_Yup = 0, Target_WorldOrigin_Zup = 1, Target_WorldOrigin_Xup = 2, Target_GroundPlanePoint = 9,
                    Target_SingleSurfacePoint = 10, Target_SurfaceSamples = 11, Target_SurfaceDisc = 12, Target_Pivot = 13
             "transformMode" : integer
                    Translate = 0, TranslateAndRotate = 1
             "flip" : boolean
             "origin" : vector3f
             "rotation" : matrix3f
             "sourceFrame" : frame3f
             "targetFrame" : frame3f
		[closeCracks]			- Close Cracks tool
		[generateFaceGroups]	- Generate Face Groups tool
			"angleThreshold" : float 
			"mode" : integer						OpeningAngle = 0, NormalDeviation = 1
			"smallGroupThreshold" : integer 
		[makeSolid]				- Make Solid tool
			"solidCellSize" : float 
			"meshCellSize" : float 
			"offsetDistance" : float 
			"offsetDistanceWorld" : float 
			"minThickness" : float 
			"minThicknessWorld" : float 
			"edgeCollapseThresh" : float 
			"solidType" : integer				Blocky = 0, Fast = 1, Accurate = 2, FastSharp = 3
			"solidResolution" : integer 
			"meshResolution" : integer 
			"colorTransferMode" : integer    		NoColorTransfer = 0, AutoColorTransfer = 1, TransferVertexColors = 2, TransferMaterials = 3
			"closeHoles" : boolean 
			"autoRepair" : boolean 
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
					MeshEdges = 0, DualMeshEdges = 1, MeshEdges_DelaunayEdges = 2, DualMeshEdges_DualDelaunayEdges_Snapped = 3,
					TiledTubes2D = 4, TiledSpheres3D = 5, Lattice = 6, 
					PoissonDistribution3D = 7, PoissonDistribution2D = 8, FaceGroupEdges = 9,
					CustomPattern = 20
			"tiling" : integer 
					RegularGrid = 0, SpherePacking = 1
			"compositionMode" : integer 
					Difference = 0, Intersection = 1
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
			"startPosition" : vector3 (read-only, in scene coordinates - use utility command to modify)
			"endPosition"   : vector3 (read-only, in scene coordinates - use utility command to modify)
        [createPivot]           - Create Pivot tool
             "offset" : float
             "offsetWorld" : float
             "positionMode" : integer  
                    SurfaceHitPoint = 0, NearestVertex = 1, NearestEdgePoint = 2, NearestEdgeMidPoint = 3, 
                    NearestFaceCenter = 4, FacegroupBorder = 5, FacegroupCenter = 6, FacegroupBorderCenter = 7, 
                    BoundaryLoopCenter = 8, FirstRayMidpoint = 10, FirstNormalMidpoint = 11, 
                    WorldBoundingBoxPoint = 20, LocalBoundingBoxPoint = 21, ExistingPivot = 25, FromLastToolFrame = 30
             "frameMode" : integer
                     WorldFrame = 0, FromGeometry = 1, AlongEyeRay = 2
             "symmetric" : boolean
             "linkToTarget" : boolean
     
		[combine]				- Combine tool (multiple selected objects)

		[union]					- Boolean Union 
		[difference]			- Boolean Difference
		[intersection]			- Boolean Intersection
			"targetLengthScale" : float 
			"robustModeDelta" : float 
			"operationType" : integer			Union = 0, Difference = 1, Intersection = 2
			"solutionType" : integer			FastApproximate = 0, Precise = 1, ExtraPrecise = 2
			"maxRefineDepth" : integer 
			"previewIterations" : integer 
			"mergeRings" : integer 
			"preserveGroupBoundaryEdges" : boolean 
			"postReduce" : boolean 
			"useIntersectionCurves" : boolean 
			"robustMode" : boolean 

		[inspector]				- Inspector tool
			"smallComponentThreshold" : float 
			"replaceType" : integer 
		[units]					- Units/Dimensions tool
			"units" : int	Centimeters = 1, Inches = 2, Millimeters = 3, Meters = 4, Feet = 5
			"worldX" : float 
			"worldY" : float 
			"worldZ" : float 
			"activeDimensionWorld" : float 
			"activeDimensionTarget" : float 
			"averageDeviation" : float 
			"maxDeviation" : float 
		[measure]				- Measure tool
			"snapMode" : integer			SnapNone = 0, SnapToVertices = 1, SnapToEdges = 2, SnapToFacegroupBorders = 3
			"dimensionValid" : boolean 
			"dimension" : float 
			"minDimension" : float 
			"maxDimension" : float 
			"queryPosition" : vector3f 
		[stability]				- Stability analysis tool
			"contactTolerance" : float 
			"isStable" : boolean
			"volume" : float
			"surfaceArea" : float
			"centerOfMassWorld" : vector3f
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
			"minFeatureSize" : float 
			"minFeatureSizeWorld" : float 
			"coneAngle" : float 
			"grazingThresh" : float 
			"wallThresh" : float 
			"coneSamples" : integer 
			"wallsOnly" : boolean 
			"bakeVertexColors" : boolean 
		[orientation]			- Orientation optimization tool
			"overhangAngleDeg" : float 
			"strengthWeight" : float 
			"supportVolumeWeight" : float 
			"supportSurfaceAreaWeight" : float 
			"selectedOrientation" : integer 
			"origin" : vector3f 
			"translation" : vector3f 
			"rotation" : matrix3f 
			"containInPrintVolume" : boolean 
		[layout]				- Print Bed layout/packing tool
			"borderWidth" : float 
			"transformationType" : integer  Translate2D=0, Translate2D_Rotate2D=1
			"packingQuality" : integer		Fastest = 0, Medium_Quality = 1, Max_Quality = 2
			"bedOrigin" : integer			RearLeftCorner = 0, Centered = 1
			"packMetric" : integer		    Circular=0, Square=2, Left-to-Right=3
		[deviation]				- Deviation measurement between two selected meshes
			"maxDeviation" : float 
			"maxDeviationWorld" : float 
			"measuredMaxDeviationWorld" : float
			"symmetric"	: bool
		[clearance]				- Clearance measurement between two selected meshes
			"minClearance" : float
			"minClearanceWorld" : float
			"measuredMinClearanceWorld" : float
			"symmetric"	: bool
		[meshQuery]
			"snapMode" : integer 
			"colorMode" : integer 
			"triAspectRatio" : float 
			"triMinAngle" : float 
			"triMaxAngle" : float 
			"vertexGaussCurvature" : float 
			"vertexMeanCurvature" : float 
			"colorMinInteriorAngle" : float 
			"colorMaxInteriorAngle" : float 
			"colorMinCurvature" : float 
			"colorMaxCurvature" : float 
			"colorInvalidRange" : float 
			"vertexID" : integer 
			"edgeID" : integer 
			"triangleID" : integer 
			"groupID" : integer 
		[generateComplex]
			"postSmoothAlpha" : float 
			"offsetDistance" : float 
			"offsetDistanceWorld" : float 
			"mode" : integer 
			"refineIters" : integer 
			"remesh" : boolean 
			"projectToFlat" : boolean 
		[exportSVG]
			"svgType" : integer 
			"boundaryMode" : integer 
			"faceFillColorMode" : integer 
			"boundaryFillColorMode" : integer 
			"includeEdges" : boolean 
			"includeFaces" : boolean 
		[unwrap]
			"elementMode" : integer				UnwrapByComponents = 0, UnwrapByGroups = 1
			"algorithm" : integer				AsRigidAsPossible = 0, DiscreteConformal = 1
			"packingMode" : integer 			Centered = 0, Linear = 1, AutoPack = 2 
			"packingWidth" : float 
			"packingHeight" : float 
			"packingBorder" : float 
		[dropPart]							(start with AppendActionCommand_DropPartAtPoint)
			"radius" : float range [0, inf]   (IN SCENE COORDINATES!!)
			"radiusWorld" : float 
			"angle"  : float range [0, 2pi]
			"bend" : float range [0, inf]
			"bulge" : float range [-180,180]
			"offset" : float range [-inf, inf]
			"scale" : float range [0.0001, inf]
			"scaleFalloff" : float range [0.0001, 1]
			"smoothRadius" : float range [0, inf]
			"tweakRaduis" : float range [0.0001, inf]
			"deformType" : integer				COILS = 0, RotInvCoord = 1  (default=0)
			"optimize" : boolean 
			"flip" : boolean 
			"createVolume" : boolean 
			"position" : vector3f *** must be point on surface ***
		[dropSolidPart]						(start with AppendActionCommand_DropSolidPartAtPoint)
			"radius" : float 
			"radiusWorld" : float 
			"angle" : float 
			"offset" : float 
			"offsetWorld" : float 
			"operationType" : integer			AppendToMesh = 0, CreateNewObject = 1, BooleanUnion = 2, BooleanSubtract = 3
			"flip" : boolean 
			"position" : vector3f 
			"initialFrameX" : vector3f 
	 */
	void AppendToolParameterCommand( std::string paramName, float fValue );
	void AppendToolParameterCommand( std::string paramName, int nValue );
	void AppendToolParameterCommand( std::string paramName, bool bValue );
	void AppendToolParameterCommand( std::string paramName, float x, float y, float z );
	void AppendToolParameterCommand( std::string paramName, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22 );
	void AppendToolParameterCommand( std::string paramName, vec3f v);
	void AppendToolParameterCommand( std::string paramName, mat3f m );

	Key AppendGetToolParameterCommand( std::string paramName );

	bool GetToolParameterCommandResult( Key k, any_result & value );
	bool GetToolParameterCommandResult( Key k, float & fValue );
	bool GetToolParameterCommandResult( Key k, int & nValue );
	bool GetToolParameterCommandResult( Key k, bool & bValue );
	bool GetToolParameterCommandResult( Key k, float & x, float & y, float & z );
	bool GetToolParameterCommandResult( Key k, float & m00, float & m01, float & m02, float & m10, float & m11, float & m12, float & m20, float & m21, float & m22 );
	bool GetToolParameterCommandResult( Key k, vec3f & v );
	bool GetToolParameterCommandResult( Key k, mat3f & m );

	// [RMS] not yet implemented for most Tools...
	Key AppendToolQuery_NewGroups();
	bool GetToolQueryResult_NewGroups(Key k, std::vector<int> & vGroups);

	Key AppendToolQuery_NewObjects();
	bool GetToolQueryResult_NewObjects(Key k, std::vector<int> & vObjects);


	/*
	 * [RMS] handle cases where tool has explicit operation, like makeSolid update
	 *	 [any tool]
			"resetToDefaults"
	 *	 [transform]
			"setActivePivot"	// argument is integer ID of PivotSO
	 *	 [planeCut]
			"setActivePivot"	// argument is integer ID of PivotSO
	 *   [align]
			"setSourcePivot"	// argument is integer ID of PivotSO
			"setDestPivot"		// argument is integer ID of PivotSO
	 *   [makeSolid]
			"update"
	 *   [makePattern]
			"update"
			"addSegment"		// only applicable to Custom pattern
			"clearSegments"		// only applicable to Custom pattern
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
	 *   [generateComplex]
			"autoGenerate"
	 *   [meshQuery]
			"setQueryPoint"	 : vec3
	 *   [createPivot]
			"dropPivot"
	 *   [measure]
			"setSourcePoint" : vec3
			"setDestPoint"	 : vec3
			"setMeasureMode" : int    MinInteriorDistance = 0, MinExteriorDistance = 1, MaxInteriorDistance = 2, MaxExteriorDistance = 3,  FixedPointPair = 4, PositionQuery = 5, CylinderFit = 6
			"setDirectionMode" : int  NormalDirection = 0, XAxis = 1, YAxis = 2, ZAxis = 3
	 *   [alignToTarget]
			"improveSolution"
	 *   [addTube]
			"setStartPoint"  : vec3
			"setEndPoint"    : vec3
	 *   [volumeBrush]
			"setPrimary":    "drag","draw","draw2","flatten","inflate","pinch","move","spikes","paintVertex","attract",
						     "bubbleSmooth","shrinkSmooth","robustSmooth",
						     "refine","reduce","adaptiveReduce","zipperEdge"
			"setSecondary":  "bubbleSmooth","shrinkSmooth","robustSmooth"
	 *   [surfaceBrush]
			"setPrimary":    "draw","draw2","draw2","drawmax","inflate","paintVertex","blur",
						     "bubbleSmooth","shrinkSmooth","robustSmooth"
			"setSecondary":  "bubbleSmooth","shrinkSmooth","robustSmooth"
	 */
	void AppendToolUtilityCommand( std::string commandName );
	void AppendToolUtilityCommand( std::string commandName, int nValue );
	void AppendToolUtilityCommand( std::string commandName, std::string sValue );
	void AppendToolUtilityCommand( std::string commandName, const vec3f & p );
	void AppendToolUtilityCommand( std::string commandName, const vec3f & v0, const vec3f & v1, float r0, float r1 );




	/*
	 * SCENE COMMANDS
	 */
	// generic "return true/false" check for scene commands that return a Key
	bool GetSceneCommandResult_IsOK(Key k);

	// write out screenshot at pFilename
	StoredCommands::Key AppendSceneCommand_SaveScreenShot(const char * pFilename);

	// open a .mix file (replaces existing file)
	Key AppendSceneCommand_OpenMixFile( const char * pFilename );
	// save current scene in a .mix file
	Key AppendSceneCommand_ExportMixFile( const char * pFilename );

	// append objects in mesh file to current scene
	Key AppendSceneCommand_AppendMeshFile( const char * pFilename );
		bool GetSceneCommandResult_AppendMeshFile( Key k, std::vector<int> & vObjects );

	// append binary packed mesh to current scene
	Key AppendSceneCommand_AppendPackedMeshFile( const char * pFilename );
		bool GetSceneCommandResult_AppendPackedMeshFile( Key k, int & nObjectID );
		bool GetSceneCommandResult_AppendPackedMeshFile( Key k, any_result & nObjectID );

	// append objects in mesh file to current scene as reference objects
	Key AppendSceneCommand_AppendMeshFileAsReference( const char * pFilename );
		bool GetSceneCommandResult_AppendMeshFileAsReference( Key k, std::vector<int> & vObjects );

	Key AppendSceneCommand_ExportMeshFile_CurrentSelection( const char * pFilename );

	Key AppendSceneCommand_ExportAsPackedMeshFile( const char * pFilename, int nObjectID );

    // create pivot in scene
    Key AppendSceneCommand_CreatePivot( frame3f f );
        bool GetSceneCommandResult_CreatePivot( Key k, int & nObjectID );
		bool GetSceneCommandResult_CreatePivot( Key k, any_result & nObjectID );
    
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

	Key AppendSceneCommand_GetObjectUUID(int nObjectID);
		bool GetSceneCommandResult_GetObjectUUID( Key k, std::string & objectUUID );
		bool GetSceneCommandResult_GetObjectUUID( Key k, std::vector<unsigned char> & objectUUID );		// [RMS] for SWIG


	Key AppendSceneCommand_FindObjectByName(const std::string & objectName);
		bool GetSceneCommandResult_FindObjectByName( Key k, int & nObjectID );
		bool GetSceneCommandResult_FindObjectByName( Key k, any_result & nObjectID );

	Key AppendSceneCommand_FindObjectByUUID(const std::string & objectUUID);
		bool GetSceneCommandResult_FindObjectByUUID( Key k, int & nObjectID );
		bool GetSceneCommandResult_FindObjectByUUID( Key k, any_result & nObjectID );

	// -1 = Unknown, 1 = Mesh, 2 = Complex, 3 = Pivot, 4 = LiveMesh, 5 = Reference
    Key AppendSceneCommand_GetObjectType( int nObjectID );
        bool GetSceneCommandResult_GetObjectType( Key k, int & nObjectType );
        bool GetSceneCommandResult_GetObjectType( Key k, any_result & nObjectType );

    Key AppendSceneCommand_GetObjectFrame( int nObjectID );
        bool GetSceneCommandResult_GetObjectFrame( Key k, frame3f & f );

    Key AppendSceneCommand_SetObjectFrame( int nObjectID, const frame3f & f );
        bool GetSceneCommandResult_SetObjectFrame( Key k );

	// list all face groups in mesh object (warning: can overflow buffer if # of groups is very large)
	Key AppendSceneCommand_ListFaceGroups( int nObjectID );
		bool GetSceneCommandResult_ListFaceGroups( Key k, std::vector<int> & vGroupIDs );

	// visibility
	Key AppendSceneCommand_SetVisible( int nObjectID );
	Key AppendSceneCommand_SetHidden( int nObjectID );
	Key AppendSceneCommand_ShowAll();

	// input live mesh
	Key AppendSceneCommand_CreateLiveMeshObject( const char * pFilename );
		bool GetSceneCommandResult_CreateLiveMeshObject( Key k, std::string & portName, int & nObjectID );
		bool GetSceneCommandResult_CreateLiveMeshObject( Key k, std::vector<unsigned char> & portName, any_result & nObjectID );  // for SWIG

	// note: currently lock request will freeze main thread in app until lock is achieved...
	Key AppendSceneCommand_RequestLiveMeshLock( const char * pPortName );
	Key AppendSceneCommand_ReleaseLiveMeshLock( const char * pPortName );
	Key AppendSceneCommand_NotifyLiveMeshUpdate( const char * pPortName );

	// tracking live mesh (when nObjectID is modified, PackedMesh written to pFilename and then UDP port gets a datagram)
	//   NOTE: when reading this file, use lock functions above!!
	Key AppendSceneCommand_CreateTrackingLiveMesh( const char * pFilename, int nObjectID, int nUDPNotificationPort);
		bool GetSceneCommandResult_CreateTrackingLiveMesh( Key k, std::string & portName );
		bool GetSceneCommandResult_CreateTrackingLiveMesh( Key k, std::vector<unsigned char> & portName );
	Key AppendSceneCommand_HaltTrackingLiveMesh( const char * pPortName );


	Key AppendSceneCommand_SelectPrinter( const char * pPrinterName );


	/*
	 * Mesh modification commands
	 */

    // create pivot in scene
    Key AppendSceneCommand_CreateMesh();
        bool GetSceneCommandResult_CreateMesh( Key k, int & nObjectID );
		bool GetSceneCommandResult_CreateMesh( Key k, any_result & nObjectID );

	// Discard un-used vertex and triangle IDs, and pack the rest into contiguous IDs.
	// You **MUST** use this on existing Mesh objects before you use per-vertex/triangle commands, otherwise the results will be garbage
	Key AppendSceneCommand_CompactMesh( int nObjectID );

	// notify mesh that it has been modified. If you use Set or Append commands below, you will not see changes
	// reflected until you run this command
	Key AppendSceneCommand_UpdateMesh( int nObjectID );

    Key AppendSceneCommand_GetVertexCount( int nObjectID );
        bool GetSceneCommandResult_GetVertexCount( Key k, int & nCount );
        bool GetSceneCommandResult_GetVertexCount( Key k, any_result & nCount );
    Key AppendSceneCommand_GetTriangleCount( int nObjectID );
        bool GetSceneCommandResult_GetTriangleCount( Key k, int & nCount );
        bool GetSceneCommandResult_GetTriangleCount( Key k, any_result & nCount );

    Key AppendSceneCommand_GetVertexPosition( int nObjectID, int nVertexID );
        bool GetSceneCommandResult_GetVertexPosition( Key k, vec3f & v );
    Key AppendSceneCommand_GetVertexNormal( int nObjectID, int nVertexID );
        bool GetSceneCommandResult_GetVertexNormal( Key k, vec3f & v );
    Key AppendSceneCommand_GetVertexColor( int nObjectID, int nVertexID );
        bool GetSceneCommandResult_GetVertexColor( Key k, vec3f & v );
    Key AppendSceneCommand_GetVertexInfo( int nObjectID, int nVertexID );		// more efficient than each of above sequentially
        bool GetSceneCommandResult_GetVertexInfo( Key k, vec3f & v, vec3f & n, vec3f & c );

    Key AppendSceneCommand_GetTriangleIndices( int nObjectID, int nTriangleID );
        bool GetSceneCommandResult_GetTriangleIndices( Key k, vec3i & t );
    Key AppendSceneCommand_GetTriangleGroup( int nObjectID, int nTriangleID );
        bool GetSceneCommandResult_GetTriangleGroup( Key k, int & nGroupID );
        bool GetSceneCommandResult_GetTriangleGroup( Key k, any_result & nGroupID );

	Key AppendSceneCommand_SetVertexPosition(int nObjectID, int nVertexID, vec3f & v );
	Key AppendSceneCommand_SetVertexNormal(int nObjectID, int nVertexID, vec3f & v );
	Key AppendSceneCommand_SetVertexColor(int nObjectID, int nVertexID, vec3f & v );
	Key AppendSceneCommand_SetTriangleGroup(int nObjectID, int nTriangleID, int nGroupID );

	Key AppendSceneCommand_SetAllVertexColors(int nObjectID, vec3f & v );

    Key AppendSceneCommand_AppendVertex( int nObjectID, vec3f & v );
    Key AppendSceneCommand_AppendVertex( int nObjectID, vec3f & v, vec3f & n );
    Key AppendSceneCommand_AppendVertex( int nObjectID, vec3f & v, vec3f & n, vec3f & c );
        bool GetSceneCommandResult_AppendVertex( Key k, int & nNewVertexID );
        bool GetSceneCommandResult_AppendVertex( Key k, any_result & nNewVertexID );
    Key AppendSceneCommand_AppendTriangle( int nObjectID, vec3i & t );
    Key AppendSceneCommand_AppendTriangle( int nObjectID, vec3i & t, int nGroupID );
        bool GetSceneCommandResult_AppendTriangle( Key k, int & nNewTriID );
        bool GetSceneCommandResult_AppendTriangle( Key k, any_result & nNewTriID );

    Key AppendSceneCommand_AllocateNewGroupID( int nObjectID );
        bool GetSceneCommandResult_AllocateNewGroupID( Key k, int & nNewGroupID );
        bool GetSceneCommandResult_AllocateNewGroupID( Key k, any_result & nNewGroupID );



	/*
	 * SPATIAL QUERY COMMANDS
	 */

	Key AppendQueryCommand_ConvertScalarToWorld(float f);
		bool GetQueryResult_ConvertScalarToWorld(Key k, float & fResult);
	Key AppendQueryCommand_ConvertScalarToScene(float f);
		bool GetQueryResult_ConvertScalarToScene(Key k, float & fResult);
	Key AppendQueryCommand_ConvertPointToWorld(float fPoint[3]);
		bool GetQueryResult_ConvertPointToWorld(Key k, float fPoint[3]);
	Key AppendQueryCommand_ConvertPointToScene(float fPoint[3]);
		bool GetQueryResult_ConvertPointToScene(Key k, float fPoint[3]);

	// get bounding box of selected objects
	Key AppendQueryCommand_GetBoundingBox();
		bool GetQueryResult_GetBoundingBox( Key k, float fMin[3], float fMax[3] );

	// get bounding box of a specific object
	Key AppendQueryCommand_GetObjectBoundingBox( int nObjectID );
		bool GetQueryResult_GetObjectBoundingBox( Key k, float fMin[3], float fMax[3] );

	// get local frame of a specific object
	Key AppendQueryCommand_GetObjectLocalFrame( int nObjectID );
		bool GetQueryResult_GetObjectLocalFrame( Key k, frame3f * pFrame );

	// get bounding box and centroid of currently-selected faces (only works when in FaceSelection tool)
	Key AppendQueryCommand_GetSelectedFacesBoundingBox();
		bool GetQueryResult_GetSelectedFacesBoundingBox( Key k, float fMin[3], float fMax[3] );
	Key AppendQueryCommand_GetSelectedFacesCentroid();
		bool GetQueryResult_GetSelectedFacesCentroid( Key k, float fCentroid[3] );

	// find first ray-intersection point with selected object
	Key AppendQueryCommand_FindRayIntersection( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendQueryCommand_FindRayIntersection( const vec3f & o, const vec3f & d );
		bool GetQueryResult_FindRayIntersection( Key k, frame3f * pFrame );

	// find nearest point on selected object
	Key AppendQueryCommand_FindNearestPoint( float x, float y, float z );
	Key AppendQueryCommand_FindNearestPoint( const vec3f & p );
		bool GetQueryResult_FindNearestPoint( Key k, frame3f * pFrame );

	// test if point is inside selected object
	Key AppendQueryCommand_IsInsideObject( const vec3f & p );
		bool GetQueryResult_IsInsideObject( Key k );

	// nFilter is a bitmap that controls which 'types' of objects the queries below will return.
	// If no filter is set, or after clear, all object types are returned
	// bit 0 = meshes, bit 1 = pivots
	Key AppendQueryCommand_SetObjectTypeFilter( int nFilter );
	Key AppendQueryCommand_ClearObjectTypeFilter();

	// find all objects hit by ray. Results are returned sorted by ray-hit parameter.
	Key AppendQueryCommand_FindObjectsHitByRay( const vec3f & o, const vec3f & d );
		bool GetQueryResult_FindObjectsHitByRay( Key k, std::vector<int> & vObjects );

	// find closest object to point
	Key AppendQueryCommand_FindNearestObject( const vec3f & p );
		bool GetQueryResult_FindNearestObject( Key k, int & nObjectID );
		bool GetQueryResult_FindNearestObject( Key k, any_result & nObjectID );

	// find objects within distance of point.
	Key AppendQueryCommand_FindObjectsWithinDistance( const vec3f & p, float fDistance );
		bool GetQueryResult_FindObjectsWithinDistance( Key k, std::vector<int> & vObjects );

	// test if two objects intersect (requires that objects be meshes)
	Key AppendQueryCommand_TestIntersection( int nObjectID, int nTestWithObjectID );
		bool GetQueryResult_TestIntersection( Key k );
	
	// Find all objects intersecting first object
	Key AppendQueryCommand_FindIntersectingObjects( int nObjectID );
		bool GetQueryResult_FindIntersectingObjects( Key k, std::vector<int> & vObjects );

	// list number of holes in current object
	Key AppendQueryCommand_ListNumberOfHoles( );
		bool GetQueryResult_ListNumberOfHoles( Key k, int & nHoles );
		bool GetQueryResult_ListNumberOfHoles( Key k, any_result & nHoles );

	// get bounding box of hole
	Key AppendQueryCommand_FindClosestHole( const vec3f & p );
		bool GetQueryResult_FindClosestHole( Key k, int & nHoleID );
		bool GetQueryResult_FindClosestHole( Key k, any_result & nHoleID );

	// get bounding box of hole
	Key AppendQueryCommand_GetHoleBoundingBox( int nHoleID );
		bool GetQueryResult_GetHoleBoundingBox( Key k, float fMin[3], float fMax[3] );


	/*
	 *  SELECTION COMMANDS
	 *    Note: some selection commands take a mode argument which defines whether to replace/append/remove from the current selection
	 *    The modes are 0=Replace 1=Append 2=Remove
	 */

	void AppendSelectCommand_All( );

	// commands:
	//  "selectVisible"
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
	Key AppendSelectCommand_NearestComponent( float cx, float cy, float cz, int nMode );
	Key AppendSelectCommand_ContainingComponent( float cx, float cy, float cz );
	Key AppendSelectCommand_ContainingComponent( float cx, float cy, float cz, int nMode );

	// parameters are ray origin & direction
	Key AppendSelectCommand_FirstComponentIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_FirstComponentIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode );
	Key AppendSelectCommand_AllComponentsIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_AllComponentsIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode );

	// parameter is 3D point
	Key AppendSelectCommand_NearestTriangle( float cx, float cy, float cz );
	Key AppendSelectCommand_NearestTriangle( float cx, float cy, float cz, int nMode );

	// parameters are ray origin & direction
	Key AppendSelectCommand_FirstTriangleIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_FirstTriangleIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode );
	Key AppendSelectCommand_AllTrianglesIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz );
	Key AppendSelectCommand_AllTrianglesIntersectingRay( float ox, float oy, float oz, float dx, float dy, float dz, int nMode );

	// parameter is sphere center/radius
	Key AppendSelectCommand_InsideSphere( float cx, float cy, float cz, float r );
	Key AppendSelectCommand_InsideSphere( float cx, float cy, float cz, float r, int nMode );
		bool GetSelectCommandResult_InsideSphere( Key k );

	Key AppendSelectCommand_InsideBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax );
	Key AppendSelectCommand_InsideBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, int nMode );
		bool GetSelectCommandResult_InsideBox( Key k );

	Key AppendSelectCommand_IntersectingObject( int nObjectID, int nMode );

	Key AppendSelectCommand_ByFaceGroups( const std::vector<int> & vGroupIDs );
	Key AppendSelectCommand_ByFaceGroups( const std::vector<int> & vGroupIDs, int nMode );
		bool GetSelectCommandResult_ByFaceGroups( Key k );

	Key AppendSelectCommand_HoleBorderRing( int nHoleID, int nMode );
		bool GetSelectCommandResult_HoleBorderRing( Key k );

	// WARNING: this may overflow the command buffer over UDP sockets
	Key AppendSelectCommand_ListSelectedFaceGroups();
		bool GetSelectCommandResult_ListSelectedFaceGroups( Key k, std::vector<int> & vGroupIDs );

	// WARNING: this may overflow the command buffer over UDP sockets
	Key AppendSelectCommand_ListSelectedTriangles();
		bool GetSelectCommandResult_ListSelectedTriangles( Key k, std::vector<int> & vTriangleIDs );

	Key AppendSelectCommand_ByTriangleID( const std::vector<int> & vTriangles, int nMode );


	Key AppendSelectCommand_HasValidSelection();
		bool GetSelectCommandResult_HasValidSelection( Key k );

	

	/*
	 *  ACTION COMMANDS
	 */

	Key AppendActionCommand_BrushStroke3D( const std::vector<brush_stamp> & vPoints );
	Key AppendActionCommand_BrushStamp3D( const vec3f & v0 );
	Key AppendActionCommand_LinearBrushStroke3D( const vec3f & v0, const vec3f & v1, int nSteps );


	/* 
	 * Open/Solid Part Dropping.
	 *
	 * During interactive part drop you can use the parameters listed under [dropPart] 
	 * and [dropSolidPart] above in the AppendToolParameterCommand section. 
	 *
	 * Note that the partPath required below must be the full path to the part .obj file,
	 * typically will be like:
	 *     C:/Users/<username>/Documents/meshmixer/libraries/parts/default/<subdir>/part_file.obj
	 * The slash type doesn't matter, we handle that internally.
	 *
	 * The radius parameter is in world units. Pass as 0 to use the default auto-radius,
	 * and pass as -1.0 to use the "original" size, ie same as shift+drop in Meshmixer.
	 */
	Key AppendActionCommand_DropPartAtPoint( const char * pPartPath, const frame3f & f, float fRadiusWorld, bool bInteractive = false );
	Key AppendActionCommand_DropSolidPartAtPoint( const char * pPartPath, const frame3f & f, float fRadiusWorld, bool bInteractive = false );
	Key AppendActionCommand_UpdateDropPart( const frame3f & f, float fRadius, bool bMinimizeRotation );
	Key AppendActionCommand_AcceptDropPart( );
	// [RMS] for any of above, returns whether drop was successful or not. Only returns new GroupIDs when drop is completed
	bool GetActionCommandResult_DropPart( Key k, std::vector<int> & vNewGroups );


	// [RMS] TODO drop solid part (or also handle with above?)

	Key AppendActionCommand_InsertPolygon( float x, float y, float z, float fRadius );
	bool GetActionCommandResult_InsertPolygon( Key k, int & nNewGroupID );
	bool GetActionCommandResult_InsertPolygon( Key k, any_result & nNewGroupID );





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
		MouseEventCommand = 0,
		CameraControlCommand = 1,

		ToolParameterChangeCommand = 2,
		BeginToolCommand = 3,
		CompleteToolCommand = 4,
		ToolParameterCommand = 5,

		SceneCommand = 6,
		SelectCommand = 7,

		BrushCommand = 8,
		PartCommand = 9,
		StampCommand = 10,

		SpatialQueryCommand = 11,
		GenericQueryCommand = 12
	};


	enum MouseEventType {
		MouseDown = 0, MouseMove = 1, MouseUp = 2
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
		CamManip = 0, CamToggleSnap = 1, CamOrbit = 2, CamTurntable = 3, CamPan = 4, CamDolly = 5, CamRecenter = 6, CamSet = 7, CamQuery = 8, CamGetRay = 9, 
		SetViewNormalMode = 10, SetViewColorMode = 11, CamOrthographic = 12, CamPerspective = 13, 	
		SetShowWireframe = 14, SetShowBoundaries = 15, SetShowGrid = 16, SetShowPrinterBed = 17, SetTransparentTarget = 18,
		SetShader_Default = 19, SetShader_XRay = 20, SetShader_Texture = 21, SetShader_Texture_Unlit = 22, SetShader_UV = 23, SetShader_Overhang = 24,
		ShowObjectBrowser = 25, HideObjectBrowser = 26, TakeFocus = 27
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
		ClearScene = 0,
		AppendMeshFile = 1,
		OpenMixFile = 2,
		ExportMixFile = 3,
		ExportMeshFile_SelectedObjects = 4,
		ListObjects = 5,
		ListSelectedObjects = 6,
		SelectObjects = 7,
		SetAsTarget = 8,
		ClearTarget = 9,
		DeleteSelected = 10,
		GetObjectName = 11,
		SetObjectName = 12,
		FindObjectByName = 13,
		SaveScreenShot = 14,
		SetVisible = 15,
		SetHidden = 16,
		ShowAll = 17,
		AppendMeshFileAsReference = 18,
        CreatePivot = 19,
        LinkPivot = 20,
		AppendPackedMeshFile = 21,
		ExportAsPackedMeshByID = 22,

		CreateLiveMeshObject = 23,
		RequestLiveMeshLock = 24,
		ReleaseLiveMeshLock = 25,
		NotifyLiveMeshUpdated = 26,
		CreateTrackingLiveMesh = 27,
		HaltTrackingLiveMesh = 28,

		GetObjectType = 29,
		GetObjectFrame = 30,
		SetObjectFrame = 31,

		SelectPrinter = 32,


		CompactMeshObject = 33,
		GetVertexCount = 34,
		GetTriangleCount = 35,
		GetVertexPosition = 36,
		GetVertexNormal = 37,
		GetVertexColor = 38,
		GetTriangleIndices = 39,
		GetTriangleGroup = 40,
		SetVertexPosition = 41,
		SetVertexNormal = 42,
		SetVertexColor = 43,
		SetTriangleGroup = 44,
		AppendVertex = 45,
		AppendTriangle = 46,
		UpdateMesh = 47,
		AllocateNewGroupID = 48,
		GetVertexInfo = 49,

		CreateMeshObject = 50,
		GetObjectUUID = 51,
		FindObjectByUUID = 52,

		SetAllVertexColors = 53,
		ListFaceGroups = 54
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
		frame3f f;
	};



	enum SelectCmdType {
		SelectAll = 0,

		SelectNearestComponent = 1,
		SelectContainingComponent = 2,
		SelectFirstComponentIntersectingRay = 3,
		SelectAllComponentsIntersectingRay = 4,
		SelectNearestTriangle = 5,
		SelectFirstTriangleIntersectingRay = 6,
		SelectAllTrianglesIntersectingRay = 7,

		SelectInsideSphere = 8,
		SelectInsideBox = 9,
		SelectFaceGroups = 10,

		ListSelectedFaceGroups = 11,

		SelectUtility = 12,
		SelectIntersectingObject = 13,
		HasValidSelection = 14,
		SelectHoleBorderRing = 15,

		ListSelectedTriangles = 16,
		SelectTriangles = 17
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
		Stroke3D = 0
	};
	struct BrushCmd {
		BrushCmdType eType;
		vector_float vStamps;
	};



	enum PartCmdType {
		DropPart = 0, UpdatePart = 1, AcceptPart = 2, DropSolidPart = 3
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
		InsertPolygonStamp = 0
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
		SelectedObjectsBoundingBoxQuery = 0,
		NearestPointSpatialQuery = 1,
		RayIntersectionSpatialQuery = 2,
		SelectedFacesBoundingBoxQuery = 3,
		SelectedFacesCentroidQuery = 4,
		ObjectBoundingBoxQuery = 5,
		ObjectLocalFrameQuery = 6,
		RayHitObjectsQuery = 7,
		NearestObjectQuery = 8,
		ObjectsWithinRadiusQuery = 9,
		IsInsideObjectQuery = 10,
		ObjectIntersectionQuery = 11,
		FindIntersectingObjects = 12,
		SetObjectTypeFilter = 13,
		ClearObjectTypeFilter = 14,
		ListNumberOfHoles = 15,
		GetHoleBoundingBox = 16,
		FindClosestHole = 17
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
		ToolManager_NewGroups = 0,
		ToolManager_ScalarToWorld = 1,
		ToolManager_ScalarToScene = 2,
		ToolManager_PointToWorld = 3,
		ToolManager_PointToScene = 4,
		ToolManager_NewObjects = 5
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
