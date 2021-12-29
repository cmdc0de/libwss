#include "readerwriter.h"
#include <algorithm>

#include "../buffer/segmented_reader_writer_impl.h"

using namespace wss;


IReaderWriter * ReaderWriter::Create_Default_Interface()
{
	return SegmentedReaderWriterImpl::Create();
}

ReaderWriter::ReaderWriter( IReaderWriter* stream_interface ) :
ReadCursor(0)
,WriteCursor(0) 
,Impl(stream_interface) 
{
	WriteCursor = stream_interface->size();
}

ReaderWriter::ReaderWriter( const ReaderWriter& other ) :
ReadCursor(0)
,WriteCursor(0) 
,Impl(other.Impl) 
{
	WriteCursor = Impl->size();
}

ReaderWriter::~ReaderWriter() {

}

/**********************************************************************************************
* ReaderWriter::Read -- Read from the stream
* 
* In: dst - Buffer data will be written to.
*		length - Amount of data (in bytes) to read.
*
* Returns: Number of bytes read, -1 on error.
*
*********************************************************************************************/
size_t ReaderWriter::read( void * dst, size_t length ) {
	//Adjust length here.  Never pass length to Impl that would read more than is available
	if(length > 0) {
		size_t real_len = length;
		if(real_len + ReadCursor > Impl->size()) {
			real_len = Impl->size() - ReadCursor;
		}

		size_t read_len = Impl->read(ReadCursor, dst, real_len);
		ReadCursor += read_len;
		return read_len;
	}
	return STREAM_ERROR; //error
}

size_t ReaderWriter::readUntilDelim(void * dst, size_t length, char delim, bool &wasDelimHit) {
	//Adjust length here.  Never pass length to Impl that would read more than is available
	if(length > 0) {
		size_t real_len = length;
		if(real_len + ReadCursor > Impl->size()) {
			real_len = Impl->size() - ReadCursor;
		}

		size_t read_len = Impl->readUntilDelim(ReadCursor, dst, real_len, delim, wasDelimHit);
		ReadCursor += read_len;
		return read_len;
	}
	return STREAM_ERROR; //error
}
/**********************************************************************************************
* ReaderWriter::Write -- Write data to the stream
*
* In: src - Data to be written to stream
*		length - Number of bytes to write to stream
*
* Returns: The number of bytes actually written or -1 on error.
* 
*********************************************************************************************/
size_t ReaderWriter::write( const void * src, size_t length ) {

	if(length > 0)
	{
		//If fixed size then adjust length of data if needed.
		size_t real_length = length;
		if(real_length + WriteCursor > Impl->capacity())
		{
			if(Impl->fixedCapacity())
			{
				//Can't resize the buffer
				real_length = (Impl->capacity() - WriteCursor);
			}
		}

		size_t written = Impl->write(WriteCursor, src, real_length);
		WriteCursor += written;
		return written;
	}
	return STREAM_ERROR; //error
}


/**********************************************************************************************
* ReaderWriter::Erase -- Erase data from the stream
* 
* In: idx - Index to begin erasing at
*		length - Number of bytes to erase
*
* Returns: Number of bytes actually erased.
*
*********************************************************************************************/
size_t ReaderWriter::erase( size_t idx, size_t length ) {

	//get actual amount erased
	size_t erased_length = Impl->erase(idx, length);

	//Move read and write cursors
	//If
	//  Cursor is before erased data - Do nothing.
	//  Cursor is inside erased data - Place cursor on first un-erased byte
	//  Cursor is after erased data - Subtract amount erased so that cursor points to the same data

	//Cursor is inside erased data
	if(ReadCursor >= idx && ReadCursor < idx+erased_length)
	{
		ReadCursor = idx;
	}
	//cursor is past erased data
	else if(ReadCursor >= idx+erased_length)
	{
		ReadCursor -= erased_length;
	}

	//Cursor is inside erased data
	if(WriteCursor >= idx && WriteCursor < idx+erased_length)
	{
		WriteCursor = idx;
	}
	//cursor is past erased data
	else if(WriteCursor >= idx+erased_length)
	{
		WriteCursor -= erased_length;
	}

	return erased_length;
}

/**********************************************************************************************
* ReaderWriter::TellRead -- Returns the current read posision of the stream
* 
*********************************************************************************************/
size_t ReaderWriter::tellRead() const {
	return ReadCursor;
}


