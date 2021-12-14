#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include "ionode.h"
#include <fstream>

namespace wss {
	template<typename OSIOTraits,typename char_type, typename Traits > class Directory; //templated forward def
	/**
	*	class File
	*	@author: Demetrius Comes
	*	@date: 10-010-03
	*	Description:
	*		This is an abstraction of a file.  
	*/
	template<typename OSIOTraits,typename char_type, typename Traits = std::char_traits<char_type> >
	class File : public IoNode
	{
	protected:
		typedef std::basic_string<char_type, Traits> FileString;
		typedef Directory<OSIOTraits,char_type, Traits> DirType;
		typedef File<OSIOTraits, char_type, Traits> FileType;
	private:
		DirType m_Directory;
		FileString m_strFileName;
	public:
		/**
		* @date  10/13/00 3:58:50 PM
		* @return  FileString 
		*  
		*/
		FileString getName() const {return m_strFileName;}

		/**
		* @date  2/17/2005 11:49:01 AM
		* @return  DirType 
		*  
		*  
		*/
		DirType getDirectory() const {return m_Directory;}

		/**
		* @date  10/13/00 3:59:03 PM
		* @return  bool 
		*  
		*/
		virtual bool isFile() const {return true;}
		/**
		* @date  10/13/00 3:59:28 PM
		* @param  const DirType &dirPath
		* @param  const FileString &strFileName
		*  ctor 
		*/
		File(const DirType &dirPath,const FileString &strFileName);
		/**
		* @date  10/13/00 3:59:41 PM
		* @param  const FileString &strFileName
		*  Ctor This may be a full path not just the file name.
		*	the ctor will split it into it's directory and file parts if need be.
		*/
		File(const FileString &strFileName);
		/**
		* @date  10/13/00 3:59:52 PM
		*  default Ctor
		*/
		File() {}

