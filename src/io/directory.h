
#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#include "ionode.h"
#include "file.h"
#include <string>
#include <vector>
#include <list>
#include <assert.h>

namespace wss { 
	/**
	*	@author Demetrius Comes
	*	@date 10/01/03
	*/
	template<typename OSIOTraits,typename char_type, typename Traits = std::char_traits<char_type> >
	class Directory : public IoNode
	{
	public:
		typedef std::basic_string<char_type,Traits> DirString;
		typedef Directory<OSIOTraits,char_type> CurrentDirType;
		typedef wss::File<OSIOTraits,char_type,Traits> FileType;
	public:
		/**
		* @date  10/1/2003 1:04:41 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isDirectory() const {return true;}
		/**
		* @date  10/1/2003 1:04:44 PM
		* @param  const DirString &strPath = DirString((const char_type *)"")
		* @param  bool bCreate = false
		*  
		*  
		*/
		Directory(const DirString &strPath = DirString((const char_type *)""),bool bCreate = false)
			: mstrPath(strPath) {
			setUpInitalPath(bCreate);
		}

		/**
		* @date  10/1/2003 1:04:51 PM
		* @param  const CurrentDirType &Parent
		* @param  const DirString &strPath
		* @param  bool bCreate = false
		*  
		*  
		*/
		Directory(const CurrentDirType &Parent, const DirString &strPath,bool bCreate = false) 
			: mstrPath(Parent.getPathWithSep() + strPath)
		{
			setUpInitalPath(bCreate);
		}

		/**
		* @date  10/1/2003 1:04:57 PM
		* @param  const Directory &d
		*  
		*  
		*/
		Directory(const Directory &d) : mstrPath(d.mstrPath) {}


		/**
		* @date  10/1/2003 1:05:00 PM
		* @return  Directory &
		* @param  const Directory &d
		*  
		*  
		*/
		Directory &operator=(const Directory &d) {mstrPath = d.mstrPath;return *this;}

		/**
		* @date  10/1/2003 1:05:07 PM
		* @return  bool 
		* @param  const Directory &d
		*  
		*  
		*/
		bool operator==(const Directory &d) {return mstrPath==d.mstrPath;}

		/**
		* @date  10/1/2003 1:05:11 PM
		* @return  bool 
		* @param  const Directory &d
		*  
		*  
		*/
		bool operator!=(const Directory &d) {return !operator==(d);}

		/**
		* @date  10/1/2003 1:05:32 PM
		* @return  ErrorType 
		* @param  IoNode *pNode
		*  
		*  
		*/
		ErrorType removeFile(IoNode *pNode) {
			return pNode->removeFromFS();
		}

		/**
		* @date  10/1/2003 1:05:38 PM
		* @return  bool 
		*  
		*  
		*/
		bool exists() const
		{
			return OSIOTraits::exists(mstrPath.c_str());
		}

		/**
		* @date  10/1/2003 1:05:42 PM
		* @return  bool 
		* @param  bool bRecurse
		*  
		*  returns true if directory already exists
		*/
		bool make(bool bRecurse) const {
			if(bRecurse) {
				CurrentDirType d(getParentDirectory());
				if(!d.exists())	{
					d.make(true);
				}
			}
			if(exists()) {
				return true;
			}
			return OSIOTraits::createDirectory(mstrPath.c_str());
		}

		/**
		* @date  10/1/2003 1:05:47 PM
		* @return  Directory<OSIOTraits,char_type> 
		*  
		*  
		*/
		static Directory<OSIOTraits,char_type> getTempDir() {
			DirString s;
			OSIOTraits::getTempDirectory(s);
			return Directory<OSIOTraits,char_type>(s);
		}

		/**
		* @date  10/1/2003 1:05:50 PM
		* @return  bool 
		*  
		*  
		*/
		bool isEmpty() {
			DirString s(this->getPathNoEndSep());
			return OSIOTraits::isEmpty(s.c_str());
		}

		/**
		* @date  10/1/2003 1:05:54 PM
		* @return  DirString 
		* @param  const DirString &strFileName
		*  
		*  
		*/
		DirString makeFullPath(const DirString &strFileName) const {
			DirString s(mstrPath);
			s+=OSIOTraits::PATH_SEP;
			s+=strFileName;
			return s;
		}

		/**
		* @date  10/1/2003 1:05:57 PM
		* @return  const DirString 
		*  
		*  
		*/
		const DirString &getPathNoEndSep() const {
			return mstrPath;
		}

		/**
		* @date  10/1/2003 1:06:00 PM
		* @return  DirString 
		*  
		*  
		*/
		DirString getPathWithSep() const {
			return mstrPath+OSIOTraits::PATH_SEP;
		}

		/**
		* @date  10/1/2003 1:06:03 PM
		* @return  bool
		*  
		*  if file does not exists this still returns true
		*/
		virtual ErrorType removeFromFS() {
			if(exists()) {
				return OSIOTraits::removeDirectory(mstrPath.c_str());
			}
			return ErrorType();
		}

		/**
		* @date  10/1/2003 1:06:07 PM
		* @return  ErrorType 
		*  
		*  
		*/
		ErrorType recursiveRemoveFromFS()  {
			return OSIOTraits::recursiveRemoveFromFS(mstrPath.c_str());			
		}

		/**
		* @date  10/1/2003 1:06:10 PM
		* @return  bool 
		* @param  std::vector<FileType > &files
		*  
		*  
		*/
		bool getFiles(std::vector<FileType > &files) {
			return getFiles(files,OSIOTraits::ALL_FILES_WILDCARD);
		}

