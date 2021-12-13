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

#ifndef INETADDRESS_IPV4_H_
#define INETADDRESS_IPV4_H_

#include <string>
#include <vector>

namespace wss {
	/**
	*	@author Demetrius Comes
	*	@date	12-31-00
	*
	*
	*	Representation of a port number the class can translate between host order and network
	*	order bytes
	*/
	class PortNum
	{
	public:
		enum {UNKNOWN = 0};
	public:
		/**
		* @date  12/31/2000 10:30:43 PM
		* @param  unsigned short nPort = UNKNOWN
		*
		*  default ctor
		*/
		PortNum(unsigned short nPort = UNKNOWN) : m_nPort(nPort){}
		/**
		* @date  12/31/2000 10:30:51 PM
		* @return  unsigned short
		*
		*  returns the port number in network byte order
		*/
		unsigned short getNetworkType() const  {return htons(m_nPort);}
		/**
		* @date  12/31/2000 10:31:03 PM
		* @return  unsigned short
		*
		*  returns the port number in host order
		*/
		unsigned short getHostType() const {return ntohs(m_nPort);}
		/**
		* @date  12/31/2000 10:31:22 PM
		* @return  operator unsigned
		*
		*  casting operator
		*/
		operator unsigned short() const {return m_nPort;}
		PortNum(const PortNum &r) : m_nPort(r.m_nPort) {}
		PortNum &operator=(unsigned short s) {m_nPort=s;return *this;}
	private:
		unsigned short m_nPort;
	};

	/**
	*	@author Demetrius Comes
	*	@date 12-9-00
	*
	*
	*	Representation of ipv4 address
	*/
	class InetAddressV4
	{
	public:
		/**
		* @date  11/4/2003 7:00:39 PM
		*
		*
		*/
		InetAddressV4();
		/**
		* @date  11/4/2003 7:00:40 PM
		* @param  const struct in_addr &addr
		*
		*
		*/
		InetAddressV4(const struct in_addr &addr);
		/**
		* @date  11/5/2003 12:03:14 AM
		* @param  uint32 addr
		*
		*
		*/
		InetAddressV4(uint32_t addr);
		/**
		* @date  11/4/2003 7:00:42 PM
		* @param  const std::string &strAddr
		*
		*
		*/
		InetAddressV4(const std::string &strAddr);
		/**
		* @date  11/4/2003 7:00:44 PM
		* @param  const InetAddressV4 &r
		*
		*
		*/
		InetAddressV4(const InetAddressV4 &r);
		/**
		* @date  11/4/2003 7:00:45 PM
		* @return  const std::string
		*
		*
		*/
		const std::string &getHostname() const;
		/**
		* @date  11/4/2003 7:00:47 PM
		* @return  std::string
		*
		*
		*/
		std::string getDotAddress() const;
		/**
		* @date  11/4/2003 7:00:48 PM
		* @return  bool
		* @param  void
		*
		*
		*/
		bool isValid(void) const;
		/**
		* @date  11/4/2003 7:00:50 PM
		* @return  struct in_addr
		*
		*
		*/
		inline struct in_addr getAddress() const {return mIPaddr;}
		/**
		* @date  11/4/2003 7:00:51 PM
		* @return  InetAddressV4 &
		* @param  const std::string &str
		*
		*
		*/
		InetAddressV4 &operator=(const std::string &str);
		/**
		* @date  11/4/2003 7:00:52 PM
		* @return  InetAddressV4 &
		* @param  struct in_addr
		*
		*
		*/
		InetAddressV4 &operator=(struct in_addr);
		/**
		* @date  11/4/2003 7:00:54 PM
		* @return  bool
		* @param  const InetAddressV4 &a
		*
		*
		*/
		bool operator==(const InetAddressV4 &a) const;
		/**
		* @date  11/4/2003 7:00:55 PM
		* @return  bool
		* @param  const InetAddressV4 &a
		*
		*
		*/
		bool operator!=(const InetAddressV4 &a) const;
		/**
		* @date  11/4/2003 7:00:56 PM
		* @return  bool
		*
		*
		*/
		inline bool operator!() const {return !isValid();}
		/**
		* @date  12/4/2003 3:45:13 PM
		* @return  void
		*
		*  set the InetAddress invalid
		*/
		void clear();
	public:
		/**
		* @date  11/4/2003 7:01:00 PM
		* @return  InetAddressV4
		* @param  std::string strHost
		*
		*
		*/
		static InetAddressV4 fromString(std::string strHost);
		/**
		* @date  11/4/2003 7:01:01 PM
		* @return  std::vector<std::string>
		* @param  const std::string &strHost
		*
		*
		*/
		static std::vector<std::string> getAllHostNames(const std::string &strHost);
	protected:
		struct in_addr mIPaddr;
		mutable std::string mHostName;
	};
}
#endif
