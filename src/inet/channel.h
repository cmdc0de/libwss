#ifndef WSS_CHANNELS_H
#define WSS_CHANNELS_H

#pragma once

#include "../portable_types.h"
#include "../io/readerwriter.h"
#include "../error_type.h"

namespace wss {
	class TCPServerSocket;
//	class SequenceIDValidator;

/*
*	@Author Demetrius Comes
*
*	Represents a communication channel basically an abstraction of a socket/protocol mix
*
*	Currently (2/2012) this class only via TCPV4
*	Future should implement TCPV6, UDPV4 and UDPV6
*/
class ComChannel  {
public: //static
	ComChannel();
	/**
	* @return  int 
	*  
	*  calls to decedents to buffer incoming bytes but tracks bytes for stats
	*/
	int bufferIn();
	/**
	* @return  int 
	*  
	*  returns the number of bytes sent to the communication channel (socket) or -1 for error
	*/
	int sendData();
	/**
	* @return  void 
	*  
	*  marks a socket to be closed once it's outbuffer is empty
	*/
	void setDeath();
	/*
	*	Sets the communication channel to be cleaned up by the next "pulse" of  ASyncClient, ASyncMultiplexedClient, and ASync Server
	*/
	bool markedForDeath() const {return mMarkedForDeath;}
	/*
	*	Interface to returned the number of bytes available on the specific implementation's socket 
	*/
	virtual int getByteCount()=0;
	/*
	*	Returns the size of the application input buffer.  
	*	Low level we read from socket and place bytes into this buffer.
	*/
	uint32_t getCommandBufferLength() {return uint32_t(mIncomingBuffer.size());}
	/*
	*	returns the IPV4 peer address of socket
	*/
	virtual bool getPeerAddress(InetAddressV4 &addr, PortNum &portNum)=0;
	/*
	*	returns the NativeTimeType of the last time any data was received on the lower level socket
	*/
	virtual NativeTimeType getLastReceiveTime();
	/*
	*	return the NativeTimeType of the creation time of this communication channel
	*/
	NativeTimeType getBirthDate() {return mBirthDate;}
	/*
	*	Bytes to remove from the incoming buffer, most likely because you already processed them
	*/
	void removeFromBuffer(uint32_t bytesToRemove);
	/*
	*	return the number of bytes sent through this comchannel, stat info
	*/
	uint64_t getBytesSent() const {return mBytesSent;}
	/*
	*	return the number of bytes received through this comchannel, stat info
	*/
	uint64_t getBytesReceived() const {return mBytesReceived;}
	/*
	*	returns true if there is data in the outgoing buffer
	*/
	bool hasDataToSend();
	/*
	*	Returns the incoming buffer
	*/
	const ReaderWriter &getIncomingBuffer() {return mIncomingBuffer;}
	/*
	*	Given a container write it to the outgoing buffer
	*/
	ErrorType bufferOut(const void * data, size_t len);

	virtual ~ComChannel();
protected:
	void setLastReceiveTime(NativeTimeType now);
protected:
	virtual int onBufferIn()=0;
	virtual int onSendData()=0;
	void removeFromOutBuffer(uint32_t bytesToRemove);
	const ReaderWriter &getOutBuffer() {return mOutBuffer;}
	void clearMessageBuffer();
protected:
	ReaderWriter		mIncomingBuffer;
	NativeTimeType		mLastReceiveTime;
	ReaderWriter		mOutBuffer;
private:
	uint64_t				mBytesSent;
	uint64_t				mBytesReceived;
	NativeTimeType		mBirthDate;
	uint64_t				mBytesBuffered;
	bool					mMarkedForDeath;
};

/*
*	tcp communication channel
*/
class TCPComChannel : public ComChannel {
public:
	TCPComChannel(TCPServerSocket *ss);
	virtual bool getPeerAddress(InetAddressV4 &addr, PortNum &portNum);
	virtual int getByteCount();
	virtual ~TCPComChannel();
	ErrorType getLastSocketError();
protected:
	virtual int onBufferIn();
	virtual int onSendData();
private:
	TCPServerSocket	*mSock;
};

}
#endif
