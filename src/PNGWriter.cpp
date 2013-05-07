/**
 * \file   PNGWriter.cpp
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 * 
 * \brief Implementation file for the PNGWriter class.
 */

#include "PNGWriter.h"

#include <cstdlib>
using namespace std;


/**
 *
 */
void PNGWriter::finalize()
{
	if (infoStruct_ != NULL) png_free_data(writeStruct_, infoStruct_, PNG_FREE_ALL, -1);
	if (writeStruct_ != NULL) png_destroy_write_struct(&writeStruct_, (png_infopp)NULL);

	infoStruct_ = NULL;
	writeStruct_ = NULL;
}


void PNGWriter::writeCallback(png_structp writeStruct, png_bytep data, png_size_t length)
{
	//Here we get our IO pointer back from the read struct.
	//This is the parameter we passed to the png_set_read_fn() function.
	//Our std::istream pointer.
	png_voidp ioPtr = png_get_io_ptr(writeStruct);
	//Cast the pointer to std::istream* and read 'length' bytes into 'data'
	((std::ostream*)ioPtr)->write((char*)data, length);
}


/**
 *
 */
void PNGWriter::flushCallback(png_structp writeStruct)
{
	//Here we get our IO pointer back from the read struct.
	//This is the parameter we passed to the png_set_read_fn() function.
	//Our std::istream pointer.
	png_voidp ioPtr = png_get_io_ptr(writeStruct);
	//Cast the pointer to std::istream* and read 'length' bytes into 'data'
	((std::ostream*)ioPtr)->flush();
}


void PNGWriter::errorCallback(png_structp writeStruct, png_const_charp errorMsg)
{
	cerr << "ERROR: Error occured while writing PNG: " << errorMsg << endl;
	exit(1);
}


/**
 * Constructor.
 */
PNGWriter::PNGWriter(Ref<Output> output) :
	Object(),
	output_(output)
{
}


/**
 *
 */
void PNGWriter::start(int width, int height)
{
	// Initialize write structure
	writeStruct_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (writeStruct_ == NULL) {
		cerr << "ERROR: Could not allocate write struct.\n" << endl;
		finalize();
		return;
	}
	
	// Initialize info structure
	infoStruct_ = png_create_info_struct(writeStruct_);
	if (infoStruct_ == NULL) {
		cerr << "ERROR: Could not allocate info struct.\n" << endl;
		finalize();
		return;
	}
	
	png_set_error_fn(writeStruct_, (png_voidp)this, errorCallback, NULL);
	
	// TODO: Set up IO
	png_set_write_fn(writeStruct_, output_->getStream(), writeCallback, flushCallback);
	
	// Write header (8 bit colour depth)
	png_set_IHDR(
		writeStruct_,
		infoStruct_,
		width, height,
		8, PNG_COLOR_TYPE_GRAY,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	
	png_write_info(writeStruct_, infoStruct_);
}


/**
 *
 */
void PNGWriter::end()
{
	png_write_end(writeStruct_, NULL);
	finalize();
}


/**
 *
 */
void PNGWriter::writeRow(void *row)
{
	png_write_row(writeStruct_, (png_bytep)row);
}


