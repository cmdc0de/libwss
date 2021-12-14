
#include "properties.h"
#include "../utility/crc64.h"
#include "../utility/trim.h"
#include "../utility/logger.h"

using namespace wss;

uint64 Properties::calcStorageKey(const std::string &str) {
	return CRC64::String(str.c_str(),0);
}

Properties::Properties() : mLastFileNameHash(0), mValueMap() {
	clear();
}

Properties::~Properties() {
	clear();
}

bool Properties::contains(uint64 nameHash) const {
	return (mValueMap.find(nameHash)!=mValueMap.end());
}

void Properties::list(std::basic_ostream<char> &out) const {
	out << "Hash Value\tName\tValue" << std::endl;
	ValueMap::const_iterator it = mValueMap.begin();
	while(it!=mValueMap.end()) {
		out << (*it).first << "\t" << (*it).second.first << "\t" << (*it).second.second << std::endl;
		++it;
	}
}

bool Properties::loadIfModified(const std::string &fileName) {
	TFILE file(fileName);
	if(file.exists()) {
		struct tm lastModTime;
		file.getLastModificationTime(&lastModTime);
		if(mktime(&lastModTime)!=mktime(&mLastFileModificationTime)
			|| mLastFileNameHash!=calcStorageKey(file.getAbsolutePath())) {
			return load(file);
		}
	}
	return false;
}

bool Properties::load(const std::string &fileName) {
	TFILE file(fileName);
	return load(file);
}

bool Properties::load(const TFILE &file) {
	if(file.exists()) {
		std::ifstream in(file.getAbsolutePath().c_str());
		file.getLastModificationTime(&mLastFileModificationTime);
		mLastFileNameHash = calcStorageKey(file.getAbsolutePath());
		return load(in);
	}
	return false;
}

bool Properties::load(std::basic_istream<char> &in) {
	int lineNumber = 0;
	char line[MAX_PROPERTIES_LINE] = {'\0'};
	mValueMap.clear();
	while(in.good()) {
		lineNumber++;
		in.getline(line,MAX_PROPERTIES_LINE);
		if(line[0]!='#') {
			std::string s(line);
			std::string::size_type endPos = s.find("=",0);
			if(endPos!=std::string::npos) {
				std::string name = s.substr(0,endPos);
				std::string value;
				std::string::size_type newEndPos = s.find("\r",0);
				if(newEndPos!=std::string::npos) {
					value = s.substr((endPos+1), ((newEndPos-endPos)-1));
				}
				else {
					value = s.substr(endPos+1);
				}
				name = trim(name);
				value = trim(value);
				addProperty(name,value);
			}
			if(!in && !in.eof()) {
				WSSERRMSG("Error Reading file "  << lineNumber << " lines read.");
				return false;
			}
		}
	}
	return true;
}

bool Properties::save(std::basic_ostream<char> &out) const {
	ValueMap::const_iterator it = mValueMap.begin();
	for(;it!=mValueMap.end()  && out.good();++it) {
		out << (*it).second.first << "=" << (*it).second.second << std::endl;
	}
	return out.good();
}

bool Properties::save(const std::string &fileName) const {
	TFILE file(fileName);
	return save(file);
}

bool Properties::save(const TFILE &file) const {
	if(file.make(true)) {
		std::ofstream out(file.getAbsolutePath().c_str());
		return save(out);
	} else {
		WSSERRMSG("Can't create file " << file.getAbsolutePath());
		return false;
	}
}

bool Properties::saveSortedByName(std::basic_ostream<char> &out) const {
	typedef std::vector<std::pair<std::string,std::string> > NameValueSortVector;
	NameValueSortVector sortVector;
	ValueMap::const_iterator it = mValueMap.begin();
	for(;it!=mValueMap.end() && out.good();++it) {
		sortVector.push_back(std::pair<std::string,std::string>((*it).second.first, (*it).second.second));
	}
	std::sort(sortVector.begin(), sortVector.end());
	NameValueSortVector::const_iterator itV = sortVector.begin();
	for(;itV!=sortVector.end() && out.good();++itV) {
		out << (*itV).first << "=" << (*itV).second << std::endl;
	}
	return out.good();
}

bool Properties::saveSortedByName(const std::string &fileName) const {
	TFILE file(fileName);
	return saveSortedByName(file);
}

bool Properties::saveSortedByName(const TFILE &file) const {
	if(file.make(true)) {
		std::ofstream out(file.getAbsolutePath().c_str());
		return saveSortedByName(out);
	} else {
		WSSERRMSG("Can't create file " << file.getAbsolutePath());
		return false;
	}
}

void Properties::clear() {
	mValueMap.clear();
	mLastFileNameHash = 0;
	memset(&mLastFileModificationTime,0,sizeof(mLastFileModificationTime));
}
