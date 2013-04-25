/**
 * \file   PNGWriter.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-04-23
 *
 * \brief Header file for the PNGWriter class.
 */

#ifndef PNGWRITER_9MF72TF9
#define PNGWRITER_9MF72TF9

#include <ostream>

using namespace std;

#include <png.h>

#include <Object.h>
#include <Output.h>

using namespace cppapp;

/**
 * \todo Write documentation for class PNGWriter.
 */
class PNGWriter : public Object {
private:
	PNGWriter(const PNGWriter& other);
	
	Ref<Output> output_;

	png_structp writeStruct_;
	png_infop   infoStruct_;
	
	void finalize();
	
	static void writeCallback(png_structp writeStruct,
						 png_bytep data,
						 png_size_t length);
	static void flushCallback(png_structp writeStruct);

	static void errorCallback(png_structp writeStruct, png_const_charp errorMsg);

	
public:
	PNGWriter(Ref<Output> output);
	virtual ~PNGWriter() {}
	
	void start(int width, int height);
	void end();
	
	void writeRow(void *row);
};

#endif /* end of include guard: PNGWRITER_9MF72TF9 */