/**********************************************************************************************
* ReaderWriter::SeekRead -- Move the read cursor to the specified location
* 
* In: distance - Distance from the specified start point.  Can be negative.
*		seek_type - Starting position to seek from (BEGIN, CUR, END).
*
* Returns: new location of read cursor. -1 on error.
*
*********************************************************************************************/
size_t ReaderWriter::seekRead( pos_type distance, StreamSeekType seek_type ) {
	size_t new_cursor = 0;
	switch(seek_type)
	{
	case BEGIN:
		new_cursor = distance;
		break;
	case CUR:
		new_cursor = ReadCursor + distance;
		break;
	case END:
		new_cursor = Impl->size() + distance;
		break;
	}

	if(new_cursor >= 0 && new_cursor <= Impl->size())
	{
		ReadCursor = new_cursor;
		return ReadCursor;
	}
	return STREAM_ERROR; //error
}

/**********************************************************************************************
* ReaderWriter::UnreadSize -- Number of bytes left to read in the stream
* 
*********************************************************************************************/
size_t ReaderWriter::unreadSize() const {
	return size() - tellRead();
}

/**********************************************************************************************
* ReaderWriter::TellWrite -- Returns the current position of the write cursor.
* 
*********************************************************************************************/
size_t ReaderWriter::tellWrite() const {
	return WriteCursor;
}

/**********************************************************************************************
* ReaderWriter::SeekWrite -- Move the write cursor to the specified location
* 
* In: distance - Distance from the specified start point.  Can be negative.
*		seek_type - Starting position to seek from (BEGIN, CUR, END).
*
* Returns: new location of write cursor. -1 on error.
*
*********************************************************************************************/
size_t ReaderWriter::seekWrite( pos_type distance, StreamSeekType seek_type ) {
	size_t new_cursor = 0;
	switch(seek_type)
	{
	case BEGIN:
		new_cursor = distance;
		break;
	case CUR:
		new_cursor = ReadCursor + distance;
		break;
	case END:
		new_cursor = Impl->size() + distance;
		break;
	}

	if(new_cursor >= 0 && new_cursor <= Impl->size())
	{
		WriteCursor = new_cursor;
		return WriteCursor;
	}
	return STREAM_ERROR; //error
}

/**********************************************************************************************
* ReaderWriter::Empty -- Returns true if the buffer has no data.
* 
*********************************************************************************************/
bool ReaderWriter::empty() const {
	return size() == 0;
}


/**********************************************************************************************
* ReaderWriter::Size -- Returns the current size of the stream.
* 
*********************************************************************************************/
size_t ReaderWriter::size() const {
	return Impl->size();
}


/**********************************************************************************************
* ReaderWriter::Raw -- Fetch a raw pointer to the data at a given index  
*
* In: idx - Index of the requested data
*
* Out: out_length - Amount of data that can be read from the returned pointer.  This may not
*						  be the whole buffer in the case of a segmented buffer.  Multiple may be 
*						  needed to read all of the data.
* 
* Returns: pointer to data that at least out_length bytes can be read from
*
*********************************************************************************************/
const void * ReaderWriter::raw( size_t idx, size_t& out_length ) const {
	return Impl->raw(idx, out_length);
}


/**********************************************************************************************
* ReaderWriter::Get_Preferred_Block_Size -- returns preferred blocksize for this buffer
* 
*********************************************************************************************/
size_t ReaderWriter::getPreferredBlockSize() const {
	return Impl->getPreferredBlockSize();
}

/**********************************************************************************************
*
* ReaderWriter::readVarUInt32 -- Read an Unsigned VarInt into a 32 bit value
*
*********************************************************************************************/
size_t ReaderWriter::readVarUInt32(uint32_t& val) {
	uint32_t result = 0;
	int count = 0;
	uint8_t b;

	do {
		if (static_cast<uint32_t>(count) >= MaxVarInt32) {      
			return STREAM_ERROR;
		}

		if(readFixed8(b) == STREAM_ERROR) {
			return STREAM_ERROR;
		}

		result |= static_cast<uint32_t>(b & 0x7F) << (7 * count);
		count++;
	} while(b & 0x80);

	val = result;
	return count;
}

/**********************************************************************************************
*
* ReaderWriter::ReadVarUInt32 -- Read an Signed VarInt into a 32 bit value
*
*********************************************************************************************/
size_t ReaderWriter::readVarSInt32(int32_t& val) {
	uint32_t number;
	size_t ret = readVarUInt32(number);
	if (ret  != STREAM_ERROR) {
		val = zigZagDecode32(number);
	}
	
	return ret;
}

