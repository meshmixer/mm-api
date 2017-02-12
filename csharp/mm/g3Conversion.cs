using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using g3;

namespace mm
{
    public static class g3Conversion
    {


        // [RMS] This doesn't work because I did something stupid when
        //   making unwrap meshes. The set of triangles aren't the same. argh.
        public static DMesh3 ConvertUnwrapToUVs(RemoteControl rc, out DenseMeshUVSet UVSet, string mainName = "main")
        {
            int mainID = rc.FindObjectByName(mainName);
            rc.CompactMesh(mainID);
            DMesh3 mainMesh = ReadMeshFromMM(rc, mainID);
            Debug.Assert(mainMesh.IsCompact);

            List<int> all = rc.ListObjects();
            List<int> unwrapIDs = new List<int>();
            foreach (int id in all) {
                string sName = rc.GetObjectName(id);
                if (sName.StartsWith("unwrap", StringComparison.OrdinalIgnoreCase))
                    unwrapIDs.Add(id);
            }
            int[] AllGroups = FaceGroupUtil.FindAllGroups(mainMesh).ToArray();


            Dictionary<int, DMesh3> Unwraps = new Dictionary<int, DMesh3>();

            foreach (int id in unwrapIDs) {
                DMesh3 mesh = ReadMeshFromMM(rc, id);
                HashSet<int> subGroups = FaceGroupUtil.FindAllGroups(mesh);
                Debug.Assert(subGroups.Count == 1);
                int gid = subGroups.ToArray()[0];
                Unwraps[gid] = mesh;
            }


            UVSet = new DenseMeshUVSet();
            UVSet.TriangleUVs.resize(mainMesh.TriangleCount);
            Dictionary<Index2i, int> UVSetMap = new Dictionary<Index2i, int>();

            Dictionary<int, int> GroupCounters = new Dictionary<int, int>();
            for (int i = 0; i < AllGroups.Length; ++i)
                GroupCounters[AllGroups[i]] = 0;

            for (int ti = 0; ti < mainMesh.TriangleCount; ++ti) {
                Index3i main_tri = mainMesh.GetTriangle(ti);
                int gid = mainMesh.GetTriangleGroup(ti);
                int sub_tid = GroupCounters[gid];
                GroupCounters[gid]++;

                DMesh3 SubMesh = Unwraps[gid];
                Index3i sub_tri = SubMesh.GetTriangle(sub_tid);

                for ( int j = 0; j < 3; ++j ) {
                    int sub_vid = sub_tri[j];
                    Index2i key = new Index2i(gid, sub_vid);
                    int uvset_idx;
                    if ( UVSetMap.TryGetValue(key, out uvset_idx) == false ) {
                        Vector3d v = SubMesh.GetVertex(sub_vid);
                        Vector2f uv = new Vector2f((float)v.x, (float)v.z);
                        uvset_idx = UVSet.AppendUV(uv);
                        UVSetMap[key] = uvset_idx;
                    }
                    sub_tri[j] = uvset_idx;
                }

                UVSet.TriangleUVs[ti] = sub_tri;
            }

            return mainMesh;
        }








        public static DMesh3 ReadMeshFromMM(RemoteControl rc, int nObjectId, bool bWantColors = false)
        {
            Debug.Assert(bWantColors == false); // haven't implemented yet

            DMesh3 mesh = new DMesh3(MeshComponents.FaceGroups);

            int NV = rc.GetVertexCount(nObjectId);
            int nBatchSize = 750;
            int nFullBatches = NV / nBatchSize;
            for ( int i = 0; i < nFullBatches; ++i ) {
                Vector3f[] vPositions = rc.GetVertexPositionInRange(nObjectId, i * nBatchSize, nBatchSize);
                for ( int j = 0; j < vPositions.Length; ++j ) {
                    int vid = mesh.AppendVertex( (Vector3d)vPositions[j] );
                    Debug.Assert(vid == i * nBatchSize + j);
                }
            }
            int nLeft = NV - nFullBatches * nBatchSize;
            if ( nLeft > 0 ) {
                Vector3f[] vPositions = rc.GetVertexPositionInRange(nObjectId, nFullBatches * nBatchSize, nLeft);
                for ( int j = 0; j < nLeft; ++j ) {
                    int vid = mesh.AppendVertex( (Vector3d)vPositions[j] );
                }
            }

            int NT = rc.GetTriangleCount(nObjectId);
            nBatchSize = 750;
            nFullBatches = NT / nBatchSize;
            for ( int i = 0; i < nFullBatches; ++i ) {
                Index3i[] vTriangles = rc.GetTrianglesInRange(nObjectId, i * nBatchSize, nBatchSize);
                int[] vGroups = rc.GetFaceGroupsInRange(nObjectId, i * nBatchSize, nBatchSize);
                for ( int j = 0; j < vTriangles.Length; ++j ) {
                    int vid = mesh.AppendTriangle( vTriangles[j], vGroups[j] );
                    Debug.Assert(vid == i * nBatchSize + j);
                }
            }
            nLeft = NT - nFullBatches * nBatchSize;
            if ( nLeft > 0 ) {
                Index3i[] vTriangles = rc.GetTrianglesInRange(nObjectId, nFullBatches * nBatchSize, nLeft);
                int[] vGroups = rc.GetFaceGroupsInRange(nObjectId, nFullBatches * nBatchSize, nLeft);
                for ( int j = 0; j < nLeft; ++j ) {
                    int vid = mesh.AppendTriangle( vTriangles[j], vGroups[j] );
                }
            }

            return mesh;
        }




    }
}
