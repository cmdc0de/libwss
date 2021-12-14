//////////////////////////////////////////////////////////////
//	Inet namespace for the wolfshade LGPL lib.
//Copyright (C) 2000 Demetrius Comes
//
//This library is free software; you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.
//
//This library is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this library; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//	Permission is given to Incredible technologies to use and modify this code without
//	cost of any kind. Demetrius Comes 11-03-03
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include "socket_typedefs.h"
#pragma warning(disable: 4786)
#else 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "inetaddress_v4.h"
#include <string>
#include <cstring>
#include <locale>

using namespace wss;

InetAddressV4::InetAddressV4() {
	clear();
};

InetAddressV4::InetAddressV4(const std::string &strAddr) {
	mIPaddr.s_addr = ::inet_addr(strAddr.c_str());
};

InetAddressV4::InetAddressV4(const struct in_addr &addr) {
	::memcpy(&mIPaddr,&addr,sizeof(in_addr));
};

InetAddressV4::InetAddressV4(uint32_t addr) {
	::memset(&mIPaddr, 0, sizeof(in_addr));
	mIPaddr.s_addr = addr;
}

InetAddressV4::InetAddressV4(const InetAddressV4 &r) : mIPaddr(r.mIPaddr), mHostName(r.mHostName) {
}

void InetAddressV4::clear() {
	::memset(&mIPaddr, 0, sizeof(in_addr));
	mIPaddr.s_addr=INADDR_NONE;
}

bool InetAddressV4::isValid(void) const {
	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));
	if(memcmp(&addr, &mIPaddr, sizeof(addr))) {
		if(mIPaddr.s_addr!=INADDR_NONE) {
			return true;
		}
	}
	return false;
}

InetAddressV4 &InetAddressV4::operator=(struct in_addr addr) {
	::memcpy(&mIPaddr,&addr,sizeof(in_addr));
	return *this;
}

InetAddressV4 &InetAddressV4::operator=(const std::string &str) {
	mIPaddr.s_addr = inet_addr(str.c_str());
	return *this;
}

bool InetAddressV4::operator==(const InetAddressV4 &a) const {
	if(memcmp((char *)&mIPaddr, (char *)&a.mIPaddr, sizeof(mIPaddr)))
		return false;
	return true;
}

bool InetAddressV4::operator!=(const InetAddressV4 &a) const {
	if(memcmp((char *)&mIPaddr, (char *)&a.mIPaddr, sizeof(mIPaddr)))
		return true;
	return false;
}

InetAddressV4 InetAddressV4::fromString(std::string strHost) { 
	struct hostent *hp;
	struct in_addr **blist;
	char	hostname[1024] = {'\0'};
	struct in_addr ipaddr;
	
	if(strHost.empty()) {
		if(gethostname(hostname, sizeof(hostname))==-1) {
			return InetAddressV4();
		}
		strHost = hostname;
	}

	if(std::isdigit(strHost[0],std::locale()))	{
		ipaddr.s_addr = inet_addr(strHost.c_str());
	} else {
		hp = gethostbyname(strHost.c_str());
		if(!hp)
			return InetAddressV4();

		blist = (struct in_addr **)hp->h_addr_list;
		while(*blist != NULL)
			++blist;
		--blist;
		ipaddr = **blist; 

	}
	return InetAddressV4(ipaddr);
};

const std::string &InetAddressV4::getHostname() const {
	//Lazy eval of host name
	if(mHostName.empty()) {
		struct hostent *hp;

		hp = gethostbyaddr((char *)&mIPaddr, sizeof(mIPaddr), AF_INET);
	
		if(hp)
			mHostName = hp->h_name;
		else
			mHostName = inet_ntoa(mIPaddr);
	}
	return mHostName;
}

std::vector<std::string> InetAddressV4::getAllHostNames(const std::string &strHost) {
	struct hostent *hp;
	hp = gethostbyname(strHost.c_str());
	if(hp) {
		std::vector<std::string> Names;
		Names.insert(Names.end(),hp->h_name);
		int i(0);
		while(hp->h_aliases[i]!=0) {
			Names.insert(Names.end(),hp->h_aliases[i++]);
		}
		return Names;
	}
	return std::vector<std::string>();
}

std::string InetAddressV4::getDotAddress() const {
	return std::string(inet_ntoa(mIPaddr));
}
