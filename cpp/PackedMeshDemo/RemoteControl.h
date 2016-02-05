#ifndef __MMAPI_REMOTE_CONTROL_H__
#define __MMAPI_REMOTE_CONTROL_H__

namespace mm {

class UDPConnection;
class StoredCommands;

class RemoteControl
{
public:
	RemoteControl();
	virtual ~RemoteControl();

	bool Connect(unsigned int nPortNum, unsigned int nResponsePortNum);
	bool IsConnected();

	// [RMS] this sends the command and waits for the result on the receive socket
	bool Send(StoredCommands * pCommands);

	void Disconnect();

protected:
	UDPConnection * pSendConnection;
	UDPConnection * pRecvConnection;
};

} // end namespace mm

#endif __MMAPI_REMOTE_CONTROL_H__