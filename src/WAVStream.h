/**
 * \file   WAVStream.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-22
 *
 * \brief Header file for the WAVStream class.
 */

#ifndef WAVSTREAM_DWWNH2AB
#define WAVSTREAM_DWWNH2AB

#include <string>
#include <ostream>
#include <vector>

using namespace std;

#include <Object.h>
#include <Input.h>

using namespace cppapp;

#include "Frontend.h"
#include "Backend.h"


struct WAVFormat {
	static const char *CHUNK_ID;
	static const char *CHUNK_FORMAT;
	
	static const char *FORMAT_SUBCHUNK_ID;
	static const char *INF1_SUBCHUNK_ID;
	static const char *DATA_SUBCHUNK_ID;
	
	static const int FORMAT_SUBCHUNK_SIZE = 76;
	
	int audioFormat;
	int channelCount;
	int sampleRate;
	int byteRate;
	int blockAlign;
	int bitsPerSample;
};


/**
 * \todo Write documentation for class WAVStream.
 */
class WAVStream : public Frontend {
private:
	Ref<Input>      input_;

	vector<char>    stringBuffer_;
	
	bool            formatRead_;
	bool            dataRead_;
	
	WAVFormat       format_;
	string          inf1_;
	
	int             dataBufferSize_;
	vector<int16_t> dataBuffer_;
	vector<Complex> outputBuffer_;
	
	template<class T>
	T readScalar()
	{
		T result;
		input_->getStream()->read((char*)&result, sizeof(T));
		return result;
	}
	
	int32_t readInt32();
	int16_t readInt16();
	string readString(int length);
	
	void readFormatSubchunk(int size);
	void readInf1Subchunk(int size);
	void readDataSubchunk(int size);
	void readUnknownSubchunk(int size);
	int readSubchunk();
	
	WAVStream(const WAVStream& other);
	
public:
	WAVStream(Ref<Input> input);
	virtual ~WAVStream() {}
	
	void setBackend(Ref<Backend> backend) { backend_ = backend; }
	
	virtual void run();
};

#endif /* end of include guard: WAVSTREAM_DWWNH2AB */

