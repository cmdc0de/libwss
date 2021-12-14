#if defined(WSS_LINUX) || defined(WSS_MAC)
#ifndef WSS_LINUX_IO_TRAITS_H
#define WSS_LINUX_IO_TRAITS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>

namespace wss {
	/**
	*	class name LinuxIOTraits
	*	@author Demetrius Comes
	*	@date 10-8-03
	*	Traits class to support OS IO specific operations
	*/
template<typename char_type>
struct LinuxIOTraits
{
	///convient typedef for strings
	typedef std::basic_string<char_type> string_type;
	///convient typedef for case insensitive strings
	static const char_type *PATH_SEP;
	static const char_type *ALL_FILES_WILDCARD;
	static const char_type PATH_SEP_CHAR;

	/**
	* @date  10/19/2005 12:26:45 PM
	* @return  bool
	* @param  char *from
	* @param  char *to
	*
	*  return true if successful
	*/
	static bool rename(const char *from, const char *to) {
		return (0==::rename(from,to));
	}

	/**
	* @date  10/8/2003 10:05:31 AM
	* @return  bool
	* @param  string_type &s
	*
	*
	*/
	static bool getCurrentDirectory(string_type &s) {
		char_type buf[PATH_MAX];
		::memset(buf,0,sizeof(char_type)*PATH_MAX);
		if(::getcwd((char_type *)buf,PATH_MAX)!=0) {
			s=buf;
			return true;
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:05:34 AM
	* @return  bool
	* @param  const char_type *lpFileName
	* @param  string_type &path
	* @param  string_type &file
	*
	*
	*/
	static bool getFullPathName(const char_type *lpFileName, string_type &path, string_type &file) {
		char_type buf[PATH_MAX];
		memset(buf,0,sizeof(char_type)*PATH_MAX);
		if(::realpath(lpFileName,buf)!=NULL) {
			path=buf;
			return true;
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:05:39 AM
	* @return  ErrorType
	* @param  const char_type * lpPath
	*
	*
	*/
	static ErrorType removeDirectory(const char_type * lpPath) {
		if(::remove(lpPath)!=0)	{
			return ErrorType(ErrorType::FacilityOS,errno);
		}
		return ErrorType();
	}

	/**
	* @date  10/8/2003 10:05:41 AM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool exists(const char_type *path) {
		struct stat statbuf;
		if(stat((path), &statbuf)==0) {
			if(statbuf.st_mode & S_IFDIR) {
				return true;
			}
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:05:47 AM
	* @return  bool
	* @param  const char_type * file
	*
	*
	*/
	static bool fileExists(const char_type * file) {
		struct stat statbuf;
		return (stat(file, &statbuf)==0) ? true : false;
	}

	/**
	* @date  10/8/2003 10:05:50 AM
	* @return  bool
	* @param  const char_type * lpPathName
	*
	*
	*/
	static bool createDirectory(const char_type * lpPathName) {
		if(::mkdir(lpPathName,S_IRWXU)!=0) {
			return false;
		}
		return true;
	}

	/**
	* @date  10/17/2003 2:40:18 PM
	* @return  bool
	* @param  string_type &s
	*
	*
	*/
	static bool getTempDirectory(string_type &s) {
		s = P_tmpdir;
		return true;
	}

	/**
	* @date  10/8/2003 10:06:00 AM
	* @return  bool
	* @param  const char_type * lpDirName
	*
	*
	*/
	static bool isEmpty(const char_type * lpDirName) {
		const char_type *CURRENT_DIR=".";
		const char_type *BACK_DIR="..";
		DIR *pDir = opendir(lpDirName);
		if(pDir!=0) {
			struct dirent *pDirent;
			while((pDirent=readdir(pDir))!=NULL) {
				string_type s(pDirent->d_name);
				if(s!=CURRENT_DIR && s!=BACK_DIR) {
					::closedir(pDir);
					return false;
				}
			}
			::closedir(pDir);
			return true;
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:06:04 AM
	* @return  bool
	* @param  const char_type * lpFrom
	* @param  const char_type * lpTo
	*
	*
	*/
	static bool copyFile(const char_type * lpFrom, const char_type * lpTo) {
		std::ifstream from(lpFrom);
		std::ofstream to(lpTo);
		if(from.good() && to.good()) {
			const unsigned int BUFSIZE=2048;
			char buf[BUFSIZE];
			while(from.good()) {
				from.read(buf,BUFSIZE);
				to.write(buf,from.gcount());
			}
			from.close();
			to.close();
			if(from.eof() && to.good()) {
				return true;
			}
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:06:09 AM
	* @return  string_type
	*
	*
	*/
	static string_type getTempFile() {
		const char *FixedTemplate = "/tmp/ITXXXXXX";
		char FileName[256];
		snprintf((char *)&FileName[0],sizeof(FileName),"%s",FixedTemplate);
		int fd = ::mkstemp((char *)&FileName[0]);
		if(fd==-1) {
			::close(fd);
			return string_type((const char *)"");
		}
		return string_type((const char *)&FileName[0]);
	}

	/**
	* @date  10/8/2003 10:06:13 AM
	* @return  bool
	* @param  std::vector<string_type> &files
	* @param  const char_type * lpDir
	* @param const char_type * filter
	*
	*	TODO - revisit this
	*	I pretty sure the filtering does not work as one would 100% expect.
	*	i.e. if you try you can break it
	*/
	static bool getFiles(std::vector<string_type> &files,const char_type * lpDir, const char_type *filter) {
		bool retVal = true;
		DIR *pDir = opendir(lpDir);
		struct dirent *pDirent;
		if(pDir) {
			string_type cmpString(filter);
			bool bAllFiles = (filter==0 || strcmp(ALL_FILES_WILDCARD,filter)==0);
			if(!bAllFiles) {
				typename string_type::size_type n = cmpString.find("*");
				if(n!=string_type::npos) {
					cmpString = cmpString.substr(n+1);
				}
			}
			char fileName[PATH_MAX];
			strncpy(fileName,lpDir,PATH_MAX-2);
			int pos = strnlen(lpDir,PATH_MAX-2);
			fileName[pos++] = '/';
			while ((pDirent = readdir( pDir))) {
				struct stat st;
				strncpy(&fileName[pos],pDirent->d_name,PATH_MAX-pos);
				if(stat((const char *)fileName, &st)==0) {
					if (!(st.st_mode & S_IFDIR)) {
						if(bAllFiles || strstr(pDirent->d_name,cmpString.c_str())) {
							files.insert(files.end(),string_type(pDirent->d_name));
						}
					}
				} else	{
					//const char *p = strerror(errno);
					//DBGMSG(p);
					retVal = false;
					break;
				}
			}
			closedir(pDir);
		} else {
			retVal = false;
		}
		return retVal;
	}

	/**
	* @date  10/8/2003 10:06:18 AM
	* @return  ErrroType
	* @param  const char_type * lfile
	*
	*
	*/
	static ErrorType removeFile(const char_type * lfile) {
		if(::remove(lfile)!=0) {
			return ErrorType(ErrorType::FacilityOS,errno);
		}
		return ErrorType();
	}

	/**
	* @date  10/8/2003 10:06:20 AM
	* @return  bool
	* @param  const char_type *lpDir
	* @param  std::vector<string_type> &dirs
	*
	*
	*/
	static bool getDirectories(const char_type *lpDir, std::vector<string_type> &dirs) {
		bool bRetVal = true;
		DIR *pDir = opendir(lpDir);
        struct dirent *pDirent;
		if(pDir) {
			char fileName[PATH_MAX];
			strncpy(fileName,lpDir,PATH_MAX-2);
			int pos = strnlen(lpDir,PATH_MAX-2);
			fileName[pos++] = '/';
			while ((pDirent = readdir( pDir)))  {
					struct stat st;
					strncpy(&fileName[pos],pDirent->d_name,PATH_MAX-pos);
					if(stat((const char *)fileName, &st)==0) {
							if (st.st_mode & S_IFDIR) {
									dirs.insert(dirs.end(),string_type(pDirent->d_name));
							}
					} else {
						bRetVal = false;
						break;
					}
			}
			closedir(pDir);
		} else {
			bRetVal = false;
		}
		return bRetVal;
	}

	/**
	* @date  10/8/2003 10:06:23 AM
	* @return  bool
	* @param  const char_type *strPath
	*
	*
	*/
	static bool isRootOfFileSystem(const char_type *strPath) {
		std::basic_string<char_type> s(strPath);
		if(s==PATH_SEP) {
			return true;
		}
		return false;
	}

	/**
	* @date  10/8/2003 10:06:26 AM
	* @return  bool
	* @param  const char_type *strPath
	*
	*
	*/
	static bool isAbsolutePath(const char_type *strPath) {
		return (strPath!=0 && strPath[0]==PATH_SEP_CHAR);
	}

	/**
	* @date  10/8/2003 10:06:27 AM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool makeReadOnly(const char_type *path) {
		return (::chmod(path,S_IRUSR | S_IRGRP | S_IROTH)==0);
	}

	/**
	* @date  10/8/2003 10:06:29 AM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool makeWriteable(const char_type *path) {
		struct stat buf;
		if(::stat(path,&buf)==0) {
			if(::chmod(path,buf.st_mode | S_IWUSR | S_IWGRP | S_IWOTH)==0) {
				return true;
			}
		}
		return false;
	}
	/**
	* @date  2/17/2005 2:11:08 PM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool makeExecutableUser(const char_type *path) {
		struct stat buf;
		if(::stat(path,&buf)==0) {
			if(::chmod(path,buf.st_mode | S_IXUSR)==0) {
				return true;
			}
		}
		return false;
	}
	/**
	* @date  2/17/2005 2:11:10 PM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool makeExecutableGroup(const char_type *path) {
		struct stat buf;
		if(::stat(path,&buf)==0) {
			if(::chmod(path,buf.st_mode | S_IXGRP)==0) {
				return true;
			}
		}
		return false;
	}
	/**
	* @date  2/17/2005 2:15:42 PM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool makeExecutableAll(const char_type *path) {
		struct stat buf;
		if(::stat(path,&buf)==0) {
			if(::chmod(path,buf.st_mode | S_IXOTH)==0) {
				return true;
			}
		}
		return false;
	}
	/**
	* @date  10/8/2003 10:06:32 AM
	* @return  int
	* @param  const char_type *path
	*
	*
	*/
	static int getSize(const char_type *path)
	{
		struct stat buf;
		if(stat( path, &buf )!=0) {
			return -1;
		}
		return buf.st_size;
	}

	/**
	* @date  10/8/2003 10:06:35 AM
	* @return  bool
	* @param  const char_type *path
	* @param  struct tm *t
	* @param  bool bModificationTime
	*
	*  if bModificationTime is true we get the modification time
	*	if false we get the last access time
	*/
	static bool getFileTime(const char_type *path, struct tm *t, bool bModificationTime) {
		struct stat buf;
		if(stat(path, &buf)!=0)	{
			return false;
		}
		if(bModificationTime) {
			memcpy(t,localtime(&buf.st_mtime),sizeof(struct tm));
		} else {
			memcpy(t,localtime(&buf.st_atime),sizeof(struct tm));
		}
		return true;
	}

	/**
	* @date  10/8/2003 10:06:48 AM
	* @return  bool
	* @param  const char_type *path
	* @param  struct tm *t
	* @param  bool bModificationTime
	*
	*  if bModificationTime is true we set the modification time
	*	if false we set the last access time
	*/
	static bool setFileTime(const char_type *path, struct tm *t, bool bModificationTime) {
		/* struct utimbuf { time_t actime;          // access time
		        time_t modtime;         // modification time };*/
		utimbuf timebuf;
		struct stat buf;
		if(stat(path,&buf)!=0) {
			return false;
		} else {
			if(bModificationTime) {
				if(t==0) {
					timebuf.modtime = ::time(0);
				} else {
					timebuf.modtime = ::mktime(t);
				}
				timebuf.actime = buf.st_atime;
			} else {
				if(t==0) {
					timebuf.actime = ::time(0);
				} else {
					timebuf.actime = ::mktime(t);
				}
				timebuf.modtime = buf.st_mtime;
			}
			if(utime(path,&timebuf)!=0) {
				return false;
			}
		}
		return true;
	}

	/**
	* @date  10/8/2003 10:07:02 AM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool isReadOnly(const char_type *path) {
		//04 Read permission
		//02  Write permission
		return access(path,W_OK)!=0;
	}

	/**
	* @date  10/8/2003 10:07:05 AM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool isWriteable(const char_type *path) {
		return !isReadOnly(path);
	}
	/**
	* @date  2/17/2005 2:14:56 PM
	* @return  bool
	* @param  const char_type *path
	*
	*
	*/
	static bool isExecutable(const char_type *path) {
		return (access(path,X_OK)==0);
	}

	/**
	* @date  10/8/2003 10:07:07 AM
	* @return  ErrorType
	* @param  const char_type *path
	*
	*
	*/
	static ErrorType recursiveRemoveFromFS(const char_type *path) {
		DIR* pdir = opendir( path );
		ErrorType retVal;
		if (pdir) {
			struct dirent *pdirent;
			while ((pdirent = readdir( pdir))) {
				if (!strcmp( pdirent->d_name, ".") ||
					!strcmp( pdirent->d_name, ".."))
					continue;

				std::basic_string<char_type> str = path;
				str += PATH_SEP;
				str += pdirent->d_name;

				struct stat st;
				stat( str.c_str(), &st);
				if (st.st_mode & S_IFDIR) {
					if(!(retVal = recursiveRemoveFromFS( str.c_str()))) {
						break;
					}
				} else {
					if(!(retVal = removeFile(str.c_str()))) {
						break;
					}
				}
			}
			closedir(pdir);
		}
		if(retVal.ok()) {
            if(0!=::rmdir(path)) {
				retVal = ErrorType(ErrorType::FacilityOS,errno);
			}
		}
		return retVal;
	}
}; //end
} //
#endif
#endif
