#ifndef SEGMENTED_BUFFER_H
#define SEGMENTED_BUFFER_H

#include <vector>
#include <deque>
#include <algorithm>
#include "../portable_types.h"

namespace wss {


class BlockBuffer {
public:
	//ctor to create this interface around a block of memory
	BlockBuffer(uint8_t *buf, uint32_t size);
	//swaps internal pointers
	void swap(BlockBuffer &bb);
	//copy from this memory to an out going buffer
	uint32_t CopyTo(uint32_t pos, uint8_t* toBuffer, uint32_t sizeToCopy) const;
	//copy from external buffer to this memory
	uint32_t CopyFrom(uint32_t pos, const uint8_t* fromBuffer, uint32_t sizeToCopy);
	//check to see if block is empty
	bool isEmpty() const;
	//move memory within this block
	void MemMove(uint32_t moveTo, uint32_t moveFrom);
	//erease memory from position 'from' to end
	void erase(uint32_t from);
	//returns start pointer
	const uint8_t* getStart() const;
	//sets start pointer within buffer, this does not change the allocated start pointer
	void setStart(uint32_t pos);
	//set end = deepestWrite
	void syncCapacityWithBytesWritten();
	//returns the capacity of this buffer
	uint32_t capacity() const;
	//returns bytes written to this buffer
	uint32_t bytesWritten() const;
	//returns bytes free in this buffer (i.e. from deepestWrite to allocated end
	uint32_t bytesLeft() const;
	//allows you to 'fill' the buffer
	void fill();
	//frees block
	~BlockBuffer();
private:
	uint8_t *allocatedStart; //never moved used for delete []
	uint8_t *start; //moved if we erase from front of block
	uint8_t *deepestWrite; //moved as we write into block
	uint8_t *end; //moved if we erase from end or middle of block (moved towards start)
private:
	SET_NO_COPY(BlockBuffer);
};

template<unsigned int BLOCKSIZE>
class BlockBufferAllocator {
public:
	static const uint32_t BLOCK_SIZE = BLOCKSIZE;
	static BlockBuffer *allocate() {
		return new BlockBuffer(new uint8_t[BLOCKSIZE],BLOCKSIZE);
	}
	static void deallocate(BlockBuffer *buffer) {
		delete buffer;
	}
	BlockBufferAllocator() {}
private:
	SET_NO_COPY(BlockBufferAllocator);
};

/*
	@Author: Demetrius Comes
	@date 8-15-11

	SegmentedBuffer:  A deque of block buffers
	From the outside worlds perspective this should be treated as if its a contiguous array of memory, internally we keep a deque of blocks (size in bytes per block is determined by the BAllocator template parameter)

	Rules used for blocks:
		1.  BlockBuffers are allocated each block buffer has an contiguous arrary of memory of BAllocator::BLOCK_SIZE size
		2.  writing into the buffer is done via a position, the position is translated into a index / offset set to look up internally where to read, write, erase, or splice at.
		3.  From erase of less than 1 block:
				a.  but encompasses offset 0 we move the 'start' pointer to the new position in the block buffer
				b.  does not encompass offset 0, we memmove bytes then move the deepest write and 'end' pointer

		4.  No block has a 'whole' in it as an example: bytes 0 - 56 have data, 57-62 do not, byte 63 has data in a 64 byte block.  Instead we have data in 0-57, but now the capacity of the block is 57 bytes NOT 64 anymore.
		5.  Besides the last block - bytesWritten() for a BlockerBuffer must == Capacity for that blocker buffer (see #4)

	Size() = sum of bytesWritten for each BlockBuffer in this SegmentedBuffer
	Capacity() = sum of capacity for each BlockBuffer in this SegmentedBuffer
		**NOTE** they must be equal for all block buffers except the last buffer
*/

template<typename BAllocator>
class SegmentedBuffer {
	typedef std::deque<BlockBuffer*> BufferListType;
public:
	typedef uint32_t size_type;
	typedef uint32_t pos_type;
	static const uint32_t BLOCK_SIZE = BAllocator::BLOCK_SIZE;
private:
	class Position {
	public:
		static const size_type INVALID_INDEX = 0xFFFFFFFF;
		static Position convertFromLinearPosition(pos_type pos, const SegmentedBuffer *buf);
		pos_type convertToLinearOffset() const;
		bool operator>(const Position &p) const;
		size_type Index;
		pos_type OffSet;
		bool Is_Valid();
	private:
		Position(size_type index, size_type offSet, const SegmentedBuffer *buf);
		const SegmentedBuffer *SBuffer;
		friend class SegmentedBuffer;
	};
public:
	SegmentedBuffer();
	SegmentedBuffer(const SegmentedBuffer &sb);
	~SegmentedBuffer();

	const void *raw(pos_type pos, uint32_t &outSize) const ;

	size_type read(pos_type readPos, void *outBuff, size_type outBufferSize);

	size_type getBlockCount();
	size_type write(pos_type writePos, const void *inBuffer, size_type inBufferSize);
	size_type pushFront(const void *inBuffer, size_type inBufferSize);
	size_type resize(size_type size);

	size_type erase(pos_type endPos) ;

	size_type erase(pos_type startPos, size_type lengthToErase) ;
	size_type Capacity() const;
	size_type BytesWritten() const;
	size_type Size() const;

protected:
	Position getEnd();
	void EncodeBufferList();
	size_type EncodeSize() const;
	void UpdateEncodeEnd();
	size_type EncodeSize(bool forCapacity) const;
	Position findBlockIndex(uint32_t pos) const;
private:
	BufferListType BufferList;
private:
	struct Encoded {
		uint32_t startIndex;
		uint32_t endIndex;
		uint32_t BytesWritten;
		Encoded() : startIndex(0), endIndex(0), BytesWritten(0) {}
	};
	typedef std::vector<Encoded> EncodeListType;
	typedef typename EncodeListType::iterator EncodeListTypeIT;
	typedef typename EncodeListType::const_iterator EncodeListTypeCIT;
	std::vector<Encoded> EncodedCollection;
	friend class Position;
};

typedef SegmentedBuffer<BlockBufferAllocator<4000> > ReservedSizeSegmentedBuffer;
}

#endif
