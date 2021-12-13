#ifndef WSS_SEGMENTED_READER_WRITER_H
#define WSS_SEGMENTED_READER_WRITER_H

#pragma once


#include "segmented_buffer.h"
#include "../io/ireaderwriter.h"

namespace wss {
class SegmentedReaderWriterImpl : public IReaderWriter 
{
public:
	static IReaderWriter * Create();
protected:
	SegmentedReaderWriterImpl();
public:
	virtual ~SegmentedReaderWriterImpl();
	virtual size_t read(size_t pos, void * dst, size_t length);
	virtual size_t write(size_t pos, const void * src, size_t length);
	virtual size_t erase(size_t pos, size_t length);

	virtual size_t size() const;
	virtual size_t capacity() const;
	virtual bool fixedCapacity() const;
	virtual size_t getPreferredBlockSize() const;

	//Get raw pointer to data
	virtual const void * raw(size_t idx, size_t& out_length) const;
private:
	ReservedSizeSegmentedBuffer Stream;
};


}
#endif
