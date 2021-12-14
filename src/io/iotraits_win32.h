#ifdef WSS_WINDOWS
#ifndef WIN32_IO_TRAITS_H
#define WIN32_IO_TRAITS_H

#include <tchar.h>
#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>
#include "../xtl/case_insensitive.h"
#include "../platform/platform_time.h"

namespace wss { 
	/**
	*	class name Win32IoTraits
	*	@author Demetrius Comes
	*	@date 10-01-03
	*	Traits class to support OS IO specific operations
	*/
template<typename char_type>
struct Win32IOTraits
{
	///convient typedef for strings
	typedef std::basic_string<char_type> string_type;
	///convient typedef for case insensitive strings
	typedef std::basic_string<char_type,case_insensitive_traits<char_type> > case_is_string_type;
	static const char_type *PATH_SEP;
	static const char_type *ALL_FILES_WILDCARD;
	static const char_type PATH_SEP_CHAR;

	/**
	* @date  10/19/2005 12:26:45 PM
	* @return  bool 
	* @param  char *
	* @param  char *
	*
	*  return true if successful
	*/
	static bool rename(const char *from, const char *to) {
		return (0==::rename(from,to));
	}

	/**
	* @date  10/1/2003 12:26:45 PM
	* @return  bool 
	* @param  string_type &s
	*  
	*  return true if successful
	*/
	static bool getCurrentDirectory(string_type &s) {
		char_type buf[MAX_PATH];
		memset(buf,0,sizeof(char_type)*MAX_PATH);
		if(::GetCurrentDirectory(MAX_PATH,(char_type *)buf)!=0)	{
			s=buf;
		} else {
			return false;
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:27:05 PM
	* @return  bool 
	* @param  const TCHAR *lpFileName
	* @param  string_type &path
	* @param  string_type &file
	*  
	*  
	*/
	static bool getFullPathName(const TCHAR *lpFileName, string_type &path, string_type &file) {
		char_type buf[MAX_PATH];
		memset(buf,0,sizeof(char_type)*MAX_PATH);
		char_type *buffile[MAX_PATH];
		if(::GetFullPathName(lpFileName,MAX_PATH,buf,(char_type**)buffile)!=0) {
			path=buf;
			if((*buffile)==0) {
				file = "";
			} else {
				file=(*buffile);
			}
		} else {
			return false;
		}
		return true;
	}
	/**
	* @date  10/1/2003 12:27:13 PM
	* @return  ErrorType 
	* @param  const TCHAR * lpPath
	*  
	*  
	*/
	static ErrorType removeDirectory(const TCHAR * lpPath) {
		if(::RemoveDirectory(lpPath)==0) {
			return ErrorType(ErrorType::FacilityOS,::GetLastError());
		}
		return ErrorType();
	}

	/**
	* @date  10/1/2003 12:27:17 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  returns true if file exists
	*/
	static bool exists(const TCHAR *path) {
		DWORD dwFileInfo (::GetFileAttributes(path));
		if(dwFileInfo!=0xFFFFFFFF && dwFileInfo & FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}
		return false;
	}	
	
	/**
	* @date  10/1/2003 12:27:34 PM
	* @return  bool 
	* @param  const TCHAR * file
	*  
	*  
	*/
	static bool fileExists(const TCHAR * file) {
		DWORD dwFileInfo (::GetFileAttributes(file));
		if(dwFileInfo!=INVALID_FILE_ATTRIBUTES) {
			return true;
		}
		return false;
	}
	
	/**
	* @date  10/1/2003 12:27:40 PM
	* @return  bool 
	* @param  const TCHAR * lpPathName
	*  
	*  
	*/
	static bool createDirectory(const TCHAR * lpPathName) 
	{
		if(::CreateDirectory(lpPathName,0)==FALSE) {
			return false;
		}
		return true;
	} 
	
	/**
	* @date  10/1/2003 12:27:47 PM
	* @return  bool 
	* @param  string_type &s
	*  
	*  
	*/
	static bool getTempDirectory(string_type &s) {
		char_type buf[MAX_PATH];
		memset(buf,0,sizeof(char_type)*MAX_PATH);
		if(::GetTempPath(MAX_PATH,buf)==0) {
			return false;
		}
		s = buf;
		return true;
	}

	/**
	* @date  10/1/2003 12:27:52 PM
	* @return  bool 
	* @param  const TCHAR * lpDirName
	*  
	*  
	*/
	static bool isEmpty(const TCHAR * lpDirName)
	{
		string_type strDirName(lpDirName);
		strDirName+=PATH_SEP;
		strDirName+=ALL_FILES_WILDCARD;
		WIN32_FIND_DATA FindFileData;
		HANDLE h = ::FindFirstFile(strDirName.c_str(),&FindFileData);
		const TCHAR *CURRENT_DIR=".";
		const TCHAR *BACK_DIR="..";
		while(h!=INVALID_HANDLE_VALUE) {
			case_is_string_type s(FindFileData.cFileName);
			if(s!=CURRENT_DIR && s!=BACK_DIR) {
				::FindClose(h);
				return false;
			}
			if(::FindNextFile(h,&FindFileData)==0) {
				::FindClose(h);
				h = INVALID_HANDLE_VALUE;
			}
		}
		::FindClose(h);
		return true;
	}

	/**
	* @date  10/1/2003 12:27:58 PM
	* @return  bool 
	* @param  const TCHAR * lpFrom
	* @param  const TCHAR * lpTo
	*  
	*  
	*/
	static bool copyFile(const TCHAR * lpFrom, const TCHAR * lpTo) {
		if(::CopyFile(lpFrom,lpTo,false)==0){
			return false;
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:28:03 PM
	* @return  string_type 
	*  
	*  
	*/
	static string_type getTempFile()
	{
		//this function only accepts ascii chars
		char_type pathBuf[MAX_PATH];
		char_type fileBuf[MAX_PATH];
		::GetTempPath(MAX_PATH,pathBuf);
		if(::GetTempFileName(pathBuf,(char_type *)"IT",0,fileBuf))
		{
			return string_type(fileBuf);
		}
		return string_type((const char_type *)"");
	}

	/**
	* @date  10/1/2003 12:28:03 PM
	* @return  bool 
	*  @param std::vector<string_type> &files
	*  @param const TCHAR * lpDir
	*  @param const TCHAR *filter
	*
	*	populates the std::vector with file names in this directory
	*/
	static bool getFiles(std::vector<string_type> &files,const TCHAR * lpDir, const TCHAR *filter)	{
		string_type strDir(lpDir);
		strDir+=PATH_SEP,strDir+=filter;
		WIN32_FIND_DATA FindFileData;
		HANDLE h = ::FindFirstFile(strDir.c_str(),&FindFileData);
		while(h!=INVALID_HANDLE_VALUE) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))	{
				files.insert(files.end(),string_type(FindFileData.cFileName));
			}
			if(::FindNextFile(h,&FindFileData)==0) {
				h = INVALID_HANDLE_VALUE;
				if(::GetLastError()!=ERROR_NO_MORE_FILES) {
					return false;
				}
			}
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:29:17 PM
	* @return  ErrorType 
	* @param  const TCHAR * lfile
	*  
	*  
	*/
	static ErrorType removeFile(const TCHAR * lfile) {
		if(::DeleteFile(lfile)==0) {
			return ErrorType(ErrorType::FacilityOS,::GetLastError());
		}
		return ErrorType();
	}

	/**
	* @date  10/1/2003 12:29:23 PM
	* @return  bool 
	* @param  const TCHAR *lpDir
	* @param  std::vector<string_type> &dirs
	*  
	*  
	*/
	static bool getDirectories(const TCHAR *lpDir, std::vector<string_type> &dirs) {
		string_type dir(lpDir);
		dir+=PATH_SEP,dir+=ALL_FILES_WILDCARD;
		WIN32_FIND_DATA FindFileData;
		HANDLE h = ::FindFirstFile(dir.c_str(),&FindFileData);
		while(h!=INVALID_HANDLE_VALUE) 	{
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				dirs.insert(dirs.end(),string_type(FindFileData.cFileName));
			}
			if(::FindNextFile(h,&FindFileData)==0) {
				h = INVALID_HANDLE_VALUE;
				if(::GetLastError()!=ERROR_NO_MORE_FILES) {
					return false;
				}
			}
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:29:30 PM
	* @return  bool 
	* @param  const TCHAR *strPath
	*  
	*  
	*/
	static bool isRootOfFileSystem(const TCHAR *strPath) 
	{
		//32 drives times 4 bytes for each drive
		const int STRLEN = 32*4; 
		char buf[STRLEN];
		int Ret;
		if((Ret=::GetLogicalDriveStrings(STRLEN,buf))==0) {
			return false;
		}
		int nCounter = 0;
		case_is_string_type sp(strPath);
		if(sp[sp.size()-1]!=PATH_SEP_CHAR)
		{
			sp+=PATH_SEP;
		}
		while(nCounter<Ret)
		{
			if(sp==(buf+nCounter))
			{
				return true;
			}
			nCounter++;
		}
		return false;
	}

	/**
	* @date  10/1/2003 12:29:36 PM
	* @return  bool 
	* @param  const TCHAR *strPath
	*  
	*  
	*/
	static bool isAbsolutePath(const TCHAR *strPath)
	{
		case_is_string_type sp(strPath);
		const TCHAR *DRIVE_SEP = ":";
		case_is_string_type::size_type n(sp.find_first_of(DRIVE_SEP));
		if(n!=case_is_string_type::npos) {	
			sp = sp.substr(0,n+1);
			sp+=PATH_SEP;
			return isRootOfFileSystem(sp.c_str());
		}
		return false;
	}

	/**
	* @date  10/1/2003 12:29:42 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static bool makeReadOnly(const TCHAR *path) 
	{
		if(::_tchmod(path,_S_IREAD)==-1) {
			return false;
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:29:47 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static bool makeWriteable(const TCHAR *path) {
		if(::_tchmod(path,_S_IREAD | _S_IWRITE)==-1) {
			return false;
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:29:53 PM
	* @return  int 
	* @param  const TCHAR *path
	*  
	*  returns -1 on error
	*/
	static int getSize(const TCHAR *path) {
		struct _stat buf;
		if(_tstat( path, &buf )!=0) {
			return -1;
		}
		return buf.st_size;
	}
	/**
	* @date  10/1/2003 12:30:07 PM
	* @return  bool 
	* @param  const TCHAR *path
	* @param  struct tm *t
	* @param  bool bModificationTime
	*  
	*  if bModificationTime is true we get the modification time
	*	if false we get the last access time
	*/
	static bool getFileTime(const TCHAR *path, struct tm *t, bool bModificationTime) {
		struct _stat buf;
		if(_tstat(path, &buf)!=0) {
			return false;
		}
		if(bModificationTime) {
			return wss::platform::localtime(t, &buf.st_mtime) == 0;
		} else {
			return wss::platform::localtime(t, &buf.st_atime) == 0;
		}
	}

	/**
	* @date  10/1/2003 12:30:32 PM
	* @return  bool 
	* @param  const TCHAR *path
	* @param  struct tm *t
	* @param  bool bModificationTime
	*  
	*  
	*/
	static bool setFileTime(const TCHAR *path, struct tm *t, bool bModificationTime) {
		/*
		struct utimbuf {
        time_t actime;          // access time 
        time_t modtime;         // modification time 
        };*/
		//ansi
		_utimbuf timebuf;
		struct tm otherTime;
		if(bModificationTime) {
			getFileTime(path,&otherTime,false);
			if(t==0) {
				timebuf.modtime = ::time(0);
			} else {
				timebuf.modtime = ::mktime(t);
			}
			timebuf.actime = ::mktime(&otherTime);
		} else {
			getFileTime(path,&otherTime,true);
			if(t==0) {
				timebuf.actime = ::time(0);
			} else {
				timebuf.actime = ::mktime(t);
			}
			timebuf.modtime = ::mktime(&otherTime);
		}
		if(_tutime(path,&timebuf)!=0) {
			return false;
		}
		return true;
	}

	/**
	* @date  10/1/2003 12:30:38 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static bool isReadOnly(const TCHAR *path) {
		//04 Read permission 
		//02  Write permission 
		return _taccess(path,2)!=0;
	}

	/**
	* @date  10/1/2003 12:30:44 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static bool isWriteable(const TCHAR *path) {
		return !isReadOnly(path);
	}
	/**
	* @date  2/17/2005 4:11:02 PM
	* @return  bool 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static bool isExecutable(const TCHAR *path) {
		size_t l = strlen(path);
		if(l>3) {
			if(strcmp(".exe",&path[l-4])==0) {
				return true;
			}
		}
		return false;
	}

	/**
	* @date  10/1/2003 12:30:49 PM
	* @return  ErrorType 
	* @param  const TCHAR *path
	*  
	*  
	*/
	static ErrorType recursiveRemoveFromFS(const TCHAR *path) {
		string_type str = path;
	    str += "/*";
		WIN32_FIND_DATA wfd;
		HANDLE h = FindFirstFile( str.c_str(), &wfd);
		if (h) {
			do {
				if (!_tcscmp( wfd.cFileName, _TEXT( ".")) ||
					!_tcscmp( wfd.cFileName, _TEXT( "..")))
					continue;
	
				string_type str2 = path;
				str2 += '\\';
				str2 += wfd.cFileName;
				ErrorType et;
				if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) {
					//Recursion be us
					if(!(et = Win32IOTraits::recursiveRemoveFromFS( str2.c_str())))
						return et;
				} else {
					if(!(et=Win32IOTraits::removeFile( str2.c_str()))) 
						return et;
				}				
			}while( FindNextFile( h, &wfd));
			FindClose( h);
		}
		return Win32IOTraits::removeDirectory(path);
	}
	/**
	* @date  2/17/2005 2:11:08 PM
	* @return  bool 
	* @param  const char_type *path
	*  
	*  Can't be implemented!!!!
	*/
	static bool makeExecutableUser(const char_type * /*path*/) {
		return true;
	}
	/**
	* @date  2/17/2005 2:11:10 PM
	* @return  bool 
	* @param  const char_type *path
	*  
	*  Can't be implemented!!!!
	*/
	static bool makeExecutableGroup(const char_type * /*path*/) {
		return true;
	}
	/**
	* @date  2/17/2005 2:15:42 PM
	* @return  bool 
	* @param  const char_type *path
	*  
	*  Can't be implemented!!!!
	*/
	static bool makeExecutableAll(const char_type * /*path*/) {
		return true;
	}
}; //end Win32IOTraits
} //io
#endif
#endif
