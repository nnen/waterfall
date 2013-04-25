/**
 * \file   WAVStream.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-22
 * 
 * \brief Implementation file for the WAVStream class.
 */

#include "WAVStream.h"


const char *WAVFormat::CHUNK_ID = "RIFF";
const char *WAVFormat::CHUNK_FORMAT = "WAVE";

const char *WAVFormat::FORMAT_SUBCHUNK_ID = "fmt ";
const char *WAVFormat::INF1_SUBCHUNK_ID = "inf1";
const char *WAVFormat::DATA_SUBCHUNK_ID = "data";


/**
 *
 */
int32_t WAVStream::readInt32()
{
	return readScalar<int32_t>();
}


int16_t WAVStream::readInt16()
{
	return readScalar<int16_t>();
}


/**
 *
 */
string WAVStream::readString(int length)
{
	if ((int)stringBuffer_.size() < (length + 1)) {
		stringBuffer_.resize(length + 1);
	}
	stringBuffer_[length] = 0;
	input_->getStream()->read(&(stringBuffer_[0]), length);
	string result(&(stringBuffer_[0]));
	return result;
}


/**
 *
 */
void WAVStream::readFormatSubchunk(int size)
{
	format_.audioFormat = readInt16();
	format_.channelCount = readInt16();
	format_.sampleRate = readInt32();
	format_.byteRate = readInt32();
	format_.blockAlign = readInt16();
	format_.bitsPerSample = readInt16();
}


/**
 *
 */
void WAVStream::readInf1Subchunk(int size)
{
	inf1_ = readString(size);
	cerr << inf1_ << endl;
}


/**
 *
 */
void WAVStream::readDataSubchunk(int size)
{
	int rawBufferSize = dataBufferSize_ * format_.blockAlign;
	int bufferCount = size / rawBufferSize;
	int bufferRemainder = size % rawBufferSize;
	
	dataBuffer_.resize(dataBufferSize_ * format_.channelCount);
	outputBuffer_.resize(dataBufferSize_);
	
	for (int i = 0; i < bufferCount; i++) {
		input_->getStream()->read((char*)&(dataBuffer_[0]), rawBufferSize);
		
		for (int sample = 0; sample < dataBufferSize_; sample++) {
			outputBuffer_[sample].real = (double)dataBuffer_[sample * 2];
			outputBuffer_[sample].imag = (double)dataBuffer_[sample * 2 + 1];
		}
		
		if (backend_.isNotNull()) {
			backend_->process(outputBuffer_);
		}
	}
	
	if (bufferRemainder > 0) {
		int blockCount = bufferRemainder / format_.blockAlign;
		int sampleCount = blockCount * format_.channelCount;
		
		input_->getStream()->read((char*)&(dataBuffer_[0]), bufferRemainder);
		
		outputBuffer_.resize(sampleCount);
		for (int sample = 0; sample < sampleCount; sample++) {
			outputBuffer_[sample].real = (double)dataBuffer_[sample * 2];
			outputBuffer_[sample].imag = (double)dataBuffer_[sample * 2 + 1];
		}
		
		if (backend_.isNotNull()) {
			backend_->process(outputBuffer_);
		}
	}
}


/**
 *
 */
void WAVStream::readUnknownSubchunk(int size)
{
	input_->getStream()->ignore(size);
}


/**
 *
 */
int WAVStream::readSubchunk()
{
	string subchunkId = readString(4);
	int size = readInt32();
	
	cerr << "CHUNK " << subchunkId << ", SIZE = " << size << endl;
	
	if (subchunkId.compare(WAVFormat::FORMAT_SUBCHUNK_ID) == 0) {
		readFormatSubchunk(size);
	} else if (subchunkId.compare(WAVFormat::INF1_SUBCHUNK_ID) == 0) {
		readInf1Subchunk(size);
	} else if (subchunkId.compare(WAVFormat::DATA_SUBCHUNK_ID) == 0) {
		if (!dataRead_) {
			StreamInfo info;
			info.knownLength = false;
			info.length = 0;
			info.sampleRate = format_.sampleRate;
			if (backend_.isNotNull())
				backend_->startStream(info);
			dataRead_ = true;
		}
		readDataSubchunk(size);
	} else {
		readUnknownSubchunk(size);
	}
	
	return size + 4;
}


/**
 * Constructor.
 */
WAVStream::WAVStream(Ref<Input> input) :
	input_(input), dataBufferSize_(1024)
{
}


/**
 *
 */
void WAVStream::run()
{
	//char chunkID[5];
	//chunkID[4] = 0;
	//input_->getStream()->read(chunkID, 4);
	
	//cout << "CHUNK ID: " << chunkID << endl;
	
	string chunkId = readString(4);
	if (chunkId.compare(WAVFormat::CHUNK_ID) != 0) {
		cerr << "ERROR: Invalid chunk ID. Stream may not be in WAV format." << endl;
		return;
	}
	
	int32_t chunkSize = readInt32();
	
	string chunkFormat = readString(4);
	if (chunkFormat.compare(WAVFormat::CHUNK_FORMAT) != 0) {
		cerr << "ERROR: Invalid chunk format. Stream may not be in WAV format." << endl;
		return;
	}
	
	chunkSize -= 4;
	
	formatRead_ = false;
	dataRead_ = false;
	
	while (chunkSize > 0) {
		chunkSize -= readSubchunk();
	}
	
	if (backend_.isNotNull())
		backend_->endStream();
}


