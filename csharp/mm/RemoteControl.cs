using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace mm
{
    public class RemoteControl
    {
        protected IPEndPoint m_remoteIP;
        protected IPEndPoint m_listenIP;
        protected UdpClient m_pSendSock;
        protected UdpClient m_pReceiveSock;

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

            return true;
        }
        public void Shutdown()
        {
            m_pSendSock.Close();
            m_pReceiveSock.Close();
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



        public void BeginTool(string toolName) {
            StoredCommands sc = new StoredCommands();
            sc.AppendBeginToolCommand("volumeBrush");
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
        public void SetToolParameter(string paramName, Vector3 v)
        {
            StoredCommands sc = new StoredCommands();
            sc.AppendToolParameterCommand(paramName, v[0], v[1], v[2]);
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
        public bool GetToolParameter(string paramName, ref Vector3 v)
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

        public Vector3 ToWorld(Vector3 vScene) {
            StoredCommands sc = new StoredCommands();
            floatArray f = vScene.toFloatArray();
            uint key = sc.AppendQueryCommand_ConvertPointToWorld(f.cast());
            ExecuteCommands(sc);
            sc.GetQueryResult_ConvertPointToWorld(key, f.cast());
            return new Vector3(f);
        }
        public Vector3 ToScene(Vector3 vWorld) {
            StoredCommands sc = new StoredCommands();
            floatArray f = vWorld.toFloatArray();
            uint key = sc.AppendQueryCommand_ConvertPointToScene(f.cast());
            ExecuteCommands(sc);
            sc.GetQueryResult_ConvertPointToScene(key, f.cast());
            return new Vector3(f);
        }




        public void SelectionBoundingBox(ref Vector3 Min, ref Vector3 Max)
        {
            StoredCommands sc = new StoredCommands();
            uint key = sc.AppendQueryCommand_GetBoundingBox();
            ExecuteCommands(sc);
            floatArray min = new floatArray(3), max = new floatArray(3);
            sc.GetQueryResult_GetBoundingBox(key, min.cast(), max.cast());
            Min.Set(min); Max.Set(max);
        }

    }
}