		/**
		* @date  10/1/2003 1:06:14 PM
		* @return  bool 
		* @param  std::vector<FileType > &files
		* @param  const char_type *filter
		*  
		*  
		*/
		bool getFiles(std::vector<FileType > &files, const char_type *filter) {
			std::list<DirString> fileNames;
			if(OSIOTraits::getFiles(fileNames,mstrPath.c_str(),filter)) {
				for(typename std::list<DirString>::iterator it = fileNames.begin();it!=fileNames.end();++it)	{
					files.insert(files.begin(),File<OSIOTraits,char_type>(*this,(*it)));
				}
				return true;
			}
			return false;
		}
		/**
		* @date  2/2/2004 9:47:27 AM
		* @return  bool 
		* @param  std::vector<DirString> &files
		*  
		*  
		*/
		bool getFilesAsStrings(std::vector<DirString> &files) {
			return OSIOTraits::getFiles(files,mstrPath.c_str(),OSIOTraits::ALL_FILES_WILDCARD);
		}

		/**
		* @date  10/1/2003 1:06:26 PM
		* @return  bool 
		* @param  std::vector<CurrentDirType > &dirs
		*  
		*  
		*/
		bool getDirectories(std::vector<CurrentDirType > &dirs) {
			std::list<DirString> dirNames;
			if(OSIOTraits::getDirectories(mstrPath.c_str(),dirNames)) {
				DirString shortName(getDirectoryShortName());
				Directory Parent(*this);
				Directory P(getParentDirectory());
				Parent=P;
				DirString s;
				s=mstrPath+OSIOTraits::PATH_SEP;
				for(typename std::list<DirString>::iterator it = dirNames.begin();it!=dirNames.end();++it) {
					if(shortName!=(*it)) {
						Directory tDir(s+(*it));
						if(tDir!=*this && tDir!=Parent) {
							dirs.insert(dirs.begin(),tDir);
						}
					}
				}
			} else {
				return false;
			}
		}

		/**
		* @date  10/1/2003 1:06:34 PM
		* @return  DirString 
		*  
		*  
		*/
		DirString getDirectoryShortName() {
			typename DirString::size_type n(mstrPath.find_last_of(OSIOTraits::PATH_SEP));
			//not found
			if(n==DirString::npos) 	{
				return DirString();
			} else {
				return DirString(mstrPath.substr(n+1,mstrPath.size()));
			}
		}

		/**
		* @date  10/1/2003 1:06:41 PM
		* @return  Directory 
		*  
		*  
		*/
		Directory getParentDirectory() const {
			//check first to see if directory is the root of the file system
			if(OSIOTraits::isRootOfFileSystem(mstrPath.c_str())) {
				return (*this);
			}
			typename DirString::size_type n(mstrPath.find_last_of(OSIOTraits::PATH_SEP));
			return Directory(mstrPath.substr(0,n));
		}

		/**
		* @date  10/1/2003 1:07:52 PM
		* @return  bool 
		*  
		*  
		*/
		bool isRootDirectory()  {
			return OSIOTraits::isRootOfFileSystem(mstrPath.c_str());
		}
		
		/**
		* @date  10/1/2003 1:07:58 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeReadOnly() const {
			return OSIOTraits::makeReadOnly(mstrPath.c_str());
		}
		/**
		* @date  10/1/2003 1:08:04 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeWriteable() const {
			return OSIOTraits::makeWriteable(mstrPath.c_str());
		}
		/**
		* @date  2/17/2005 3:53:37 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableUser() const {
			return OSIOTraits::makeExecutableUser(mstrPath.c_str());
		}
		/**
		* @date  2/17/2005 3:53:38 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableGroup() const {
			return OSIOTraits::makeExecutableGroup(mstrPath.c_str());
		}
		/**
		* @date  2/17/2005 3:53:39 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool makeExecutableAll() const {
			return OSIOTraits::makeExecutableAll(mstrPath.c_str());
		}
		/**
		* @date  10/1/2003 1:08:10 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isReadOnly() const {
			return OSIOTraits::isReadOnly(mstrPath.c_str());
		}

		/**
		* @date  10/1/2003 1:08:16 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isWriteable() const {
			return OSIOTraits::isWriteable(mstrPath.c_str());
		}
		/**
		* @date  2/17/2005 3:55:23 PM
		* @return  bool 
		*  
		*  
		*/
		virtual bool isExecutable() const {
			return OSIOTraits::isExecutable(mstrPath.c_str());
		}
	private:
		/**
		* @date  10/1/2003 1:08:25 PM
		* @return  void 
		* @param  bool bCreate
		*  
		*  This is just a convience so Ctor overloads don't have to retype same code.
		*	This function assumes mstrPath has been populated!
		*/
		void setUpInitalPath(bool bCreate) {
			if(mstrPath.empty()) {
				OSIOTraits::getCurrentDirectory(mstrPath);
			} else {
				if(!OSIOTraits::isAbsolutePath(mstrPath.c_str()))	{
					DirString strTemp;
					OSIOTraits::getCurrentDirectory(strTemp);
					mstrPath = strTemp + OSIOTraits::PATH_SEP + mstrPath;
				}
			}
			if(exists()) {
				DirString strFile;
				OSIOTraits::getFullPathName(mstrPath.c_str(),mstrPath,strFile);
			} else {
				if(!OSIOTraits::fileExists(mstrPath.c_str())) {
					if(bCreate) {
						make(true);
						DirString strFile;
						OSIOTraits::getFullPathName(mstrPath.c_str(),mstrPath,strFile);
					}
				}
			}
			if(mstrPath.size()>1 && mstrPath[mstrPath.size()-1]==OSIOTraits::PATH_SEP_CHAR) 	{
				mstrPath = mstrPath.substr(0,mstrPath.size()-1);
			}
		}
	private:
		DirString mstrPath;
	};
} //io
#endif
