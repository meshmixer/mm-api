.. _tool-params-label:

Tool Parameters
===============
* **vector3f** is a 3-float list
* **matrix3f** is a 9-float3x3 row-major matrix


select
------
========= ======= 
size      float   
brushType integer 
symmetry  boolean 
========= ======= 

volumeBrush
-----------
================== ======= ============
strength           float   range [0,1] 
size               float               
depth              float   range [-1,1]
lazyness           float   range [0,1] 
refine             float   range [0,1] 
reduce             float   range [0,1] 
refineSmooth       float   range [0,1] 
attractStrength    float               
volumetric         boolean             
symmetric          boolean             
surfSnap           boolean             
continuous         boolean             
enableRefinement   boolean             
restrictToGroup    boolean             
holdBoundary       boolean             
preserveGroups     boolean             
brushOnTarget      boolean             
adaptiveRefinement boolean             
================== ======= ============

surfaceBrush
------------
=============== ======= 
restrictToGroup boolean 
holdBoundary    boolean 
preserveGroups  boolean 
=============== ======= 

stamp
-----
============= ======= 
snapDimension boolean 
============= ======= 

erase
-----
============== ======= =======================================================================
density        float                                                                          
smooth         float                                                                          
scale          float                                                                          
boundaryRotate float                                                                          
replaceType    integer values:  MinimalFill = 0, FlatRefinedMesh = 1, MVCPlanarDeformation = 2
============== ======= =======================================================================

discard
-------
  
  

reduce
------
================= ======= ====================================================
percentage        float                                                       
maxDeviation      float                                                       
maxDeviationWorld float                                                       
smooth            float                                                       
normalThreshold   float                                                       
goalType          integer Percentage = 0, TriangleBudget = 1,	MaxDeviation = 2
triangleCount     integer                                                     
reduceMetric      integer Uniform = 0, QuadricError = 1                       
preserveGroups    boolean                                                     
preserveBoundary  boolean                                                     
adaptive          boolean                                                     
================= ======= ====================================================

remesh
------
=============== ======= =========================================
density         float                                            
smooth          float                                            
normalThreshold float                                            
boundaryMode    integer                                          
remeshType      integer values: Uniform = 0,	 Adaptive_Normal = 1
preserveGroups  boolean                                          
adaptive        boolean                                          
=============== ======= =========================================

extrude
-------
============= ======= =================================================================
offset        float   range [-inf, inf]                                                
harden        float   range [0,1]                                                      
density       float   range [0,1]                                                      
endType       integer values: Offset = 0, Flat = 1                                     
directionType integer values: Normal = 0, Constant = 1, XAxis = 2, YAxis = 3,	ZAxis = 4
============= ======= =================================================================

extract
-------
============= ======= ==================================================================================================================
offset        float                                                                                                                     
endType       integer values: OffsetDistance = 0,	 PlanarFlat = 1                                                                       
directionType integer values: NormalDirection = 0,	 ConstantDirection = 1,	 UnitXDirection = 2,	 UnitYDirection = 3,	 UnitZDirection = 4
============= ======= ==================================================================================================================

offset
------
============= ======= 
offset        float   
offsetWorld   float   
soften        float   
softenWorld   float   
connected     boolean 
fixedBoundary boolean 
============= ======= 

handle
------
====== ======= 
smooth float   
refine float   
flip   boolean 
====== ======= 

bridge
------
====== ===== 
refine float 
====== ===== 

join
----
  
  

weldBoundaries
--------------
  
  

separate
--------
  
  

planeCut
--------
======== ======== =======================================================================================================================
cutType  integer  values: 	PreciseGeometricCut = 0, PreciseGeometricSlice = 1,	NearestPointAttach = 2,	RefinedICPAttach = 3   (default=0)
fillType integer  values: 	NoFill = 0,	CentroidRefinedFill = 1, DelaunayFill = 2,	DelaunayRefinedFill = 3  (default=3)                   
hardEdge boolean                                                                                                                         
origin   vector3f                                                                                                                        
normal   vector3f                                                                                                                        
rotation matrix3f                                                                                                                        
======== ======== =======================================================================================================================

attractToTarget
---------------
================ ======= 
density          float   
smooth           float   
offset           float   
offsetWorld      float   
normalThreshold  float   
remeshType       integer 
preserveGroups   boolean 
preserveBoundary boolean 
findSharpEdges   boolean 
adaptive         boolean 
================ ======= 

flipNormals
-----------
  
  

fitPrimitive
------------
================ ======= 
primitiveType    integer 
singlePrimitive  boolean 
createNewObjects boolean 
================ ======= 

makePart
--------
  
  

makeSolidPart
-------------
  
  

makePolygon
-----------
  
  

smooth
------
========== ======= 
smooth     float   
scale      float   
reduceType integer 
========== ======= 

faceTransform
-------------
============== ======== 
harden         float    
pivotFrameMode integer  
sharedFrame    boolean  
origin         vector3f 
translation    vector3f 
scale          vector3f 
rotation       matrix3f 
============== ======== 

