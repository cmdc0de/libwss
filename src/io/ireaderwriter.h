#ifndef WSS_IREADERWRITER_H
#define WSS_IREADERWRITER_H

#include "io_typedefs.h"
#include <memory>

namespace wss {
	class IReaderWriter {
	public:
		virtual ~IReaderWriter() { }
	protected:
		static const size_t DEFAULT_BLOCK_SIZE = 4000;
		friend class ReaderWriter;


		virtual size_t read(size_t pos, void * dst, size_t length) = 0;
		virtual size_t readUntilDelim(size_t pos, void * dst, size_t length, char delim, bool &wasDelimHit)=0;
		virtual size_t write(size_t pos, const void * src, size_t length) = 0;
		virtual size_t erase(size_t pos, size_t length) = 0;

		virtual size_t size() const = 0;
		virtual size_t capacity() const = 0;
		virtual bool fixedCapacity() const = 0;
		virtual size_t getPreferredBlockSize() const {
			return DEFAULT_BLOCK_SIZE;
		}

		//Get raw pointer to data
		virtual const void * raw(size_t idx, size_t& out_length) const = 0;
	};
}

#endif 