/**********************************************************************************************
*
* ReaderWriter::ReadVarUInt64 -- Read an Unsigned VarInt into a 64 bit value
*
*********************************************************************************************/
size_t ReaderWriter::readVarUInt64(uint64_t& val) {
	uint64_t result = 0;
	int count = 0;
	uint8_t b;

	do {
		if (static_cast<uint32_t>(count) >= MaxVarInt64) {
			return STREAM_ERROR;
		}

		if(readFixed8(b) == STREAM_ERROR) {
			return STREAM_ERROR;
		}

		result |= static_cast<uint64_t>(b & 0x7F) << (7 * count);
		count++;
	} while(b & 0x80);

	val = result;
	return count;
}

/**********************************************************************************************
*
* ReaderWriter::ReadVarSInt64 -- Read a Signed VarInt into a 64 bit value
*
*********************************************************************************************/
size_t ReaderWriter::readVarSInt64(int64_t& val) {
	uint64_t number;
	size_t ret = readVarUInt64(number);
	if (ret  != STREAM_ERROR) {
		val = zigZagDecode64(number);
	}

	return ret;
}

/**********************************************************************************************
*
* ReaderWriter::WriteVarInt32Helper -- Helper for writing 32-bit VarInts
* from the Google Protocol Buffer code base
*
*********************************************************************************************/
size_t ReaderWriter::WriteVarInt32Helper(uint32_t value, uint8_t* target) {
	target[0] = static_cast<uint8_t>(value | 0x80);
	if (value >= (1 << 7)) {
		target[1] = static_cast<uint8_t>((value >>  7) | 0x80);
		if (value >= (1 << 14)) {
			target[2] = static_cast<uint8_t>((value >> 14) | 0x80);
			if (value >= (1 << 21)) {
				target[3] = static_cast<uint8_t>((value >> 21) | 0x80);
				if (value >= (1 << 28)) {
					target[4] = static_cast<uint8_t>(value >> 28);
					return 5;
				} else {
					target[3] &= 0x7F;
					return 4;
				}
			} else {
				target[2] &= 0x7F;
				return 3;
			}
		} else {
			target[1] &= 0x7F;
			return 2;
		}
	} else {
		target[0] &= 0x7F;
		return 1;
	}
}

/**********************************************************************************************
*
* ReaderWriter::WriteVarInt64Helper -- Helper for writing 64-bit VarInts
* from the Google Protocol Buffer code base
*
*********************************************************************************************/
size_t ReaderWriter::WriteVarInt64Helper(uint64_t value, uint8_t* target) {
	// Splitting into 32-bit pieces gives better performance on 32-bit
	// processors.
	uint32_t part0 = static_cast<uint32_t>(value      );
	uint32_t part1 = static_cast<uint32_t>(value >> 28);
	uint32_t part2 = static_cast<uint32_t>(value >> 56);

	int size;

	// Here we can't really optimize for small numbers, since the value is
	// split into three parts.  Cheking for numbers < 128, for instance,
	// would require three comparisons, since you'd have to make sure part1
	// and part2 are zero.  However, if the caller is using 64-bit integers,
	// it is likely that they expect the numbers to often be very large, so
	// we probably don't want to optimize for small numbers anyway.  Thus,
	// we end up with a hardcoded binary search tree...
	if (part2 == 0) {
		if (part1 == 0) {
			if (part0 < (1 << 14)) {
				if (part0 < (1 << 7)) {
					size = 1;
					target[0] = static_cast<uint8_t>((part0      ) | 0x80);
				} else {
					size = 2;
					target[1] = static_cast<uint8_t>((part0 >>  7) | 0x80);
				}
			} else {
				if (part0 < (1 << 21)) {
					size = 3;
					target[2] = static_cast<uint8_t>((part0 >> 14) | 0x80);
				} else {
					size = 4;
					target[3] = static_cast<uint8_t>((part0 >> 21) | 0x80);
				}
			}
		} else {
			if (part1 < (1 << 14)) {
				if (part1 < (1 << 7)) {
					size = 5;
					target[4] = static_cast<uint8_t>((part1      ) | 0x80);
				} else {
					size = 6;
					target[5] = static_cast<uint8_t>((part1 >>  7) | 0x80);
				}
			} else {
				if (part1 < (1 << 21)) {
					size = 7;
					target[6] = static_cast<uint8_t>((part1 >> 14) | 0x80);
				} else {
					size = 8;
					target[7] = static_cast<uint8_t>((part1 >> 21) | 0x80);
				}
			}
		}
	} else {
		if (part2 < (1 << 7)) {
			size = 9;
			target[8] = static_cast<uint8_t>((part2      ) | 0x80);
		} else {
			size = 10;
			target[9] = static_cast<uint8_t>((part2 >>  7) | 0x80);
		}
	}
	target[size-1] &= 0x7F;
	return size;
}