		/**
		* @date  10/13/00 4:00:00 PM
		* @param  const File<OSIOTraits,char_type> &f
		*  copy ctor
		*/
		File(const FileType &f);
		File &operator=(const FileType &f);
		/**
		* @date  10/13/00 4:00:22 PM
		* @return  void 
		*  creates the file
		*	10-25-00 modified to take a bool to make directories if they don't exists.
		*/
		bool make(bool bMakeDirs) const;
		/**
		* @date  10/13/00 4:00:33 PM
		* @return  FileString 
		*  returns the absolutepath for the file
		*/
		FileString getAbsolutePath() const {return m_Directory.makeFullPath(m_strFileName);}
		/**
		* @date  10/13/00 4:00:50 PM
		* @return  void 
		* @param  const FileString &strTo
		*  Copies one file to another
		*/
		inline bool copyTo(const FileString &strTo) const;
		/**
		* @date  10/13/00 4:02:19 PM
		* @return  void 
		* @param  const File &toFile
		*  
		*/
		inline bool copyTo(const File &toFile) const;
		/**
		* @date  10/13/00 4:02:31 PM
		* @return  File 
		*  returns a os dependent temp file
		*/
		static File getTempFile();
		/**
		* @date  10/13/00 4:02:48 PM
		* @return  ErrorType 
		*  removes file from file system
		*/
		virtual ErrorType removeFromFS();
		/**
		* @date  10/13/00 4:03:01 PM
		* @return  bool 
		*  returns true if OS says file exists
		*/
		virtual bool exists() const;
		/**
		* @date  10/13/00 4:03:19 PM
		* @return  void 
		*  turns on readonly flag
		*/
		virtual bool makeReadOnly() const;
		/**
		* @date  10/13/00 4:03:37 PM
		* @return  void 
		*  turns off readonly flag
		*/
		virtual bool makeWriteable() const;
		/**
		* @date  2/17/2005 3:53:37 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableUser() const;
		/**
		* @date  2/17/2005 3:53:38 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableGroup() const;
		/**
		* @date  2/17/2005 3:53:39 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableAll() const;
		/**
		* @date  10/01/03 4:03:49 PM
		* @return  int 
		*  returns the size of the file in bytes
		*	-1 means failure
		*/
		virtual int getSize();
		/**
		* @date  10/13/00 4:04:02 PM
		* @return  bool 
		*  
		*/
		virtual bool isReadOnly() const;
		/**
		* @date  10/19/2005 12:32:22 PM
		* @return  bool 
		*  
		*  return true if file/directory renamed successfully
		*/
		virtual bool rename(const FileType &file);
		/**
		* @date  10/13/00 4:04:08 PM
		* @return  bool 
		*  
		*/
		virtual bool isWriteable() const;
		/**
		* @date  2/17/2005 3:52:31 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isExecutable() const;
		/**
		* @date  10/27/00 10:52:41 AM
		* @return  void 
		* @param  struct tm *t
		*  
		*  populates t with the file's last modifiction time
		*/
		inline bool getLastModificationTime(struct tm *t) const;
		/**
		* @date  10/27/00 12:03:09 PM
		* @return  void 
		* @param  struct tm *t
		*  
		*  populates t with the file's last access time
		*/
		inline bool getLastAccessTime(struct tm *t) const;
		/**
		* @date  10/27/00 11:17:39 AM
		* @return  void 
		* @param  struct tm *t
		*  
		*  set the last modifiction time on the file
		*/
		inline bool setLastModificationTime(struct tm *t);
		/**
		* @date  10/27/00 11:17:52 AM
		* @return  void 
		* @param  struct tm *t
		*  
		*  sets the last access time on the file
		*/
		inline bool setLastAccessTime(struct tm *t);
	};

//////////////////////////////////////////////////
//	Ctor
//  given a directory object and file string.
//////////////////////////////////////////////////
template<typename OSIOTraits,typename char_type, typename Traits>
File<OSIOTraits,char_type,Traits>::File(const DirType &dirPath,const FileString &strFileName) 
{
	//put it back to one string just incase they pass a directory
	//with the file name
	FileString fileTemp = dirPath.getPathWithSep() + strFileName;
	typename FileString::size_type n(fileTemp.find_last_of(OSIOTraits::PATH_SEP));
	FileString strDir = fileTemp.substr(0,n);
	m_strFileName = fileTemp.substr(n+1,fileTemp.size());
	m_Directory = DirType(strDir);
}

//////////////////////////////////////////////////////////////////////////
//	Given a string we spilt it into the directory and file name piece.
///////////////////////////////////////////////////////////////////////////
template<typename OSIOTraits,typename char_type, typename Traits>
File<OSIOTraits,char_type,Traits>::File(const FileString &strFileName) {
	typename FileString::size_type n(strFileName.find_last_of(OSIOTraits::PATH_SEP));
	//not found
	if(n==FileString::npos)	{
		m_strFileName = strFileName;
	} else {
		FileString strDir = strFileName.substr(0,n);
		m_strFileName = strFileName.substr(n+1,strFileName.size());
		m_Directory = DirType(strDir);
	}
}

//////////////////////////////////////////////////////////////////////////
//	Copy ctor
/////////////////////////////////////////////////////////////////////////
template<typename OSIOTraits,typename char_type, typename Traits>
File<OSIOTraits,char_type,Traits>::File(const FileType &f) {
	(*this)=f;
}

template<typename OSIOTraits,typename char_type, typename Traits>
File<OSIOTraits,char_type,Traits> & File<OSIOTraits,char_type,Traits>::operator=(const FileType &f) {
	m_Directory=f.m_Directory;
	m_strFileName=f.m_strFileName;
	return (*this);
}

///////////////////////////////////////////////////////////////////////////
//	make 
//	create the file
///////////////////////////////////////////////////////////////////////////
template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::make(bool bMakeDirs) const {
	if(!exists()) {
		if(bMakeDirs && !m_Directory.exists()) {
			m_Directory.make(true);
		}
		std::basic_ofstream<char_type> newFile((const char*)getAbsolutePath().c_str(),std::ios::out);
		if(!newFile.good()) {
			return false;
		}
		newFile.close();
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//	copyTo
//	creates a file object and calls overloaded copyTo
template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::copyTo(const FileString &strTo) const {
	File toFile(strTo);
	return copyTo(toFile);
}

//////////////////////////////////////////////////////////////////////////
//	call the Osiotraits copyFile
template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::copyTo(const File &toFile) const {
	return OSIOTraits::copyFile(this->getAbsolutePath().c_str(),toFile.getAbsolutePath().c_str());
}

/////////////////////////////////////////////////////////////////////////
//	calls OSIOtraits get temp file
template<typename OSIOTraits,typename char_type, typename Traits>
File<OSIOTraits,char_type,Traits> File<OSIOTraits,char_type,Traits>::getTempFile() {
	return File(OSIOTraits::getTempFile());
}

template<typename OSIOTraits,typename char_type, typename Traits>
ErrorType  File<OSIOTraits,char_type,Traits>::removeFromFS() {
	return OSIOTraits::removeFile(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::exists() const {
	return OSIOTraits::fileExists(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::makeReadOnly() const {
	return OSIOTraits::makeReadOnly(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::makeWriteable() const {
	return OSIOTraits::makeWriteable(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::makeExecutableUser() const {
	return OSIOTraits::makeExecutableUser(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::makeExecutableGroup() const {
	return OSIOTraits::makeExecutableGroup(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::makeExecutableAll() const {
	return OSIOTraits::makeExecutableAll(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
int File<OSIOTraits,char_type,Traits>::getSize() {
	return OSIOTraits::getSize(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::isReadOnly() const {
	return OSIOTraits::isReadOnly(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::isWriteable() const {
	return OSIOTraits::isWriteable(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::isExecutable() const {
	return OSIOTraits::isExecutable(getAbsolutePath().c_str());
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::getLastModificationTime(struct tm *t) const {
	return OSIOTraits::getFileTime(getAbsolutePath().c_str(),t,true);
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::getLastAccessTime(struct tm *t) const {
	return OSIOTraits::getFileTime(getAbsolutePath().c_str(),t,false);
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::setLastModificationTime(struct tm *t) {
	return OSIOTraits::setFileTime(getAbsolutePath().c_str(),t,true);
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::setLastAccessTime(struct tm *t) {
	return OSIOTraits::setFileTime(getAbsolutePath().c_str(),t,false);
}

template<typename OSIOTraits,typename char_type, typename Traits>
bool File<OSIOTraits,char_type,Traits>::rename(const FileType &file) {
	bool b = OSIOTraits::rename(getAbsolutePath().c_str(),file.getAbsolutePath().c_str());
	if(b) {
		(*this) = file;
	}
	return b;
}

}//io
#endif
