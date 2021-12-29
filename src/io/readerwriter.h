#ifndef WSS_READERWRITER_H
#define WSS_READERWRITER_H

#include "ireaderwriter.h"

namespace wss {
	class ReaderWriter {
	public:
		enum StreamSeekType {
			CUR,
			END,
			BEGIN
		};

		typedef int32_t pos_type;
		static const size_t STREAM_ERROR = static_cast<size_t>(-1);
	public:
		static IReaderWriter * Create_Default_Interface();

		ReaderWriter(IReaderWriter * stream_interface);
		ReaderWriter(const ReaderWriter& other);

		~ReaderWriter();

		size_t read(void * dst, size_t length);
		size_t readUntilDelim(void * dst, size_t length, char delim, bool &wasDelimHit);
		size_t write(const void * src, size_t length);
		size_t erase(size_t idx, size_t length);

		size_t tellRead() const;
		size_t seekRead(pos_type distance, StreamSeekType seek_type);
		size_t unreadSize() const;

		size_t tellWrite() const;
		size_t seekWrite(pos_type distance, StreamSeekType seek_type);

		bool empty() const;
		size_t size() const;
		size_t getPreferredBlockSize() const;

		//Get raw pointer to data and the maximum contiguous length for a given index
		const void * raw(size_t idx, size_t& out_length) const;

		// Type safe methods
		static_assert(sizeof(float)==4, "Float wrong size" );
		static_assert(sizeof(double)==8, "DoubleWrongSize" );

		size_t writeFixed8(uint8_t val)  { return write(&val, sizeof(val)); }
		size_t writeFixed32(int32_t val) { return write(&val, sizeof(val)); }
		size_t writeFixed32(uint32_t val) { return write(&val, sizeof(val)); }
		size_t writeFixed32(float val) { return write(&val, sizeof(val)); }
		size_t writeFixed64(int64_t val) { return write(&val, sizeof(val)); }
		size_t writeFixed64(uint64_t val) { return write(&val, sizeof(val)); }
		size_t writeFixed64(double val) { return write(&val, sizeof(val)); }

		size_t readFixed8(uint8_t& val) { return read(&val, sizeof(val)); }
		size_t readFixed32(int32_t& val) { return read(&val, sizeof(val)); }
		size_t readFixed32(uint32_t& val) { return read(&val, sizeof(val)); }
		size_t readFixed32(float& val) { return read(&val, sizeof(val)); }
		size_t readFixed64(int64_t& val) { return read(&val, sizeof(val)); }
		size_t readFixed64(uint64_t& val) { return read(&val, sizeof(val)); }
		size_t readFixed64(double& val) { return read(&val, sizeof(val)); }

		// VarInt support
		static const size_t MaxVarInt32 = sizeof(uint32_t) + 1;
		static const size_t MaxVarInt64 = sizeof(uint64_t) + 2;

		size_t readVarUInt32(uint32_t& val);
		size_t readVarSInt32(int32_t& val);  
		size_t readVarUInt64(uint64_t& val);
		size_t readVarSInt64(int64_t& val);

		size_t writeVarUInt32(uint32_t val)
		{
			uint8_t buffer[MaxVarInt32];
			size_t length = WriteVarInt32Helper(val, buffer);
			return write(buffer, length);
		}

		size_t writeVarUInt64(uint64_t val)
		{
			uint8_t buffer[MaxVarInt64];
			size_t length = WriteVarInt64Helper(val, buffer);
			return write(buffer, length);
		}

		size_t writeVarSInt32(int32_t val) { return writeVarUInt32(zigZagEncode32(val)); }
		size_t writeVarSInt64(int64_t val) { return writeVarUInt64(zigZagEncode64(val)); }

	private:

		static uint32_t zigZagEncode32(int32_t n) { return static_cast<uint32_t>((n << 1) ^ (n >> 31)); }
		static uint64_t zigZagEncode64(int64_t n) { return static_cast<uint64_t>((n << 1) ^ (n >> 63)); }

		static int32_t zigZagDecode32(uint32_t n) { return static_cast<int32_t>((n >> 1) ^ -(n & 1)); }
		static int64_t zigZagDecode64(uint64_t n) { return static_cast<int64_t>((n >> 1) ^ -(n & 1)); }

		static size_t WriteVarInt32Helper(uint32_t value, uint8_t* target);
		static size_t WriteVarInt64Helper(uint64_t value, uint8_t* target);

		size_t ReadCursor;
		size_t WriteCursor;

		std::shared_ptr<IReaderWriter> Impl;
	};
}

#endif //
