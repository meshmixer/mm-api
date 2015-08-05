using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;

namespace mm
{
    // A delegate type for hooking up change notifications.
    public delegate void MeshChangedHandler(object sender);

    public class MeshChangedListener
    {
        int nPort;
        string sFilePath;

        UDPServer server;
        Thread serverThread;

        PackedMesh lastRead;
        Mutex lastRead_mutex;

        public event MeshChangedHandler OnMeshChanged;

        public MeshChangedListener(int nPort, string sFilePath) {
            this.nPort = nPort;
            this.sFilePath = sFilePath;
            lastRead = new PackedMesh();
            lastRead_mutex = new Mutex();
        }
        ~MeshChangedListener() {
            if (server != null)
                server.Shutdown();
        }
            

        public void Start()
        {
            server = new UDPServer(this.nPort);
            ThreadStart threadDelegate = new ThreadStart(server.RunThread);
            serverThread = new Thread(threadDelegate);
            serverThread.Start();

            server.OnServerReceivedDatagram +=
                new ServerMessageHandler(this.OnServerDatagram);
        }

        public void Stop()
        {
            if (server != null)
            {
                server.Shutdown();
                server = null;
            }
        }



        void OnServerDatagram(object sender, byte[] data)
        {
            lastRead_mutex.WaitOne();
            lastRead.Read(sFilePath);
            lastRead_mutex.ReleaseMutex();

            OnMeshChanged(this);
        }



        public void ForceReadPort() 
        {
            lastRead_mutex.WaitOne();
            lastRead.Read(sFilePath);
            lastRead_mutex.ReleaseMutex();
        }

        public PackedMesh GetLastMesh()
        {
            lastRead_mutex.WaitOne();
            PackedMesh pm = (PackedMesh)lastRead.Clone();
            lastRead_mutex.ReleaseMutex();
            return pm;
        }
        public void GetLastMesh(PackedMesh mCopyTo)
        {
            lastRead_mutex.WaitOne();
            lastRead.CopyTo(mCopyTo);
            lastRead_mutex.ReleaseMutex();
        }
        

    }




    delegate void ServerMessageHandler(object sender, byte[] data);

    class UDPServer
    {
        int nPort;
        public bool bContinue;
        public UdpClient listenSocket;

        public event ServerMessageHandler OnServerReceivedDatagram;

        public UDPServer(int nPort)
        {
            this.nPort = nPort;
            this.bContinue = true;
        }

        public void Shutdown()
        {
            this.bContinue = false;
            if (listenSocket != null)
                listenSocket.Close();
        }

        public void RunThread()
        {
            IPEndPoint listenIP = new IPEndPoint(IPAddress.Loopback, this.nPort);

            listenSocket = new UdpClient();
            listenSocket.ExclusiveAddressUse = false;
            listenSocket.Client.SetSocketOption(
                SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            listenSocket.Client.Bind(listenIP);

            while (this.bContinue)
            {
                try
                {
                    IPEndPoint remoteIP = new IPEndPoint(IPAddress.Any, 0);
                    byte[] receiveBuf = listenSocket.Receive(ref remoteIP);

                    if (receiveBuf.Length > 0)
                        OnServerReceivedDatagram(this, receiveBuf);

                } catch {
                    // who cares!
                }

                // send signal!
            }

            listenSocket.Close();
        }
    }
}