softTransform
-------------
==================== ======== 
falloffDistance      float    
falloffDistanceWorld float    
pivotFrameMode       integer  
boundaryContinuity   integer  
transitionType       integer  
sharedFrame          boolean  
refineMesh           boolean  
holdBoundary         boolean  
falloffFixed         boolean  
origin               vector3f 
translation          vector3f 
scale                vector3f 
rotation             matrix3f 
==================== ======== 

warp
----
  
  

createFaceGroup
---------------
  
  

clearFaceGroup
--------------
  
  

smoothBoundary
--------------
================ ======= 
smoothness       float   
preserveShape    float   
iterations       integer 
expandLoops      integer 
preserveBoundary boolean 
================ ======= 

mirror
------
======== ======== 
origin   vector3f 
normal   vector3f 
rotation matrix3f 
======== ======== 

duplicate
---------
  
  

transform
---------
============== ======== 
pivotFrameMode integer  
origin         vector3f 
translation    vector3f 
scale          vector3f 
rotation       matrix3f 
============== ======== 

align
-----
  
  

closeCracks
-----------
  
  

generateFaceGroups
------------------
============== ===== 
angleThreshold float 
============== ===== 

makeSolid
---------
=================== ======= 
offsetDistance      float   
offsetDistanceWorld float   
minThickness        float   
minThicknessWorld   float   
edgeCollapseThresh  float   
solidType           integer 
solidResolution     integer 
meshResolution      integer 
closeHoles          boolean 
transferFaceGroups  boolean 
=================== ======= 

hollow
------
=================== ======= 
offsetDistance      float   
offsetDistanceWorld float   
holeRadiusWorld     float   
holeTaperWorld      float   
hollowType          integer 
solidResolution     integer 
meshResolution      integer 
holesPerComponent   integer 
=================== ======= 

makePattern
-----------
=================== ======== 
gridResolutionScale float    
meshResolutionScale float    
dimension1          float    
spacing1            float    
offsetDistance      float    
offsetDistanceWorld float    
edgeCollapseThresh  float    
gradientValue0      float    
gradientValue1      float    
pattern             integer  
tiling              integer  
compositionMode     integer  
smoothingIters      integer  
dimensionGradient   integer  
clipToSurface       boolean  
gradientPosition0   vector3f 
gradientPosition1   vector3f 
origin              vector3f 
translation         vector3f 
rotation            matrix3f 
=================== ======== 

makeSlices
----------
  
  

separateShells
--------------
  
  

addTube
-------
==================== ======= 
startRadius          float   
startRadiusWorld     float   
endRadius            float   
endRadiusWorld       float   
startTanLenFactor    float   
lengthScale          float   
collisionRadius      float   
collisionRadiusWorld float   
curveType            integer 
operationType        integer 
directionConstraint  integer 
solveIterations      integer 
==================== ======= 

combine
-------
  
  

union
-----
  
  

difference
----------
  
  

intersection
------------
  
  

inspector
---------
======================= ======= 
smallComponentThreshold float   
replaceType             integer 
======================= ======= 

units
-----
====== ===== 
worldX float 
worldY float 
worldZ float 
====== ===== 

measure
-------
  
  

stability
---------
================ ===== 
contactTolerance float 
================ ===== 

strength
--------
============ ======= 
showSections boolean 
============ ======= 

overhangs
---------
====================== ======= 
overhangPreset         integer 
overhangAngleTolerance float   
contactTolerance       float   
verticalOffset         float   
maxDraftAngle          float   
density                float   
layerHeight            float   
postBaseSize           float   
postTopSize            float   
postTipSize            float   
postTipLayers          float   
postDiscSize           float   
postDiscLayers         float   
strutDensity           float   
solidMinOffset         float   
postResolution         integer 
optimizeRounds         integer 
allowTopSupport        boolean 
====================== ======= 

slicing
-------
============== ===== 
sliceHeight    float 
minFeatureSize float 
============== ===== 

thickness
---------
================= ===== 
minThickness      float 
minThicknessWorld float 
================= ===== 

orientation
-----------
======================== ======== 
overhangAngleDeg         float    
strengthWeight           float    
supportVolumeWeight      float    
supportSurfaceAreaWeight float    
selectedOrientation      integer  
origin                   vector3f 
translation              vector3f 
rotation                 matrix3f 
======================== ======== 

layout
------
================== ======= ================================================
borderWidth        float                                                   
transformationType integer Translate2D=0, Translate2D_Rotate2D=1           
packingQuality     integer Fastest = 0, Medium_Quality = 1, Max_Quality = 2
bedOrigin          integer RearLeftCorner = 0, Centered = 1                
packMetric         integer Circular=0, Square=2, Left-to-Right=3           
================== ======= ================================================

deviation
---------
================= ===== 
maxDeviationWorld float 
================= ===== 
