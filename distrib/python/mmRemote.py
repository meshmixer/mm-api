import mmapi
import struct
import socket
import array


class mmRemote:
	
	def __init__(self):
		self.address = "127.0.0.1";
		self.receive_port = 0xAFDF;
		self.send_port = 0xAFCF;
		self.debug_print = False;

	def connect(self):
		self.send_sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
		self.receive_sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
		self.receive_sock.bind( (self.address, self.receive_port) )
		
	def shutdown(self):	
		self.send_sock.close()
		self.receive_sock.close()
		
	def runCommand(self, cmd):
		
		# serialize and create raw buffer
		serializer = mmapi.BinarySerializer()
		cmd.Store(serializer)
		commandBuf = serializer.buffer()
		#sendBuf = struct.pack("B"*len(commandBuf), *commandBuf)   # [RMS] not efficient!
		sendBuf = array.array('B',commandBuf)
		
		if self.debug_print:
			print "[mmRemote::runCommand] sending..."
		self.send_sock.sendto( sendBuf, (self.address, self.send_port) )
		if self.debug_print:
			print "[mmRemote::runCommand] waiting for response..."
		data, addr = self.receive_sock.recvfrom(1024*64)
		if self.debug_print:
			print "[mmRemote::runCommand] received result!..."
		
		# unpack result buffer into StoredCommands results
		#rcvList = struct.unpack("B"*len(data), data)
		rcvList = array.array('B', data)
		resultBuf = mmapi.vectorub(rcvList)
		serializer.setBuffer(resultBuf)
		cmd.Restore_Results(serializer)
		
		
		