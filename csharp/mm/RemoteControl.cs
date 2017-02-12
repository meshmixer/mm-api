using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using g3;

namespace mm
{
    public class RemoteControl
    {
        protected IPEndPoint m_remoteIP;
        protected IPEndPoint m_listenIP;
        protected UdpClient m_pSendSock;
        protected UdpClient m_pReceiveSock;
        protected bool m_bConnected;

        public RemoteControl()
        {
            m_bConnected = false;
        }

        public bool Initialize()
        {
            m_remoteIP = new IPEndPoint(IPAddress.Loopback, 0xAFCF);
            m_listenIP = new IPEndPoint(IPAddress.Loopback, 0xAFDF);

            m_pSendSock = new UdpClient( new IPEndPoint(IPAddress.Any, 0) );

            m_pReceiveSock = new UdpClient(  );
            m_pReceiveSock.ExclusiveAddressUse = true;
            m_pReceiveSock.Client.SetSocketOption(
                SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, false);
            m_pReceiveSock.Client.Bind(m_listenIP);

            // [TODO] check for errors...
            m_bConnected = true;

            return true;
        }
        public void Shutdown()
        {
            m_pSendSock.Close();
            m_pReceiveSock.Close();
        }

        public bool Connected
        {
            get
            {
                return m_bConnected;
            }
        }

        public bool ExecuteCommands(StoredCommands cmd)
        {
            BinarySerializer serializer = new BinarySerializer();
            cmd.Store(serializer);
            vectorub commandBuf = serializer.buffer();

            byte[] sendBuf = commandBuf.ToArray<byte>();
            int nSent = m_pSendSock.Send(sendBuf, sendBuf.Length, m_remoteIP);

            IPEndPoint remoteIP = new IPEndPoint(IPAddress.Any, 0);
            byte[] receiveBuf = m_pReceiveSock.Receive(ref remoteIP);

            vectorub resultBuf = new vectorub(receiveBuf);
            serializer.setBuffer(resultBuf);
            cmd.Restore_Results(serializer);

            return true;
        }



        public bool TestConnection()
        {
            StoredCommands cmd = new StoredCommands();
            cmd.AppendSceneCommand_ListSelectedObjects();
            BinarySerializer serializer = new BinarySerializer();
            cmd.Store(serializer);
            vectorub commandBuf = serializer.buffer();
            byte[] sendBuf = commandBuf.ToArray<byte>();
            int nSent = m_pSendSock.Send(sendBuf, sendBuf.Length, m_remoteIP);

            int nSaveTimeout = m_pReceiveSock.Client.ReceiveTimeout;
            m_pReceiveSock.Client.ReceiveTimeout = 200;
            bool bReceiveOK = true;

            IPEndPoint remoteIP = new IPEndPoint(IPAddress.Any, 0);
            try {
                byte[] receiveBuf = m_pReceiveSock.Receive(ref remoteIP);
                bReceiveOK = true;
            } catch (SocketException) {
                bReceiveOK = false;
            }
            m_pReceiveSock.Client.ReceiveTimeout = nSaveTimeout;

            return bReceiveOK;
        }



        private string to_string(vectorub vec)
        {
            byte[] bytes = vec.ToArray();
            Array.Resize(ref bytes, bytes.Length - 1);
            return System.Text.Encoding.ASCII.GetString(bytes);
        }
        public frame3f WorldFrame()
        {
            frame3f f = new frame3f();
            Vector3f vOriginS = ToScene(new Vector3f(0, 0, 0));
            f.origin_x = vOriginS[0]; f.origin_y = vOriginS[1]; f.origin_z = vOriginS[2];

            f.tan1_x = 1.0f; f.tan1_y = 0.0f; f.tan1_z = 0.0f;
            f.tan2_x = 0.0f; f.tan2_y = 0.0f; f.tan2_z = 1.0f;
            f.normal_x = 0.0f; f.normal_y = 1.0f; f.normal_z = 0.0f;

            return f;
        }




