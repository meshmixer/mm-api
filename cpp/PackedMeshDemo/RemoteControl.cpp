#include "RemoteControl.h"
#include <StoredCommands.h>

#include <winsock2.h>
#include <iostream>

using namespace mm;


namespace mm
{

class UDPConnection
{
public:
	WSAData m_winsockData;
	bool m_bWinsockDataInitialized;

	SOCKET m_socketDescriptor;
	unsigned int m_nRemotePort;
	sockaddr_in m_remoteAddress;
	bool m_bSocketAvailable;

	UDPConnection() {
		bool bOK = ( WSAStartup(0x0101, &m_winsockData) == 0 );
		if ( !bOK )
			abort();
		m_bWinsockDataInitialized = bOK;

		m_socketDescriptor = INVALID_SOCKET;
		m_bSocketAvailable = false;
	}

	bool IsConnected() { return m_bSocketAvailable; }

	bool OpenSendSocket(unsigned int nPort) 
	{
		m_nRemotePort = nPort;

		m_socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
		if ( m_socketDescriptor == INVALID_SOCKET ) {
			std::cerr << "[UDPConnection::OpenSendSocket] Could not create socket!" << std::endl;
			return false;
		}

		hostent * pHost = gethostbyname("localhost");
		in_addr * ipAddress = (struct in_addr*)pHost->h_addr_list[0];

		memset((void *)&m_remoteAddress, '\0', sizeof(struct sockaddr_in));

		// set family, port, and copy IP address 
		m_remoteAddress.sin_family = AF_INET;
		m_remoteAddress.sin_port = htons((u_short)m_nRemotePort);
		m_remoteAddress.sin_addr.s_addr = ipAddress->s_addr;

		m_bSocketAvailable = true;

		// socket is open
		return true;
	}


	bool TransmitData(const unsigned char * pData, unsigned int nBytes) {
		return TransmitData((const char *)pData, nBytes);
	}
	bool TransmitData(const char * pData, unsigned int nBytes) {
		if ( m_bSocketAvailable == false || m_socketDescriptor == INVALID_SOCKET )
			return false;

		int sockaddr_len = sizeof(struct sockaddr_in);
		int nSent = sendto(m_socketDescriptor, pData, nBytes, 0, (struct sockaddr *)&m_remoteAddress, sockaddr_len);
		if ( nSent == -1 )
			return false;
		if ( nSent < (int)nBytes )
			return false;

		return true;
	}



	bool OpenReceiveSocket(unsigned int nPort, bool bDoBind = true) 
	{
		m_nRemotePort = nPort;

		m_socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
		if ( m_socketDescriptor == INVALID_SOCKET ) {
			std::cerr << "[UDPConnection::OpenSendSocket] Could not create socket!" << std::endl;
			return false;
		}

		memset((void *)&m_remoteAddress, '\0', sizeof(struct sockaddr_in));

		// set family, port, and copy IP address 
		m_remoteAddress.sin_family = AF_INET;
		m_remoteAddress.sin_port = htons((u_short)m_nRemotePort);
		m_remoteAddress.sin_addr.s_addr = INADDR_ANY;

		m_bSocketAvailable = true;

		if ( bDoBind ) {
			return BindReceiveSocket();
		} else
			return true;
	}
	bool BindReceiveSocket()
	{
		if ( m_bSocketAvailable == false )
			return false;
		int sockaddr_len = sizeof(struct sockaddr_in);
		if ( bind(m_socketDescriptor ,(struct sockaddr *)&m_remoteAddress, sockaddr_len) == SOCKET_ERROR)
			return false;
		return true;
	}

	bool WaitForData(unsigned char * pData, int nMaxBytes, int & nBytes) {
		return WaitForData((char *)pData, nMaxBytes, nBytes);
	}
	bool WaitForData(char * pData, int nMaxBytes, int & nBytes) {
		sockaddr_in recvAddr;
		int nRecvAddrLen = sizeof(sockaddr_in);

		int nRead = recvfrom(m_socketDescriptor, pData, nMaxBytes, 0, (struct sockaddr *)&recvAddr, &nRecvAddrLen);
		if ( nRead == SOCKET_ERROR ) {
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			return false;
		}

		nBytes = nRead;
		return true;
	}


	void CloseSocket() {
		if ( m_bSocketAvailable && m_socketDescriptor != INVALID_SOCKET ) {
			closesocket(m_socketDescriptor);
			m_socketDescriptor = INVALID_SOCKET;
			m_bSocketAvailable = false;
		}
	}

	~UDPConnection() {
		if ( m_socketDescriptor != INVALID_SOCKET )
			CloseSocket();
		if ( m_bWinsockDataInitialized )
			WSACleanup();
	}



};

} // end namespace mm;




RemoteControl::RemoteControl()
{
	pSendConnection = new UDPConnection();
	pRecvConnection = new UDPConnection();
}
RemoteControl::~RemoteControl() 
{
 	if ( pSendConnection ) {
		Disconnect();
		delete pSendConnection;
		pSendConnection = NULL;
	}
	if ( pRecvConnection ) {
		delete pRecvConnection;
		pRecvConnection = NULL;
	}
}

bool RemoteControl::Connect(unsigned int nPortNum, unsigned int nResponsePortNum)
{
	if ( pSendConnection != NULL )
		Disconnect();

	bool bOK = pSendConnection->OpenSendSocket(nPortNum);
	bOK = bOK && pRecvConnection->OpenReceiveSocket(nResponsePortNum);
	return bOK;
}

bool RemoteControl::IsConnected()
{
	return (pSendConnection != NULL && pSendConnection->IsConnected() && pRecvConnection != NULL && pRecvConnection->IsConnected() );
}


bool RemoteControl::Send(StoredCommands * pCommands)
{
	if ( ! IsConnected() )
		return false;

	rms::BinarySerializer serializer;
	pCommands->Store(serializer);

	bool bSendOK = pSendConnection->TransmitData(&serializer.buffer()[0], (unsigned int)serializer.buffer().size() );

	bool bReadOK = false;
	if ( bSendOK ) {
		size_t nMaxData = 65536;
		std::vector<unsigned char> vResponseBytes;
		vResponseBytes.resize(nMaxData);
		int nReadBytes = 0;
		bReadOK = pRecvConnection->WaitForData(&vResponseBytes[0], (int)nMaxData, nReadBytes);
		if ( bReadOK && nReadBytes > 0 ) {
			serializer.setBuffer(vResponseBytes);
			pCommands->Restore_Results(serializer);
		}
	}

	return bSendOK && bReadOK;
}


void RemoteControl::Disconnect()
{
	if ( pSendConnection ) 
		pSendConnection->CloseSocket();
	if ( pRecvConnection )
		pRecvConnection->CloseSocket();
}