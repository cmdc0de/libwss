#ifndef WSS_IONODE_H_
#define WSS_IONODE_H_

#include "../error_type.h"

namespace wss {
	/**
	*	class IoNode
	*	@author Demetrius 
	*	Comes class for File and Directory...so Directory can keep
	*	a list of Ionodes...
	*
	*	Pattern:  Composite
	*/
	class IoNode
	{
	public:
		/**
		* @date  10/1/2003 12:32:07 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isReadOnly() const =0;
		/**
		* @date  10/1/2003 12:32:09 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isWriteable() const =0;
		/**
		* @date  2/17/2005 2:17:29 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isExecutable() const =0;
		/**
		* @date  10/1/2003 12:32:10 PM
		* @return  bool
		*  
		*  
		*/
		virtual bool makeReadOnly() const =0;
		/**
		* @date  10/1/2003 12:32:12 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeWriteable() const =0;
		/**
		* @date  2/17/2005 2:17:09 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableUser() const =0;
		/**
		* @date  2/17/2005 2:17:10 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableGroup() const =0;
		/**
		* @date  2/17/2005 2:17:11 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableAll() const =0;
		/**
		* @date  10/1/2003 12:32:17 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isFile() const {return false;}
		/**
		* @date  10/1/2003 12:32:19 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isDirectory() const {return false;}
		/**
		* @date  10/1/2003 12:32:20 PM
		*  
		*  
		*/
		IoNode() {}
		/**
		* @date  10/1/2003 12:32:21 PM
		* @return  ErrorType 
		*  
		*  returns true on success
		*/
		virtual ErrorType removeFromFS()=0;
		/**
		* @date  10/1/2003 12:32:22 PM
		* @return  bool 
		*  
		*  return true if file/directory exists
		*/
		virtual bool exists() const =0;
		/**
		* @date  10/1/2003 12:32:23 PM
		*  
		*  
		*/
		virtual ~IoNode() {}
	};
} //io
#endif