        public void SaveScene(string sMixPath)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_ExportMixFile(sMixPath);
            ExecuteCommands(sc);
        }


        public void ClearScene()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_Clear();
            ExecuteCommands(sc);

            System.Threading.Thread.Sleep(500);     // [RMS] MM crashes sometimes...
        }

        public List<int> OpenMixFile(string sPath)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_OpenMixFile(sPath);
            ExecuteCommands(sc);
            return ListObjects();
        }
        public List<int> ImportFile(string sPath)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_AppendMeshFile(sPath);
            ExecuteCommands(sc);
            vectori objs = new vectori();
            sc.GetSceneCommandResult_AppendMeshFile(key, objs);
            return objs.ToList();
        }
        public void ExportSelectedObject(string sPath)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_ExportMeshFile_CurrentSelection(sPath);
            ExecuteCommands(sc);
        }


        public string GetObjectName(int id)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetObjectName(id);
            ExecuteCommands(sc);
            vectorub name = new vectorub();
            bool bFound = sc.GetSceneCommandResult_GetObjectName(key, name);
            return (bFound) ? to_string(name) : null;
        }
        public void SetObjectName(int id, string newName)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_SetObjectName(id, newName);
            ExecuteCommands(sc);
        }
        public int FindObjectByName(string name)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_FindObjectByName(name);
            ExecuteCommands(sc);
            any_result r = new any_result();
            bool bOK = sc.GetSceneCommandResult_FindObjectByName(key, r);
            if (bOK)
                return r.i;
            else
                return -1;
        }



        public mmFrame GetObjectFrame(int id)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetObjectFrame(id);
            ExecuteCommands(sc);
            frame3f f = new frame3f();
            sc.GetSceneCommandResult_GetObjectFrame(key, f);
            return new mmFrame(f);
        }
        public void SetObjectFrame(int id, mmFrame newFrame)
        {
            frame3f f = newFrame.toFrame3f();
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_SetObjectFrame(id, f);
            ExecuteCommands(sc);
        }

        public int GetObjectType(int id)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetObjectType(id);
            ExecuteCommands(sc);
            any_result a = new any_result();
            bool bFound = sc.GetSceneCommandResult_GetObjectType(key, a);
            return (bFound) ? a.i : -1;
        }

        public void SelectObjects(List<int> vObjects)
        {
            StoredCommands sc = new StoredCommands();
            vectori objs = new vectori(vObjects);
            sc.AppendSceneCommand_SelectObjects(objs);
            ExecuteCommands(sc);
        }

        public void DeleteSelectedObjects()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_DeleteSelectedObjects();
            ExecuteCommands(sc);
        }



        public List<int> ListObjects()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_ListObjects();
            ExecuteCommands(sc);
            vectori newobjs = new vectori();
            sc.GetSceneCommandResult_ListObjects(key, newobjs);
            return newobjs.ToList();
        }
        public List<int> ListSelectedObjects()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_ListSelectedObjects();
            ExecuteCommands(sc);
            vectori newobjs = new vectori();
            sc.GetSceneCommandResult_ListSelectedObjects(key, newobjs);
            return newobjs.ToList();
        }
        public List<int> ListNewObjects()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendToolQuery_NewObjects();
            ExecuteCommands(sc);
            vectori newobjs = new vectori();
            sc.GetToolQueryResult_NewObjects(key, newobjs);
            return newobjs.ToList();
        }
        public List<int> ListNewFaceGroups()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendToolQuery_NewGroups();
            ExecuteCommands(sc);
            vectori newobjs = new vectori();
            sc.GetToolQueryResult_NewGroups(key, newobjs);
            return newobjs.ToList();
        }

        //public int GetObjectType(int nObjectID)
        //{
        //    StoredCommands sc = new StoredCommands();
        //    uint key = sc.AppendSceneCommand_GetObjectType(nObjectID);
        //    ExecuteCommands(sc);
        //    sc.GetSceneCommandResult_GetObjectType(key,  )
        //}

        public void SetVisible(int nObjectID, bool bVisible)
        {
            StoredCommands sc = new StoredCommands();
            if (bVisible)
                sc.AppendSceneCommand_SetVisible(nObjectID);
            else
                sc.AppendSceneCommand_SetHidden(nObjectID);
            ExecuteCommands(sc);
        }




        public int CreatePivot()
        {
            StoredCommands sc = new StoredCommands();
            frame3f f = WorldFrame();
            uint key = sc.AppendSceneCommand_CreatePivot(f);
            ExecuteCommands(sc);

            any_result r = new any_result();
            sc.GetSceneCommandResult_CreatePivot(key, r);
            return r.i;
        }
        public void LinkPivot(int pivotID, int targetID)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_LinkPivot(pivotID, targetID);
            ExecuteCommands(sc);
        }


        public void BeginTool(string toolName) {
            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand(toolName);
            ExecuteCommands(sc);
        }
        public void CompleteTool(string result) {
            StoredCommands sc = new StoredCommands();
            sc.AppendCompleteToolCommand(result);
            ExecuteCommands(sc);
        }
        public void AcceptTool()
            { CompleteTool("accept"); }
        public void CancelTool()
            { CompleteTool("cancel"); }


        public void SetToolParameter(string paramName, float f) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, f);
            ExecuteCommands(sc);
        }
        public void SetToolParameter(string paramName, int n) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, n);
            ExecuteCommands(sc);
        }
        public void SetToolParameter(string paramName, bool b) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, b);
            ExecuteCommands(sc);
        }
        public void SetToolParameter(string paramName, Vector3f v)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, v[0], v[1], v[2]);
            ExecuteCommands(sc);
        }
        public void SetToolParameter(string paramName, mmMatrix m)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
            ExecuteCommands(sc);
        }

        private bool AppendToolParameters(StoredCommands sc, Dictionary<string, object> parameters)
        {
            int nValid = 0;
            foreach (KeyValuePair<string, object> entry in parameters) {
                Type t = entry.Value.GetType();
                if (t.Equals(typeof(float))) {
                    sc.AppendToolParameterCommand(entry.Key, (float)entry.Value);
                    nValid++;
                } else if (t.Equals(typeof(int))) {
                    sc.AppendToolParameterCommand(entry.Key, (int)entry.Value);
                    nValid++;
                } else if (t.Equals(typeof(bool))) {
                    sc.AppendToolParameterCommand(entry.Key, (bool)entry.Value);
                    nValid++;
                } else if (t.Equals(typeof(Vector3f))) {
                    Vector3f v = (Vector3f)entry.Value;
                    sc.AppendToolParameterCommand(entry.Key, v[0], v[1], v[2]);
                    nValid++;
                }
            }
            return (nValid > 0);
        }
        public void SetToolParameters(Dictionary<string, object> parameters)
        {
            StoredCommands sc = new StoredCommands();
            if ( AppendToolParameters(sc, parameters) )
                ExecuteCommands(sc);
        }
        public void BeginTool(string toolName, Dictionary<string, object> parameters)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand(toolName);
            AppendToolParameters(sc, parameters);
            ExecuteCommands(sc);
        }


        public bool GetToolParameter(string paramName, ref any_result r) {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendGetToolParameterCommand(paramName);
            ExecuteCommands(sc);
            return sc.GetToolParameterCommandResult(key, r);
        }
        public bool GetToolParameter(string paramName, ref float f) {
            any_result r = new any_result();
            bool bOK = GetToolParameter(paramName, ref r);
            f = r.f;
            return bOK;
        }
        public bool GetToolParameter(string paramName, ref int n) {
            any_result r = new any_result();
            bool bOK = GetToolParameter(paramName, ref r);
            n = r.i;
            return bOK;
        }
        public bool GetToolParameter(string paramName, ref bool b) {
            any_result r = new any_result();
            bool bOK = GetToolParameter(paramName, ref r);
            b = r.b;
            return bOK;
        }
        public bool GetToolParameter(string paramName, ref Vector3f v)
        {
            any_result r = new any_result();
            bool bOK = GetToolParameter(paramName, ref r);
            v[0] = r.x; v[1] = r.y; v[2] = r.z;
            return bOK;
        }



        public void ToolUtilityCommand(string sCommand) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolUtilityCommand(sCommand);
            ExecuteCommands(sc);
        }
        public void ToolUtilityCommand(string sCommand, int nValue) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolUtilityCommand(sCommand, nValue);
            ExecuteCommands(sc);
        }
        public void ToolUtilityCommand(string sCommand, string sValue) {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolUtilityCommand(sCommand, sValue);
            ExecuteCommands(sc);
        }



        public void SelectAll()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectCommand_All();
            ExecuteCommands(sc);
        }
        public void InvertSelection()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectUtilityCommand("invert");
            ExecuteCommands(sc);
        }
        public void DeleteSelection()
        {
            BeginTool("discard");
            AcceptTool();
        }
        public void ExpandSelectionToConnected()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectUtilityCommand("expandToConnected");
            ExecuteCommands(sc);
        }
        public void ExpandSelectionToGroups()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectUtilityCommand("expandToGroups");
            ExecuteCommands(sc);
        }
        public void ExpandSelectionByRings(int nRings)
        {
            StoredCommands sc = new StoredCommands();
            for ( int k = 0; k < nRings; k++)
                sc.AppendSelectUtilityCommand("expandByOneRing");
            ExecuteCommands(sc);
        }
        public void SelectFaceGroups(List<int> vGroups)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectCommand_ByFaceGroups(new vectori(vGroups));
            ExecuteCommands(sc);
        }
        public void OptimizeSelectionBoundary()
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectUtilityCommand("optimizeBoundary");
            ExecuteCommands(sc);
        }

        public void SelectInsideBox(Vector3f min, Vector3f max)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectCommand_InsideBox(min[0], max[0], min[1], max[1], min[2], max[2]);
            ExecuteCommands(sc);
        }

        public void SelectRayHitTriangle(Vector3f o, Vector3f d)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSelectCommand_FirstTriangleIntersectingRay(o[0], o[1], o[2], d[0], d[1], d[2]);
            ExecuteCommands(sc);
        }

        public void SelectHoleBorder(int nHole, int nMode = 1 )
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSelectCommand_HoleBorderRing(nHole, nMode);
            ExecuteCommands(sc);
        }


        public List<int> ListSelectedFaceGroups()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSelectCommand_ListSelectedFaceGroups();
            ExecuteCommands(sc);
            vectori tmp = new vectori();
            sc.GetSelectCommandResult_ListSelectedFaceGroups(key, tmp);
            return tmp.ToList();
        }

        public void SelectedFacesBoundingBox(ref AxisAlignedBox3f box)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_GetSelectedFacesBoundingBox();
            ExecuteCommands(sc);
            floatArray min = new floatArray(3), max = new floatArray(3);
            sc.GetQueryResult_GetSelectedFacesBoundingBox(key, min.cast(), max.cast());
            box.Min.Set(min); box.Max.Set(max);
        }
        public Vector3f SelectedFacesCentroid()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_GetSelectedFacesCentroid();
            ExecuteCommands(sc);
            floatArray c = new floatArray(3);
            sc.GetQueryResult_GetSelectedFacesCentroid(key, c.cast());
            return g3Extensions.ToVector3f(c);
        }


        public int ListNumberOfHoles()
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_ListNumberOfHoles();
            ExecuteCommands(sc);
            any_result r = new any_result();
            sc.GetQueryResult_ListNumberOfHoles(key, r);
            return r.i;
        }



        public mmFrame NearestSurfacePoint(Vector3f queryPt)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_FindNearestPoint(queryPt[0], queryPt[1], queryPt[2]);
            ExecuteCommands(sc);

            frame3f f = new frame3f();
            sc.GetQueryResult_FindNearestPoint(key, f);
            return new mmFrame(f);
        }



        public float ToWorld(float fScene) {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_ConvertScalarToWorld(fScene);
            ExecuteCommands(sc);
            floatArray f = new floatArray(1);
            sc.GetQueryResult_ConvertScalarToWorld(key, f.cast());
            return f.getitem(0);
        }
        public float ToScene(float fWorld) {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_ConvertScalarToScene(fWorld);
            ExecuteCommands(sc);
            floatArray f = new floatArray(1);
            sc.GetQueryResult_ConvertScalarToScene(key, f.cast());
            return f.getitem(0);
        }

        public Vector3f ToWorld(Vector3f vScene) {
            StoredCommands sc = new StoredCommands();
            floatArray f = vScene.toFloatArray();
            uint key = sc.AppendQueryCommand_ConvertPointToWorld(f.cast());
            ExecuteCommands(sc);
            sc.GetQueryResult_ConvertPointToWorld(key, f.cast());
            return g3Extensions.ToVector3f(f);
        }
        public Vector3f ToScene(Vector3f vWorld) {
            StoredCommands sc = new StoredCommands();
            floatArray f = vWorld.toFloatArray();
            uint key = sc.AppendQueryCommand_ConvertPointToScene(f.cast());
            ExecuteCommands(sc);
            sc.GetQueryResult_ConvertPointToScene(key, f.cast());
            return g3Extensions.ToVector3f(f);
        }


        public void ToWorld(Vector3f[] vScenePoints, int nBatchSize)
        {
            int nBatches = vScenePoints.Length / nBatchSize;
            uint[] keys = new uint[nBatchSize];

            for (int j = 0; j <= nBatches; ++j) {
                StoredCommands sc = new StoredCommands();
                int iStart = j * nBatchSize;
                int iUseSize = (j == nBatches) ? (vScenePoints.Length - nBatches * nBatchSize) : nBatchSize;
                for (int k = 0; k < iUseSize; ++k) {
                    if (vScenePoints[iStart + k] != Vector3f.Invalid) {
                        floatArray f = vScenePoints[iStart + k].toFloatArray();
                        keys[k] = sc.AppendQueryCommand_ConvertPointToWorld(f.cast());
                    }
                }
                ExecuteCommands(sc);
                for (int k = 0; k < iUseSize; ++k) {
                    if (vScenePoints[iStart + k] != Vector3f.Invalid) {
                        floatArray f = vScenePoints[iStart + k].toFloatArray();
                        sc.GetQueryResult_ConvertPointToScene(keys[k], f.cast());
                        vScenePoints[iStart+k] = g3Extensions.ToVector3f(f);
                    }
                }
            }
        }


        public void SetView(Vector3f vPosition, Vector3f vTarget, Vector3f vUp)
        {
            StoredCommands sc = new StoredCommands();
            sc.CameraControl_Begin();
            sc.CameraControl_SetSpecificView(vPosition.toVec3f(), vTarget.toVec3f(), vUp.toVec3f());
            sc.CameraControl_End();
            ExecuteCommands(sc);
        }
        public void RecenterCamera()
        {
            StoredCommands sc = new StoredCommands();
            sc.CameraControl_RecenterView();
            ExecuteCommands(sc);
        }
        public void TakeFocus()
        {
            StoredCommands sc = new StoredCommands();
            sc.ViewControl_TakeFocus();
            ExecuteCommands(sc);
        }
        public void SetColorMode(int nMode)
        {
            StoredCommands sc = new StoredCommands();
            sc.ViewControl_SetTriangleColorMode(nMode);
            ExecuteCommands(sc);
        }




        public bool FindRayIntersection(Vector3f o, Vector3f d, ref Vector3f vHit)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_FindRayIntersection(o[0], o[1], o[2], d[0], d[1], d[2]);
            ExecuteCommands(sc);
            frame3f frame = new frame3f();
            bool bHit = sc.GetQueryResult_FindRayIntersection(key, frame);
            if (bHit)
                vHit = new Vector3f(frame.origin_x, frame.origin_y, frame.origin_z);
            return bHit;
        }

        public void ObjectBoundingBox(int objectID, ref AxisAlignedBox3f box)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_GetObjectBoundingBox(objectID);
            ExecuteCommands(sc);
            floatArray min = new floatArray(3), max = new floatArray(3);
            sc.GetQueryResult_GetBoundingBox(key, min.cast(), max.cast());
            box.Min.Set(min); box.Max.Set(max);
        }


        public void SelectionBoundingBox(ref AxisAlignedBox3f box)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_GetBoundingBox();
            ExecuteCommands(sc);
            floatArray min = new floatArray(3), max = new floatArray(3);
            sc.GetQueryResult_GetBoundingBox(key, min.cast(), max.cast());
            box.Min.Set(min); box.Max.Set(max);
        }



        public void CompactMesh(int nObjectID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_CompactMesh(nObjectID);
            ExecuteCommands(sc);
        }
        public void UpdateMesh(int nObjectID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_UpdateMesh(nObjectID);
            ExecuteCommands(sc);
        }


        public int GetVertexCount(int nObjectID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetVertexCount(nObjectID);
            ExecuteCommands(sc);
            any_result r = new any_result();
            bool bOK = sc.GetSceneCommandResult_GetVertexCount(key, r);
            return (bOK) ? r.i : -1;
        }


        public Vector3f GetVertexPosition(int nObjectID, int nVertexID)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendSceneCommand_GetVertexPosition(nObjectID, nVertexID);
            ExecuteCommands(sc);
            vec3f v = new vec3f();
            bool bOK = sc.GetSceneCommandResult_GetVertexPosition(key, v);
            return (bOK) ? g3Extensions.ToVector3f(v) : Vector3f.Invalid;
        }

        public Vector3f[] GetVertexPositionInRange(int nObjectID, int nVertexIDStart, int nCount)
        {
            StoredCommands sc = new StoredCommands();
            uint[] keys = new uint[nCount];
            for ( int k = 0; k < nCount; ++k)
                keys[k] = sc.AppendSceneCommand_GetVertexPosition(nObjectID, nVertexIDStart+k);
            ExecuteCommands(sc);
            Vector3f[] vPositions = new Vector3f[nCount];
            for (int k = 0; k < nCount; ++k) {
                vec3f v = new vec3f();
                bool bOK = sc.GetSceneCommandResult_GetVertexPosition(keys[k], v);
                vPositions[k] = (bOK) ? g3Extensions.ToVector3f(v) : Vector3f.Invalid;
            }
            return vPositions;
        }



        public void SetVertexColor(int nObjectID, int nVertexID, Vector3f vColor)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendSceneCommand_SetVertexColor(nObjectID, nVertexID, vColor.toVec3f());
            ExecuteCommands(sc);
        }
        public void SetVertexColors(int nObjectID, Vector3f[] vColors, int nBatchSize)
        {
            int nBatches = vColors.Length / nBatchSize;
            for (int j = 0; j <= nBatches; ++j) {
                StoredCommands sc = new StoredCommands();
                int iStart = j * nBatchSize;
                int iUseSize = (j == nBatches) ? (vColors.Length - nBatches * nBatchSize) : nBatchSize;
                for (int k = 0; k < iUseSize; ++k) {
                    if (vColors[iStart + k] != Vector3f.Invalid)
                        sc.AppendSceneCommand_SetVertexColor(nObjectID, iStart + k, vColors[iStart + k].toVec3f());
                }
                ExecuteCommands(sc);
            }
        }

    }
}
