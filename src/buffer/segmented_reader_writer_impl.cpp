#include "segmented_reader_writer_impl.h"

using namespace wss;


/**********************************************************************************************
* SegmentedReaderWriterImpl::Create -- static create function for segmented buffer pg live interface
* 
* History: 6/3/2011 3:33:47 PM - Demetrius_Comes
*********************************************************************************************/
IReaderWriter * SegmentedReaderWriterImpl::Create( ) {
	return new SegmentedReaderWriterImpl();
}

SegmentedReaderWriterImpl::SegmentedReaderWriterImpl() : Stream() {

}


SegmentedReaderWriterImpl::~SegmentedReaderWriterImpl() {

}

/**********************************************************************************************
* SegmentedReaderWriterImpl::Read -- reads length bytes into dst from pos in current segmented buffer
* 
*********************************************************************************************/
size_t SegmentedReaderWriterImpl::read(size_t pos, void * dst, size_t length) {
	return Stream.read(pos,dst,length);
}


size_t SegmentedReaderWriterImpl::readUntilDelim(size_t pos, void * dst, size_t length, char delim, bool &wasDelimHit) {
	return Stream.read(pos,dst,length,delim,wasDelimHit);
}
/**********************************************************************************************
* SegmentedReaderWriterImpl::Write -- writes length bytes from src starting at pos in segmented buffer
* 
*********************************************************************************************/
size_t SegmentedReaderWriterImpl::write(size_t pos, const void * src, size_t length) {
	return Stream.write(pos,src,length);
}


/**********************************************************************************************
* SegmentedReaderWriterImpl::Erase -- erases length bytes from segmented buffer starting at position 'pos'
* 
*********************************************************************************************/
size_t SegmentedReaderWriterImpl::erase(size_t pos, size_t length) {
	return Stream.erase(pos,length);
}


/**********************************************************************************************
* SegmentedReaderWriterImpl::Size -- retursn size of segmented buffer (bytes written)
* 
*********************************************************************************************/
size_t SegmentedReaderWriterImpl::size() const {
	return Stream.Size();
}


/**********************************************************************************************
* SegmentedReaderWriterImpl::Capacity -- returns the number of bytes that have + can yet be written to this buffer
* 
*********************************************************************************************/
size_t SegmentedReaderWriterImpl::capacity() const {
	return Stream.Capacity();
}

size_t SegmentedReaderWriterImpl::getPreferredBlockSize() const {
	return ReservedSizeSegmentedBuffer::BLOCK_SIZE;
}


/**********************************************************************************************
* SegmentedReaderWriterImpl::Fixed_Capacity -- alwyas returns false
* 
*********************************************************************************************/
bool SegmentedReaderWriterImpl::fixedCapacity() const {
	return false;
}

//Get raw pointer to data
const void * SegmentedReaderWriterImpl::raw(size_t idx, size_t& out_length) const {
	return Stream.raw(idx,(uint32_t &)out_length);
}


