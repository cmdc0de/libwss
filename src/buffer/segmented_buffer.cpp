#include "segmented_buffer.h"
#include <cstring>
#include <cassert>

using namespace wss;

//#define VALIDATE_SEGMENTED_BUFFER

namespace wss {
/*
	@author Demetrius Comes

	Interface around a contiguous block of memory
	Segmented buffer class keeps bytesWritten()==capacity() as long as this is the last block buffer in the segmented buffer's list
		i.e. the current block buffer being written to
*/

}

BlockBuffer::BlockBuffer(uint8_t *buf, uint32_t size) : allocatedStart(buf), start(buf), deepestWrite(start), end(buf+size) {

}

void BlockBuffer::swap(BlockBuffer &bb) {
	//tmp
	uint8_t *ostart = start;
	uint8_t *oend = end;
	uint8_t *dw = deepestWrite;
	uint8_t *aStart = allocatedStart;

	//this
	start = bb.start;
	end = bb.end;
	deepestWrite = bb.deepestWrite;
	allocatedStart = bb.allocatedStart;

	//other
	bb.start = ostart;
	bb.end = oend;
	bb.deepestWrite = dw;
	bb.allocatedStart = aStart;
}

uint32_t BlockBuffer::CopyTo(uint32_t pos, uint8_t* toBuffer, uint32_t sizeToCopy) const  {
	uint32_t toCopy = (bytesWritten()-pos) >= 0 ? (bytesWritten()-pos) : 0;
	toCopy = (std::min)(toCopy,sizeToCopy);
	memcpy(toBuffer,&start[pos],toCopy);
	return toCopy;
}

uint32_t BlockBuffer::CopyToWithDelim(uint32_t pos, uint8_t* toBuffer, uint32_t sizeToCopy, char delim, bool &delimHit) const  {
  delimHit = false;
	uint32_t MaxToCopy = (bytesWritten()-pos) >= 0 ? (bytesWritten()-pos) : 0;
	MaxToCopy = (std::min)(MaxToCopy,sizeToCopy);
  uint32_t copied = 0;
  for(;copied<MaxToCopy;++copied) {
    if(static_cast<char>(start[pos])==delim) {
      toBuffer[copied] = 0;
      delimHit = true;
      return copied;
    } else {
      toBuffer[copied] = start[pos++];
    }
  }
	return copied;
}

uint32_t BlockBuffer::CopyFrom(uint32_t pos, const uint8_t* fromBuffer, uint32_t sizeToCopy) {
	uint32_t toCopy = (capacity()-pos) >= 0 ? (capacity()-pos) : 0;
	toCopy = (std::min)(toCopy,sizeToCopy);
	if(fromBuffer) {
		memcpy(&start[pos],fromBuffer,toCopy);
	}
	//move deepest write
	deepestWrite = (deepestWrite+toCopy)>deepestWrite ? (deepestWrite+toCopy) : deepestWrite;
	return toCopy;
}

/**********************************************************************************************
* BlockBuffer::isEmpty -- checks to see if buffer is empty
*
* History: 6/16/2011 6:14:33 PM - Demetrius_Comes
*********************************************************************************************/
bool BlockBuffer::isEmpty() const {
	return deepestWrite==start;
}

/**********************************************************************************************
* BlockBuffer::MemMove -- Moves memory from offset 'moveFrom' to offset 'moveTo'
*
* History: 6/16/2011 6:15:02 PM - Demetrius_Comes
*********************************************************************************************/
void BlockBuffer::MemMove(uint32_t moveTo, uint32_t moveFrom) {
	memmove(&start[moveTo],&start[moveFrom],deepestWrite-&start[moveFrom]);
	deepestWrite = deepestWrite-(moveFrom-moveTo);
}

/**********************************************************************************************
* BlockBuffer::erase -- erases everything after 'from'
*
* History: 6/16/2011 6:15:31 PM - Demetrius_Comes
*********************************************************************************************/
void BlockBuffer::erase(uint32_t from) {
	deepestWrite = &start[from];
}

