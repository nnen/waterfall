/**
 * \file   WaterfallBackend.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-25
 * 
 * \brief Implementation file for the WaterfallBackend class.
 */

#include "WaterfallBackend.h"

#include <cppapp/Logger.h>

#include <iostream>
using namespace std;


void WaterfallBackend::writeHeader(fitsfile   *file,
							const char *keyword,
							int         type,
							void       *value,
							const char *comment,
							int        *status)
{
	//char keywordBuffer[FLEN_KEYWORD];
	//char commentBuffer[FLEN_COMMENT];
	//
	//strcpy(keywordBuffer, keyword);
	//strcpy(commentBuffer, comment);
	
	//fits_write_key(file, type, keywordBuffer, value, commentBuffer, status);
	fits_write_key(file, type, keyword, value, comment, status);
}


void WaterfallBackend::writeHeader(fitsfile   *file,
							const char *keyword,
							const char *value,
							const char *comment,
							int        *status)
{
	char valueBuffer[FLEN_VALUE];
	strcpy(valueBuffer, value);
	writeHeader(file, keyword, TSTRING, (void*)valueBuffer, comment, status);
}


void WaterfallBackend::writeHeader(fitsfile   *file,
							const char *keyword,
							float       value,
							const char *comment,
							int        *status)
{
	writeHeader(file, keyword, TFLOAT, (void*)&value, comment, status);
}


void WaterfallBackend::writeHeader(fitsfile   *file,
							const char *keyword,
							int         value,
							const char *comment,
							int        *status)
{
	writeHeader(file, keyword, TINT, (void*)&value, comment, status);
}


void* WaterfallBackend::snapshotThread()
{
	MutexLock lock(&mutex_);
	
	while (!endSnapshotThread_) {
		snapshotCondition_.wait(mutex_);
		makeSnapshot();
	}

	return NULL;
}


void WaterfallBackend::makeSnapshot()
{
	WFTime time = outBuffer_.times[0];
	
	char *fileName = new char[1024];
	sprintf(fileName, "!snapshot_%s_%s.fits",
		   origin_.c_str(),
		   time.format("%Y_%m_%d_%H_%M_%S").c_str());
		   //(int)outBuffer_.times[0].seconds());
		   //(int)timeBuffer_[0].seconds());
	
	int status = 0;
	fitsfile *fptr;
	
	LOG_INFO("Writing snapshot \"" << (fileName + 1) << "\"...");
	
	fits_create_file(&fptr, fileName, &status);
	if (status) {
		cerr << "ERROR: Failed to create FITS file (code: " << status << ")." << endl;
		return;
	}
	
	int width = rightBin_ - leftBin_;
	//long dimensions[2] = { width, bufferMark_ };
	long dimensions[2] = { width, outBuffer_.mark };
	fits_create_img(fptr, FLOAT_IMG, 2, dimensions, &status);
	if (status) {
		cerr << "ERROR: Failed to create primary HDU in FITS file (code: " <<
			status << ")." << endl;
	}
	
	writeHeader(fptr, "ORIGIN", origin_.c_str(), "", &status);
	//writeHeader(fptr, "DATE", WFTime::now().format("%Y-%m-%dT%H:%M:%S").c_str(), "", &status);
	fits_write_date(fptr, &status);
	fits_write_comment(fptr, WFTime::now().format("Local time: %Y-%m-%d %H:%M:%S %Z", true).c_str(), &status);
	writeHeader(fptr, "DATE-OBS", time.format("%Y-%m-%dT%H:%M:%S").c_str(), "observation date (UTC)", &status);
	
	writeHeader(fptr, "CTYPE2", "TIME",                      "in seconds", &status);
	writeHeader(fptr, "CRPIX2", 1,                           "",           &status);
	writeHeader(fptr, "CRVAL2", (float)time.seconds(),       "",           &status);
	writeHeader(fptr, "CDELT2", 1.f / (float)fftSampleRate_, "",           &status);
	
	writeHeader(fptr, "CTYPE1", "FREQ",                      "in Hz", &status);
	writeHeader(fptr, "CRPIX1", 1.f,                         "",      &status);
	writeHeader(fptr, "CRVAL1", (float)leftFrequency_,       "",      &status);
	writeHeader(fptr, "CDELT1", (float)binToFrequency(),     "",      &status);
	
	//char ctype2[] = { 'T', 'i', 'm', 'e', 0 };
	//fits_write_key(fptr, TSTRING, "CTYPE2", (void*)ctype2, "", &status);
	//float crpix2 = 1;
	//fits_write_key(fptr, TFLOAT, "CRPIX2", (void*)&crpix2, "", &status);
	////float crval2 = (float)timeBuffer_[0].time.tv_sec;
	//float crval2 = (float)outBuffer_.times[0].seconds();
	//fits_write_key(fptr, TFLOAT, "CRVAL2", (void*)&crval2, "", &status);
	//float cdelt2 = 1.0 / fftSampleRate_;
	//fits_write_key(fptr, TFLOAT, "CDELT2", (void*)&cdelt2, "", &status);
	
	//char ctype1[] = { 'F', 'r', 'e', 'q', 0 };
	//fits_write_key(fptr, TSTRING, "CTYPE1", (void*)ctype1, "", &status);
	//float crpix1 = 1.0;
	//fits_write_key(fptr, TFLOAT, "CRPIX1", (void*)&crpix1, "", &status);
	//float crval1 = leftFrequency_;
	//fits_write_key(fptr, TFLOAT, "CRVAL1", (void*)&crval1, "", &status);
	//float cdelt1 = binToFrequency();
	//fits_write_key(fptr, TFLOAT, "CDELT1", (void*)&cdelt1, "", &status);
	
	//char origin[origin_.size() + 1];
	//origin_.copy(origin, origin_.size());
	//origin[origin_.size()] = 0;
	//fits_write_key(fptr, TSTRING, "ORIGIN", (void*)origin, "", &status);
	
	if (status) {
		cerr << "ERROR: Error occured while writing FITS file header (code: " <<
			status << ")." << endl;
	}
	
	long fpixel[2] = { 1, 1 };
	//for (int y = 0; y < bufferMark_; y++) {
	for (int y = 0; y < outBuffer_.mark; y++) {
		fits_write_pix(fptr,
					TFLOAT,
					fpixel,
					width,
					//(void*)(buffer_[y] + leftBin_),
					(void*)(outBuffer_.getRow(y) + leftBin_),
					&status);
		fpixel[1]++;
		if (status) break;
	}
	
	if (status) {
		cerr << "ERROR: Error occured while writing data to FITS file (code: " <<
			status << ")." << endl;
	}
	
	fits_close_file(fptr, &status);
	if (status) {
		cerr << "ERROR: Failed to close FITS file (code: " << status << ")." << endl;
	}
	
	delete [] fileName;

	LOG_DEBUG("Finished writing snapshot.");
}


