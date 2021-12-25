
#ifndef IT_PROPERTIES_H_
#define IT_PROPERTIES_H_

#include "../portable_types.h"
#include "io_typedefs.h"
#include <time.h>
#include <string>
#include <sstream>
#include <map>

namespace wss { 

class Properties {
private:
	typedef std::map<uint64_t,std::pair<std::string,std::string> > ValueMap;
public:
	static const int MAX_PROPERTIES_LINE = 1024;
public:
	/**
	* @date  9/18/2005 9:06:25 PM
	* @return  uint64 
	* @param  const std::string &str
	*  
	*  returns hash value of a string used by the Properties construct.
	*/
	static uint64_t calcStorageKey(const std::string &str);
public:
	/**
	* @date  9/18/2005 9:06:03 PM
	*  
	*  default ctor
	*/
	Properties();
public:
	/**
	* @date  9/18/2005 9:00:51 PM
	* @return  bool 
	* @param  const std::string  &name
	* @param  T &value
	*  
	*  populates value with the value found in the value map
	*	returns true if value is found
	*/
	template<typename T>
	bool getProperty(const std::string  &name, T &value) const {
		return getProperty(name,value,value);
	}
	/**
	* @date  9/18/2005 9:01:41 PM
	* @return  bool 
	* @param  const std::string  &name
	* @param  T &value
	* @param  const T &defaultValue
	*  
	*	populates  value with the value found in the value map retursn true if value is found
	*	if value is not found the value is set to default value.
	*/
	template<typename T>
	bool getProperty(const std::string  &name, T &value, const T &defaultValue) const {
		return getPropertyByHash(calcStorageKey(name),value,defaultValue);
	}
	/**
	* @date  9/18/2005 9:02:35 PM
	* @return  bool 
	* @param  const uint64 &nameHash
	* @param  T &value
	*  
	*	same as getProperty except you pass the hash value instead of the name string 
	*/
	template<typename T>
	bool getPropertyByHash(const uint64 &nameHash, T &value) const {
		return( getPropertyByHash(nameHash,value,value) );
	}
	/**
	* @date  9/18/2005 9:03:10 PM
	* @return  bool 
	* @param  const uint64 &nameHash
	* @param  T &value
	* @param  const T &defaultValue
	*  
	*  same as getPropertyByHash with default except look up is done via hash value instead of hashing the name
	*	string passed in
	*/
	template<typename T>
	bool getPropertyByHash(const uint64 &nameHash, T &value, const T &defaultValue) const {
		ValueMap::const_iterator it = mValueMap.find(nameHash);
		if(it!=mValueMap.end()) {
			std::istringstream is((*it).second.second.c_str());
			is >> value;
			return true;
		}
		value = defaultValue;
		return false;
	}
	/**
	* @date  6/21/2006 11:16:00 AM
	* @return  bool 
	* @param  const uint64 &nameHash
	* @param  T (*__w64 value)[N]
	* @param  const T (*__w64 defaultValue)[N]
	*  
	*  template specialization for arrays
	*/
	template<typename T, int N>
	bool getPropertyArrayByHash(const uint64 &nameHash, T (*__w64 value)[N], const T (*__w64 defaultValue)[N]) const {
		int i;
		ValueMap::const_iterator it = mValueMap.find(nameHash);
		if(it!=mValueMap.end()) {
			std::istringstream is((*it).second.second.c_str());
			for(i=0;i<N;i++) {
				is >> (*value)[i];
			}
			return true;
		}
		for(i=0;i<N;i++) {
			(*value)[i] = (*defaultValue)[i];
		}
		return false;
	}
	/**
	* @date  9/18/2005 9:04:02 PM
	* @return  bool 
	* @param  const uint64 &nameHash
	* @param  std::string &value
	* @param  const std::string &defaultValue
	*  
	*  template specialization for strings
	*/
	bool getPropertyByHash(const uint64 &nameHash, std::string &value, const std::string &defaultValue) const {
		ValueMap::const_iterator it = mValueMap.find(nameHash);
		if(it!=mValueMap.end()) {
			value = (*it).second.second.c_str();
			return true;
		}
		value = defaultValue;
		return false;
	}
	/**
	* @date  9/18/2005 9:04:24 PM
	* @return  void 
	* @param  const std::string &name
	* @param  const T &value
	*  
	*  allows you to add name value pairs to the valuemap
	*/
	template<typename T>
	void addProperty(const std::string &name, const T &value) {
		std::ostringstream os;
		os << value;
		mValueMap.insert(std::pair<uint64,std::pair<std::string,std::string> > (calcStorageKey(name),std::pair<std::string,std::string>(name,os.str())));
	}
	/**
	* @date  6/22/2006 3:11:00 PM
	* @return  void 
	* @param  const std::string &name
	* @param  const T (*__w64 value)[N]
	*  
	*  template specialization for strings
	*/
	template<typename T, int N>
	void addPropertyArray(const std::string &name, const T (*__w64 value) [N]) {
		std::ostringstream os;
		if (N>0)
		{
			os << (*value)[0];
		}
		for(int i=1;i<N;i++) {
			os << " " << (*value)[i];
		}
		mValueMap.insert(std::pair<uint64,std::pair<std::string,std::string> > (calcStorageKey(name),std::pair<std::string,std::string>(name,os.str())));
	}
	/**
	* @date  9/18/2005 9:04:45 PM
	* @return  void 
	* @param  const std::string &name
	* @param  const std::string &value
	*  
	*  speciailization for strings
	*/
	void addProperty(const std::string &name, const std::string &value) {
		mValueMap.insert(std::pair<uint64,std::pair<std::string,std::string> > (calcStorageKey(name),std::pair<std::string,std::string>(name,value)));
	}
	/**
	* @date  9/18/2005 9:05:05 PM
	* @return  bool 
	* @param  const std::string &name
	*  
	*  returns true if found
	*/
	bool contains(const std::string &name) const {
		return contains(calcStorageKey(name));
	}
	/**
	* @date  9/18/2005 9:00:20 PM
	* @return  bool 
	* @param  uint64 nameHash
	*  
	*  retursn true if hashvalue is in valueMap
	*/
	bool contains(uint64 nameHash) const;
	/**
	* @date  9/18/2005 8:56:26 PM
	* @return  bool 
	* @param  std::basic_istream<char> &in
	*  
	*  returns true on an error free load
	*	Note lines must be less than 1024 characters and only contain 1 = 
	*	empty lines and lines that start with # are ignored.
	*/
	bool load(std::basic_istream<char> &in);
	/**
	* @date  9/18/2005 9:05:46 PM
	* @return  bool 
	* @param  const std::string &fileName
	*  
	*  see bool load(std::basic_istream<char> &in);
	* if file does not exist returns false
	*/
	bool load(const std::string &fileName);
	/**
	* @date  9/18/2005 9:05:51 PM
	* @return  bool 
	* @param  const TFILE &file
	*  
	*  seee bool load(std::basic_istream<char> &in);
	* if file does not exist returns false
	*/
	bool load(const TFILE &file);
	/**
	* @date  9/18/2005 8:58:05 PM
	* @return  bool 
	* @param  const std::string &fileName
	*  
	*  checks the file for a change in the modification timestamp on the file
	*	uses a 1 second resolution (i.e. time())
	*  returns true if file loaded, returns false if file could not be loaded OR file was not modified
	*/
	bool loadIfModified(const std::string &fileName);
	/**
	* @date  9/18/2005 8:59:31 PM
	* @param  std::basic_ostream<char> &out
	*  
	*  prints the name, value and the name's hash value to the output stream
	*/
	void list(std::basic_ostream<char> &out) const;
	/**
	* @date  9/18/2005 8:58:47 PM
	* @return  bool 
	* @param  std::basic_ostream<char> &out
	*  
	*  saves name value pairs to a file
	*	returns true on an error free save
	*/
	bool save(std::basic_ostream<char> &out) const;
	/**
	* @date  9/18/2005 9:05:28 PM
	* @return  bool 
	* @param  const std::string &fileName
	*  
	*  see bool save(std::basic_ostream<char> &out) const;
	*/
	bool save(const std::string &fileName) const;
	/**
	* @date  9/18/2005 9:05:33 PM
	* @return  bool 
	* @param  const TFILE &file
	*  
	*  see bool save(std::basic_ostream<char> &out) const;
	*/
	bool save(const TFILE &file) const;
	/**
	* @date  9/18/2005 8:58:47 PM
	* @return  bool 
	* @param  std::basic_ostream<char> &out
	*  
	*  saves name value pairs to a file sorted by name
	*	returns true on an error free save
	*/
	bool saveSortedByName(std::basic_ostream<char> &out) const;
	/**
	* @date  9/18/2005 9:05:28 PM
	* @return  bool 
	* @param  const std::string &fileName
	*  
	*  see bool saveSortedByName(std::basic_ostream<char> &out) const;
	*/
	bool saveSortedByName(const std::string &fileName) const;
	/**
	* @date  9/18/2005 9:05:33 PM
	* @return  bool 
	* @param  const TFILE &file
	*  
	*  see bool saveSortedByName(std::basic_ostream<char> &out) const;
	*/
	bool saveSortedByName(const TFILE &file) const;
	/**
	* @date  9/18/2005 10:27:54 PM
	* @return  void 
	*  
	*  
	*/
	void clear();
	/**
	* @date  9/18/2005 10:50:56 PM
	* @return  std::size_t 
	*  
	*  return the number of name value pairs stored
	*/
	std::size_t size() {return mValueMap.size();}
	/**
	* @date  9/18/2005 10:50:13 PM
	*  
	*  dtor
	*/
	~Properties();
private:
	struct tm mLastFileModificationTime;
	uint64 mLastFileNameHash;
	ValueMap mValueMap;
private:
	Properties(const Properties &defaults);  //broke
};

}

#endif

