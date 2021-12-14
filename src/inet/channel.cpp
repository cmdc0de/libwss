#include <sstream>
#include <time.h>
#include "tcp.h"
#include "channel.h"
#include "../wsinit.h"

using namespace wss;

ComChannel::ComChannel() : mIncomingBuffer(ReaderWriter::Create_Default_Interface()), mLastReceiveTime(time(0)), mOutBuffer(ReaderWriter::Create_Default_Interface()), mBytesSent(0),mBytesReceived(0), mBirthDate(time(0)), 
		mBytesBuffered(0), mMarkedForDeath(false) {}

int ComChannel::bufferIn() {
	int bytesIn = onBufferIn();
	if(bytesIn>0) {
		//VALUE_METRIC2("Channel::bufferInBytes", bytesIn);
		mBytesReceived+=bytesIn;
	}
	return bytesIn;
}
bool ComChannel::hasDataToSend() {
	return mOutBuffer.size()!=0;
}

ErrorType ComChannel::bufferOut( const void * data, size_t len )
{
	ErrorType et;
	size_t outlen = mOutBuffer.write(data, len);
	assert(outlen == len);
	//VALUE_METRIC2("Channel::bufferOutBytes", outlen);
	return et;
}

int ComChannel::sendData() {
	int writtenBytes = onSendData();
	if(writtenBytes>0) {
		mBytesSent+=writtenBytes;
	}
	return writtenBytes;
}
void ComChannel::setDeath() {
	mMarkedForDeath = true;
}

void ComChannel::removeFromBuffer(uint32_t bytesToRemove) {
	mIncomingBuffer.erase(0, bytesToRemove);
}

NativeTimeType ComChannel::getLastReceiveTime() {
	return mLastReceiveTime;
}

void ComChannel::setLastReceiveTime(NativeTimeType now) {
	mLastReceiveTime = now;
}

ComChannel::~ComChannel() {}

void ComChannel::removeFromOutBuffer(uint32_t bytesToRemove) {
	mOutBuffer.erase(0, bytesToRemove);
}

TCPComChannel::TCPComChannel(TCPServerSocket *ss) 
	: ComChannel(), mSock(ss) {
	mSock->setNonBlocking();
	mSock->setNagelOff();
}

bool TCPComChannel::getPeerAddress(InetAddressV4 &addr, PortNum &portNum) {
	if(mSock) {
		if(mSock->getPeerAddress(addr,portNum)) {
			return true;
		}
	}
	return false;
}
TCPComChannel::~TCPComChannel() {
	if(mSock) {
		mSock->closeSocket();
		delete mSock;
	}
}

ErrorType TCPComChannel::getLastSocketError() {
	if(mSock) {
		return mSock->getLastError();
	} else {
		return ErrorType(ErrorType::codeOSSpecific,BaseSocketInterface::SENOTCONN);
	}
}

int TCPComChannel::onBufferIn() {
	char buf[512];
	int total = 0;
	int bytes = mSock->receive(buf,512);
	if(bytes>0) {
		setLastReceiveTime(time(0));
		mIncomingBuffer.write(buf, bytes);
		total+=bytes;
		while((bytes = mSock->receive(buf,512))>0) {
			mIncomingBuffer.write(buf, bytes);
			total += bytes;
		}
		return total;
	} else {
		if(bytes==0 && !mSock->getLastError()) {
			GET_LOGGER->error("Socket read error: {}", mSock->getLastError().getOSErrorString().c_str());
		}
		return bytes;
	}
}

int TCPComChannel::onSendData() {
	int total_sent = 0;
	if(hasDataToSend())
	{
		size_t raw_size = 0;
		int sent = 0;
		const char * raw_ptr = 0;
		do 
		{
			raw_ptr = reinterpret_cast<const char *>(getOutBuffer().raw(total_sent, raw_size));
			sent = mSock->send(raw_ptr, raw_size);
			if(BaseSocketInterface::SOCK_ERROR == sent)
			{
				//total_sent bytes will still be removed even if 
				//the send fails.  All successful sends prior to 
				//this one are still considered successful.
				std::string strErr;
				mSock->getLastError().getErrorString(strErr);
				GET_LOGGER->error("Socket Send Error: {}", strErr.c_str());
				break;
			}
			else
			{
				total_sent += sent;
			}
		} 
		//Keep sending data as long as we send the whole chunk
		//Sending a partial chunk is not an error and the data will remain in the buffer for the next call.
		while (raw_size == (uint32_t)sent && (uint32_t)total_sent < getOutBuffer().size());

		//VALUE_METRIC2("Channel::socketOutBytes", total_sent);
		GET_LOGGER->trace("TCPComChannel - Sent {}/{} bytes", total_sent, getOutBuffer().size());

		//Remove any data we sent
		removeFromOutBuffer(total_sent);
	}
	return total_sent;
}

int TCPComChannel::getByteCount() {return mSock->bytesToRead();}