void WaterfallBackend::startSnapshot()
{
	MutexLock lock(&mutex_);
	
	inBuffer_.swap(outBuffer_);
	snapshotCondition_.signal();
	
	//bufferMark_ = 0;
	inBuffer_.rewind();
}


void WaterfallBackend::processFFT(const fftw_complex *data, int size, DataInfo info)
{
	//float *row = buffer_[bufferMark_];
	//float *row = inBuffer_.getRow(bufferMark_);
	float *row = inBuffer_.addRow(info.timeOffset);
	
	int halfSize = size / 2;
	for (int i = 0; i < halfSize; i++) {
		row[halfSize - i - 1] = sqrt(
			data[i][0] * data[i][0] +
			data[i][1] * data[i][1]
		);
	}
	for (int i = halfSize; i < size; i++) {
		row[size - (i - halfSize) - 1] = sqrt(
			data[i][0] * data[i][0] +
			data[i][1] * data[i][1]
		);
	}
	//for (int i = 0; i < size; i++) {
	//	row[i] = sqrt(
	//		data[i][0] * data[i][0] +
	//		data[i][1] * data[i][1]
	//	);
	//}
	
	//timeBuffer_[bufferMark_] = info.timeOffset;
	//inBuffer_.times[bufferMark_] = info.timeOffset;
	
	//if (++bufferMark_ >= bufferSize_) {
		//makeSnapshot();
		//bufferMark_ = 0;
	if (inBuffer_.isFull()) {
		startSnapshot();
	}
}


WaterfallBackend::WaterfallBackend(int    bins,
                                   int    overlap,
                                   string origin,
                                   int    bufferSize,
                                   float  leftFrequency,
                                   float  rightFrequency) :
	FFTBackend(bins, overlap),
	origin_(origin),
	bufferSize_(bufferSize),
	//buffer_(NULL),
	//bufferMark_(0),
	inBuffer_(bufferSize, bins_),
	outBuffer_(bufferSize, bins_),
	leftFrequency_((leftFrequency < rightFrequency) ? leftFrequency : rightFrequency),
	rightFrequency_((leftFrequency > rightFrequency) ? leftFrequency : rightFrequency)
{
	//timeBuffer_.resize(bufferSize_);
	LOG_DEBUG("Waterfall backend: buffer size = " << bufferSize << ", bins = " << bins_);
}


WaterfallBackend::~WaterfallBackend()
{
	//for (float **a = buffer_; a < (buffer_ + bufferSize_); a++) {
	//	delete [] *a;
	//	*a = NULL;
	//}
	//delete [] buffer_;
	//buffer_ = NULL;
}


/**
 *
 */
void WaterfallBackend::startStream(StreamInfo info)
{
	FFTBackend::startStream(info);
	
	if (leftFrequency_ == rightFrequency_) {
		leftFrequency_ = -(float)info.sampleRate;
		rightFrequency_ = (float)info.sampleRate;
		leftBin_  = 0;
		rightBin_ = bins_;
	} else {
		leftBin_  = frequencyToBin(leftFrequency_);
		rightBin_ = frequencyToBin(rightFrequency_);
	}
	
	endSnapshotThread_ = false;
	snapshotThread_ =
		new MethodThread<void, WaterfallBackend>(this,
										 &WaterfallBackend::snapshotThread);
}


/**
 *
 */
void WaterfallBackend::endStream()
{
	FFTBackend::endStream();
	
	//if (bufferMark_ > 0) {
	if (inBuffer_.mark > 0) {
		startSnapshot();
		//makeSnapshot();
	}
	
	endSnapshotThread_ = true;
	snapshotThread_->join();
	delete snapshotThread_;
	snapshotThread_ = NULL;
}