/**********************************************************************************************
* BlockBuffer::getStart -- returns current start pointer in this block
*
* History: 6/16/2011 6:15:49 PM - Demetrius_Comes
*********************************************************************************************/
const uint8_t* BlockBuffer::getStart() const
{
	return start;
}

/**********************************************************************************************
* BlockBuffer::fill -- fills buffer
*
* History: 6/16/2011 6:16:18 PM - Demetrius_Comes
*********************************************************************************************/
void BlockBuffer::fill() {
	deepestWrite = end;
}

/**********************************************************************************************
* BlockBuffer::setStart -- sets start pointer
*
* History: 6/16/2011 6:16:29 PM - Demetrius_Comes
*********************************************************************************************/
void BlockBuffer::setStart(uint32_t pos) {
	start = start+pos;
	assert(end>=start);
}

/**********************************************************************************************
* BlockBuffer::capacity -- returns capacity of this buffer
*
* History: 6/16/2011 6:16:56 PM - Demetrius_Comes
*********************************************************************************************/
uint32_t BlockBuffer::capacity() const {
	return static_cast<uint32_t>(end-start);
}

/**********************************************************************************************
* BlockBuffer::bytesWritten -- returns byte written to buffer
*
* History: 6/16/2011 6:17:06 PM - Demetrius_Comes
*********************************************************************************************/
uint32_t BlockBuffer::bytesWritten() const {
	return static_cast<uint32_t>(deepestWrite-start);
}

/**********************************************************************************************
* BlockBuffer::bytesLeft -- returns bytes left in the buffer
*
* History: 6/16/2011 6:17:18 PM - Demetrius_Comes
*********************************************************************************************/
uint32_t BlockBuffer::bytesLeft() const {
	return static_cast<uint32_t>(end-deepestWrite);
}

/**********************************************************************************************
* BlockBuffer::syncCapacityWithBytesWritten -- ensure end and deepest write are equal
*
* History: 8/18/2011 1:32:16 PM - Demetrius_Comes
*********************************************************************************************/
void BlockBuffer::syncCapacityWithBytesWritten() {
	end = deepestWrite;
}

BlockBuffer::~BlockBuffer() {
	delete [] allocatedStart;
}

template<typename BAllocator>
bool SegmentedBuffer<BAllocator>::Position::Is_Valid() {
	return Index!=INVALID_INDEX && Index>=0 && Index<SBuffer->BufferList.size();
}

/*
	Convert linear position to index / offset
	Use capacity to determine which block / offset we need to return
*/
template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::Position SegmentedBuffer<BAllocator>::Position::convertFromLinearPosition(pos_type pos, const SegmentedBuffer *buf) {
	//early out test
	if(pos==0) {
		return Position(0,0,buf);
	} else {
#ifdef VALIDATE_SEGMENTED_BUFFER
		BufferListType::const_iterator it = buf->BufferList.begin();
		uint32_t totalCapacity = 0;
		uint32_t lastCapacity = 0;
		size_type index = 0;
		for(;it!=buf->BufferList.end();++it,++index) {
			totalCapacity+= (lastCapacity=(*it)->capacity());
			if(pos<=totalCapacity) {
				return Position(index,(pos - (totalCapacity-lastCapacity)), buf);
			}
		}
		return Position(INVALID_INDEX,0,0);
#else
		return buf->findBlockIndex(pos);
#endif
	}
}

