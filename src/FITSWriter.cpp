/**
 * \file   FITSWriter.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-06-29
 * 
 * \brief  Implementation file for the FITSWriter class.
 */

#include "FITSWriter.h"


#define CHECK_STATUS(errorMsg) { \
	if (status_ && (lastStatus_ != status_)) { \
		LOG_ERROR(errorMsg << " (status: " << status_ << ")"); } \
	lastStatus_ = status_; \
}


FITSWriter::FITSWriter() :
	file_(NULL), status_(0), lastStatus_(0)
{ }


FITSWriter::~FITSWriter()
{ }


void FITSWriter::open(string fileName)
{
	fileName_ = fileName;

	fits_create_file(&file_, fileName_.c_str(), &status_);
	CHECK_STATUS("Failed to open FITS file \"" << fileName_ << "\".");
}


void FITSWriter::close()
{
	fits_close_file(file_, &status_);
	CHECK_STATUS("Failed to close FITS file \"" << fileName_ << "\".");
	
	file_       = NULL;
	status_     = 0;
	lastStatus_ = 0;
}


void FITSWriter::createImage(long width, long height, int type)
{
	long dimensions[2] = { width, height };
	fits_create_img(file_, type, 2, dimensions, &status_);
	CHECK_STATUS("Failed to create primary HDU in FITS file.");
}


void FITSWriter::writeHeader(const char *keyword,
					    int         type,
					    void       *value,
					    const char *comment)
{
	fits_write_key(file_, type, keyword, value, comment, &status_);
	CHECK_STATUS("Failed to write FITS header \"" << keyword << "\".");
}


void FITSWriter::writeHeader(const char *keyword,
					    const char *value,
					    const char *comment)
{
	char valueBuffer[FLEN_VALUE];
	strcpy(valueBuffer, value);
	
	writeHeader(keyword, TSTRING, (void*)valueBuffer, comment);
}


void FITSWriter::writeHeader(const char *keyword,
					    float       value,
					    const char *comment)
{
	writeHeader(keyword, TFLOAT, (void*)&value, comment);
}


void FITSWriter::writeHeader(const char *keyword,
					    int         value,
					    const char *comment)
{
	writeHeader(keyword, TINT, (void*)&value, comment);
}


void FITSWriter::comment(const char *value)
{
	fits_write_comment(file_, value, &status_);
	CHECK_STATUS("Failed to write FITS header comment (\"" << value << "\").");
}


void FITSWriter::date()
{
	fits_write_date(file_, &status_);
	CHECK_STATUS("Failed to write FITS DATE header.");
}


