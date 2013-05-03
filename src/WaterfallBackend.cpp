/**
 * \file   WaterfallBackend.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-25
 * 
 * \brief Implementation file for the WaterfallBackend class.
 */

#include "WaterfallBackend.h"

#include <Logger.h>

#include <iostream>
using namespace std;

/**
 *
 */
void WaterfallBackend::makeSnapshot()
{
	//int fits_create_file(fitsfile **fptr, char *filename, int *status)
	//int fits_close_file(fitsfile *fptr, int *status)
	
	//int fits_create_img(fitsfile *fptr, int bitpix, int naxis, 
     //                    long *naxes, int *status)
	
	//int fits_write_pix(fitsfile *fptr, int datatype, long *fpixel,
     //                   long nelements, void *array, int *status);
	
	//int fits_write_key(fitsfile *fptr, int datatype, char *keyname, 
     //                   void *value, char *comment, int *status)
	
	char *fileName = new char[1024];
	sprintf(fileName, "!snapshot_%s_%d.fits",
		   origin_.c_str(),
		   (int)timeBuffer_[0].time.tv_sec);
	
	int status = 0;
	fitsfile *fptr;
	
	LOG_INFO("Writing snapshot \"" << (fileName + 1) << "\"...");
	
	fits_create_file(&fptr, fileName, &status);
	if (status) {
		cerr << "ERROR: Failed to create FITS file (code: " << status << ")." << endl;
		return;
	}
	
	int width = rightBin_ - leftBin_;
	long dimensions[2] = { width, bufferMark_ };
	fits_create_img(fptr, FLOAT_IMG, 2, dimensions, &status);
	if (status) {
		cerr << "ERROR: Failed to create primary HDU in FITS file (code: " <<
			status << ")." << endl;
	}
	
	char ctype2[] = { 'T', 'i', 'm', 'e', 0 };
	fits_write_key(fptr, TSTRING, "CTYPE2", (void*)ctype2, "", &status);
	float crpix2 = 1;
	fits_write_key(fptr, TFLOAT, "CRPIX2", (void*)&crpix2, "", &status);
	float crval2 = (float)timeBuffer_[0].time.tv_sec;
	fits_write_key(fptr, TFLOAT, "CRVAL2", (void*)&crval2, "", &status);
	float cdelt2 = 1.0 / fftSampleRate_;
	fits_write_key(fptr, TFLOAT, "CDELT2", (void*)&cdelt2, "", &status);
	
	char ctype1[] = { 'F', 'r', 'e', 'q', 0 };
	fits_write_key(fptr, TSTRING, "CTYPE1", (void*)ctype1, "", &status);
	float crpix1 = 1.0;
	fits_write_key(fptr, TFLOAT, "CRPIX1", (void*)&crpix1, "", &status);
	float crval1 = leftFrequency_;
	fits_write_key(fptr, TFLOAT, "CRVAL1", (void*)&crval1, "", &status);
	float cdelt1 = binToFrequency();
	fits_write_key(fptr, TFLOAT, "CDELT1", (void*)&cdelt1, "", &status);
	
	char origin[origin_.size() + 1];
	origin_.copy(origin, origin_.size());
	origin[origin_.size()] = 0;
	fits_write_key(fptr, TSTRING, "ORIGIN", (void*)origin, "", &status);
	
	if (status) {
		cerr << "ERROR: Error occured while writing FITS file header (code: " <<
			status << ")." << endl;
	}
	
	long fpixel[2] = { 1, 1 };
	for (int y = 0; y < bufferMark_; y++) {
		fits_write_pix(fptr,
					TFLOAT,
					fpixel,
					width,
					(void*)(buffer_[y] + leftBin_),
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
}


void WaterfallBackend::processFFT(const fftw_complex *data, int size, DataInfo info)
{
	float *row = buffer_[bufferMark_];
	
	for (int i = 0; i < size; i++) {
		row[i] = sqrt(
			data[i][0] * data[i][0] +
			data[i][1] * data[i][1]
		);
	}
	
	timeBuffer_[bufferMark_] = info.timeOffset;
	
	if (++bufferMark_ >= bufferSize_) {
		makeSnapshot();
		bufferMark_ = 0;
	}
}


/**
 * Constructor.
 */
WaterfallBackend::WaterfallBackend(string origin,
							int bufferSize,
							float leftFrequency,
							float rightFrequency) :
	origin_(origin),
	bufferSize_(bufferSize),
	buffer_(NULL),
	bufferMark_(0),
	leftFrequency_(leftFrequency),
	rightFrequency_(rightFrequency)
{
	buffer_ = new float*[bufferSize_];
	for (float **a = buffer_; a < (buffer_ + bufferSize_); a++) {
		*a = new float[bins_];
	}
	
	timeBuffer_.resize(bufferSize_);
}


/**
 * Destructor
 */
WaterfallBackend::~WaterfallBackend()
{
	for (float **a = buffer_; a < (buffer_ + bufferSize_); a++) {
		delete [] *a;
		*a = NULL;
	}
	delete [] buffer_;
	buffer_ = NULL;
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
}


/**
 *
 */
void WaterfallBackend::endStream()
{
	FFTBackend::endStream();
	
	if (bufferMark_ > 0) {
		makeSnapshot();
	}
}