/*
	we want end to return the end of the written bytes not the end of the allocated bytes
*/
template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::Position SegmentedBuffer<BAllocator>::getEnd() {
	assert(BufferList.size());
	return Position(uint32_t(BufferList.size()-1),BufferList[BufferList.size()-1]->bytesWritten(),this);
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::pos_type SegmentedBuffer<BAllocator>::Position::convertToLinearOffset() const {
	uint32_t pos = 0;
	for(uint32_t i=0;i<Index;i++) {
		pos += SBuffer->BufferList[i]->capacity();
	}
	return (pos+OffSet);
}

template<typename BAllocator>
bool SegmentedBuffer<BAllocator>::Position::operator>(const typename SegmentedBuffer<BAllocator>::Position &p) const {
	return (Index>p.Index || (Index==p.Index && OffSet>p.OffSet));
}

template<typename BAllocator>
SegmentedBuffer<BAllocator>::Position::Position(size_type index, size_type offSet, const SegmentedBuffer<BAllocator> *buf) : Index(index), OffSet(offSet), SBuffer(buf) {}

template<typename BAllocator>
SegmentedBuffer<BAllocator>::SegmentedBuffer() {
	BufferList.push_back(BAllocator::allocate());
}

template<typename BAllocator>
SegmentedBuffer<BAllocator>::~SegmentedBuffer() {
	std::for_each(BufferList.begin(),BufferList.end(),&BAllocator::deallocate);
}


template<typename BAllocator>
SegmentedBuffer<BAllocator>::SegmentedBuffer(const SegmentedBuffer<BAllocator> &sb) : BufferList(sb.BufferList)
{
}

/*
	given a position conver the linear position to an index/offset
	set outSize to the number of bytes remaining in this block buffer from the index/offset calculated above

	***NOTE***
		bytesWritten is used (over capacity) here because if we are in the last block buffer we want to return the number of
		*valid* bytes left not the number of bytes total.
		This is written this way to fit with current PGLiveStreamInterface implementations, not sure its actually right.
		Perhaps it would be better to put that detail in the hands of the caller, i.e. they have to know the size rather than
		relying on the implementation to give them the number of bytes written not the number of bytes available that can be read
		without leaving the allocated memory
*/
template<typename BAllocator>
const void *SegmentedBuffer<BAllocator>::raw(pos_type pos, uint32_t &outSize) const {
	Position s = Position::convertFromLinearPosition(pos,this);
	//position is valid AND the returned offset is less or equal to than the number of bytes written
	//Convert FromLinear OFfset uses capacity NOT bytes written so it could return an offset that is larger
	//than the number of bytes written to the last block
	if(s.Is_Valid() && (s.OffSet<=BufferList[s.Index]->bytesWritten())) {
		outSize = BufferList[s.Index]->bytesWritten()-s.OffSet; //add 1 b/c we return the number of bytes not an index
		return BufferList[s.Index]->getStart()+s.OffSet;
	} else {
		outSize = 0;
		return 0;
	}
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::read(pos_type readPos, void *outBuff, size_type outBufferSize) {
	Position ReadPos = Position::convertFromLinearPosition(readPos,this);
	if(ReadPos.Is_Valid()) {
		size_type copiedTotal = 0;
		size_type leftToCopy = outBufferSize;
		size_type currentIndex = ReadPos.Index;
		size_type currentOffSet = ReadPos.OffSet;
		while(leftToCopy>0 && currentIndex<BufferList.size()) {
			size_type copied = BufferList[currentIndex++]->CopyTo(currentOffSet,static_cast<uint8_t*>(outBuff)+copiedTotal,leftToCopy);
			copiedTotal += copied;
			leftToCopy -= copied;
			currentOffSet = 0;
		}
		return outBufferSize;
	} else {
		return 0;
	}
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::read(pos_type readPos, void *outBuff
    , size_type outBufferSize, char delim, bool &bDelimHit) {
	Position ReadPos = Position::convertFromLinearPosition(readPos,this);
	if(ReadPos.Is_Valid()) {
		size_type copiedTotal = 0;
		size_type leftToCopy = outBufferSize;
		size_type currentIndex = ReadPos.Index;
		size_type currentOffSet = ReadPos.OffSet;
		while(leftToCopy>0 && currentIndex<BufferList.size()) {
			size_type copied = BufferList[currentIndex++]->CopyToWithDelim(currentOffSet,static_cast<uint8_t*>(outBuff)+copiedTotal,leftToCopy,delim, bDelimHit);
			copiedTotal += copied;
			leftToCopy -= copied;
			currentOffSet = 0;
      if(bDelimHit) {
        break;
      }
		}
		return outBufferSize;
	} else {
		return 0;
	}
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::getBlockCount() {
	return (size_type)BufferList.size();
}

template<typename BAllocator>
void SegmentedBuffer<BAllocator>::EncodeBufferList() {
	BufferListType::iterator it = BufferList.begin();
	uint32_t lastSize=0xFFFFFFFF;
	uint32_t count=0;
	EncodedCollection.resize(0);
	for(;it!=BufferList.end();++it) {
		if((*it)->bytesWritten()==lastSize) {
			EncodedCollection.back().endIndex = count++;
		} else {
			Encoded e;
			e.endIndex = e.startIndex = count++;
			//favor bytes written over capacity per block even though they can only be different for the last block
			e.BytesWritten = (*it)->bytesWritten();
			EncodedCollection.push_back(e);
			lastSize = e.BytesWritten;
		}
	}
}

template<typename BAllocator>
void SegmentedBuffer<BAllocator>::UpdateEncodeEnd() {
	uint32_t lastIndex=EncodedCollection.empty() ? 0 : EncodedCollection.back().endIndex;
	if(EncodedCollection.empty()) {
		Encoded e;
		e.endIndex = e.startIndex = lastIndex;
		//favor bytes written over capacity per block even though they can only be different for the last block
		e.BytesWritten = BufferList[lastIndex]->bytesWritten();
		EncodedCollection.push_back(e);
		lastIndex++;
	} else if(EncodedCollection.back().endIndex-EncodedCollection.back().startIndex>0) {
		if(EncodedCollection.back().BytesWritten!=BufferList[lastIndex]->bytesWritten()) {
			EncodedCollection.back().endIndex--;
			Encoded e;
			e.endIndex = e.startIndex = lastIndex;
			//favor bytes written over capacity per block even though they can only be different for the last block
			e.BytesWritten = BufferList[lastIndex]->bytesWritten();
			EncodedCollection.push_back(e);
			lastIndex++;
		}
	} else if(EncodedCollection.back().BytesWritten!=BufferList[lastIndex]->bytesWritten()) {
		EncodedCollection.back().BytesWritten = BufferList[lastIndex]->bytesWritten();
	}
	uint32_t lastSize=EncodedCollection.back().BytesWritten;
	uint32_t bsize = (uint32_t)BufferList.size();
	for(uint32_t i=EncodedCollection.back().endIndex+1;i<bsize;i++) {
		if(BufferList[i]->bytesWritten()==lastSize) {
			EncodedCollection.back().endIndex = lastIndex++;
		} else {
			Encoded e;
			e.endIndex = e.startIndex = lastIndex++;
			//favor bytes written over capacity per block even though they can only be different for the last block
			e.BytesWritten = BufferList[i]->bytesWritten();
			EncodedCollection.push_back(e);
			lastSize = e.BytesWritten;
		}
	}
}


template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::EncodeSize() const {
	return EncodeSize(false);
}

//
// if forCapacity = true, then add the difference between bytes written and capacity of the last block.
//
template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::EncodeSize(bool forCapacity) const {
	uint32_t size = 0;
	EncodeListTypeCIT it = EncodedCollection.begin();
	for(;it!=EncodedCollection.end();++it) {
		size+=(*it).BytesWritten*(((*it).endIndex-(*it).startIndex)+1);
	}
	if(forCapacity) {
		size+=BufferList[BufferList.size()-1]->capacity()-BufferList[BufferList.size()-1]->bytesWritten();
	}
	return size;
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::Position SegmentedBuffer<BAllocator>::findBlockIndex(uint32_t pos) const {
	EncodeListTypeCIT it = EncodedCollection.begin();
	uint32_t lastLoopLinearPos = 0;
	uint32_t index = 0;
	for(;it!=EncodedCollection.end();++it) {
		uint32_t endLinear = (*it).BytesWritten*(((*it).endIndex-(*it).startIndex)+1) + lastLoopLinearPos;
		if(pos>=lastLoopLinearPos && pos<=endLinear) {
			uint32_t BucketSize = pos-lastLoopLinearPos;
			index+= BucketSize/(*it).BytesWritten;
			return Position(index,BucketSize%(*it).BytesWritten,this);
		}
		lastLoopLinearPos = endLinear;
		index+=((*it).endIndex-(*it).startIndex)+1;
	}
	return Position(Position::INVALID_INDEX,0,0);
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::write(pos_type writePos, const void *inBuffer, size_type inBufferSize) {
	Position WritePos  = Position::convertFromLinearPosition(writePos,this);
	if(!WritePos.Is_Valid()) {
		WritePos = getEnd();
	}
	size_type copiedTotal = 0;
	size_type leftToCopy = inBufferSize;
	size_type currentIndex = WritePos.Index;
	size_type currentOffSet = WritePos.OffSet;
	while(leftToCopy>0) {
		size_type copied = 0;
		//do we need to allocate a new block
		if(currentIndex>=BufferList.size()) {
			BlockBuffer *bb = BAllocator::allocate();
//			WSSLOG_ASSERTMSG(0,bb,"failed to allocate buffer");
			BufferList.push_back(bb);
		}
		if(inBuffer) {
			copied = BufferList[currentIndex++]->CopyFrom(currentOffSet,static_cast<const uint8_t*>(inBuffer)+copiedTotal,leftToCopy);
		} else { //passing null simply moves points to act like a "fill"
			copied = BufferList[currentIndex++]->CopyFrom(currentOffSet,static_cast<const uint8_t*>(inBuffer),leftToCopy);
		}
		copiedTotal += copied;
		leftToCopy -= copied;
		currentOffSet = 0;
	}
#ifdef VALIDATE_SEGMENTED_BUFFER
	for(uint32_t i=0;i<BufferList.size();i++)
		if(i!=(BufferList.size()-1)) {
			assert(BufferList[i]->bytesWritten()==BufferList[i]->capacity());
		}
#endif
	EncodeBufferList();
	return inBufferSize;
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::pushFront(const void *inBuffer, size_type inBufferSize) {
	SegmentedBuffer sb;
	sb.write(0,inBuffer,inBufferSize);
	//set capacity and bytesWritten equal before we tail on current blocks
	sb.BufferList[sb.BufferList.size()-1]->syncCapacityWithBytesWritten();
	BufferListType::iterator it = BufferList.begin();
	for(;it!=BufferList.end();++it) {
		sb.BufferList.push_back((*it));
	}
	BufferList.swap(sb.BufferList);
	//TODO OPT
	//could just update encoded list directly as we know we only pushed front
	EncodeBufferList();
	return this->BytesWritten();
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::erase(pos_type endPos) {
	return erase(0,endPos);
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::erase(pos_type startPos, size_type lengthToErase)
{
	Position sPos = Position::convertFromLinearPosition(startPos,this);
	Position ePos = Position::convertFromLinearPosition(startPos+lengthToErase,this);
	if(!ePos.Is_Valid()) {
		ePos = getEnd();
	}
#ifdef VALIDATE_SEGMENTED_BUFFER
	//only valid in test cases
	uint32_t validationLength = ePos.convertToLinearOffset()-sPos.convertToLinearOffset();
#endif
	//total blocks to reserve
	uint32_t numBlocks = uint32_t(BufferList.size()) - (ePos.Index-sPos.Index);
	//reserve blocks
	BufferListType newContainer(numBlocks,0), removedBlocks;
	uint32_t index = 0;
	uint32_t wIndex = 0;
	size_type erased = 0;
	for(;index<BufferList.size();index++) {
		if(wIndex>=newContainer.size()) {
			newContainer.push_back(0);
		}
		//entire block needs to be retained
		if(index<sPos.Index) {
			newContainer[wIndex++] = BufferList[index];
		}
		//we are removing from this block
		else if(index==sPos.Index && index==ePos.Index) {
			//we can erase the entire block
			if(BufferList[index]->bytesWritten()<=lengthToErase) {
				removedBlocks.push_back(BufferList[index]);
				erased+=BufferList[index]->bytesWritten();
			} else { //erase just part of this block
				newContainer[wIndex] = BufferList[index];
				newContainer[wIndex]->MemMove(sPos.OffSet,ePos.OffSet);
				//if this is not the last buffer sync Capacity with bytes written
				//otherwise just change deepest write
				if(index!=(BufferList.size()-1)) {
					newContainer[wIndex]->syncCapacityWithBytesWritten();
				}
				erased+=(ePos.OffSet-sPos.OffSet);
				wIndex++;
			}
		}
		//portion of the current block will be erased
		else if(index==sPos.Index && index<ePos.Index) {
			if(sPos.OffSet==0) { //nothing needed from this block
				removedBlocks.push_back(BufferList[index]);
				erased+=BufferList[index]->bytesWritten();
			} else {	//partial info needed
				newContainer[wIndex] = BufferList[index];
				uint32_t oldBytesWritten = BufferList[index]->bytesWritten();
				newContainer[wIndex]->erase(sPos.OffSet);
				//its not the last block so sync capacity with bytes written
				//we knows its not the last block because index<ePos.Index
				newContainer[wIndex]->syncCapacityWithBytesWritten();
				erased+=oldBytesWritten-BufferList[index]->bytesWritten();
				wIndex++;
			}
		} //entire block can be erased
		else if(index>sPos.Index && index<ePos.Index) {
			removedBlocks.push_back(BufferList[index]);
			erased+=BufferList[index]->bytesWritten();
		} //erase portion of the ending block
		else if(index>sPos.Index && index==ePos.Index) {
			BufferList[index]->setStart(ePos.OffSet);
			if(BufferList[index]->bytesWritten()==0) {
				removedBlocks.push_back(BufferList[index]);
			} else {
				newContainer[wIndex] = BufferList[index];
				wIndex++;
			}
			erased+=ePos.OffSet;
		} else { //index>ePos.Index entire block stays
			newContainer[wIndex++] = BufferList[index];
		}
	}
	//clear the nulls
	while(!newContainer.empty() && !newContainer.back()) {
		newContainer.pop_back();
	}
#ifdef VALIDATE_SEGMENTED_BUFFER
	//only valid in test cases
	assert(erased==validationLength);
#endif
	//swap Container pointers
	BufferList.swap(newContainer);
	//free erased blocks
	std::for_each(removedBlocks.begin(),removedBlocks.end(),&BAllocator::deallocate);

#ifdef VALIDATE_SEGMENTED_BUFFER
	for(uint32_t i=0;i<BufferList.size();i++)
	if(i!=(BufferList.size()-1)) {
		assert(BufferList[i]->bytesWritten()==BufferList[i]->capacity());
	}
#endif
	EncodeBufferList();
	return erased;
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::Capacity() const {
#ifdef VALIDATE_SEGMENTED_BUFFER
	struct Pred {
		size_type val;
		Pred() : val(0) {}
		void operator()(BlockBuffer *bb) {
			val+=bb->capacity();
		}
	};
	Pred pred;
	Pred retVal = std::for_each(BufferList.begin(),BufferList.end(),pred);
	assert(retVal.val==EncodeSize());
	return retVal.val;
#else
	return EncodeSize(true);
#endif
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::BytesWritten() const {
#ifdef VALIDATE_SEGMENTED_BUFFER
	struct Pred {
		size_type val;
		Pred() : val(0) {}
		void operator()(BlockBuffer *bb) {
			val+=bb->bytesWritten();
		}
	};
	Pred pred;
	Pred retVal = std::for_each(BufferList.begin(),BufferList.end(),pred);
	assert(retVal.val==EncodeSize());
	return retVal.val;
#else
	return EncodeSize();
#endif
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::Size() const {
	return BytesWritten();
}

template<typename BAllocator>
typename SegmentedBuffer<BAllocator>::size_type SegmentedBuffer<BAllocator>::resize(size_type size) {
	write(Size(),0,size-Size());
	return Size();
}

template class SegmentedBuffer<BlockBufferAllocator<4000> >;
template class SegmentedBuffer<BlockBufferAllocator<64> >;
template class SegmentedBuffer<BlockBufferAllocator<4060> >;
template class SegmentedBuffer<BlockBufferAllocator<1024> >;
