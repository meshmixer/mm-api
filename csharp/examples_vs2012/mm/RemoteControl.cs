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



    }
}
