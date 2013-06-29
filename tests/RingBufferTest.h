/**
 * \file   RingBufferTest.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-06-06
 *
 * \brief  Header file for the RingBufferTest class.
 */

#ifndef RINGBUFFERTEST_TEKSJAT1
#define RINGBUFFERTEST_TEKSJAT1

#include <cppapp/cppapp.h>
using namespace cppapp;

#include "../src/RingBuffer.h"


/**
 * \todo Write documentation for class RingBufferTest.
 */
class RingBufferTest : public TestCase {
public:
	/**
	 * Constructor.
	 */
	//RingBufferTest(const char *name) : TestCase(name)
	//{
	//	TEST_ADD(RingBufferTest, testConstructor);
	//	TEST_ADD(RingBufferTest, testCopyConstructor);
	//	TEST_ADD(RingBufferTest, testCopyConstructorOverlap);
	//	TEST_ADD(RingBufferTest, testPush);
	//}
	
	void testConstructor()
	{
		RingBuffer<int> buffer(100);
		
		TEST_EQUALS(100, buffer.getCapacity(), "created buffer has wrong capacity");
		TEST_EQUALS(0, buffer.getSize(), "created buffer should have size 0");
		TEST_ASSERT(buffer.isEmpty(), "created buffer should be empty");
		TEST_ASSERT(!buffer.isFull(), "created buffer should not be full");
	}
	
	void testCopyConstructor()
	{
		RingBuffer<int> a(100);
		
		for (int i = 0; i < 10; i++)
			a.push(i);
		
		TEST_EQUALS(10, a.getSize(), "just checking");
		
		RingBuffer<int> b(a);
		
		TEST_EQUALS(10, a.getSize(),
				  "copied buffer's size should not change");
		TEST_EQUALS(a.getSize(), b.getSize(),
				  "copy's size should be the same as original");
		
		for (int i = 0; i < a.getSize(); i++) {
			int valueA;
			int valueB;
			TEST_ASSERT(b.tryPop(&valueB),
					  "there should still be elements in the buffer");
			TEST_ASSERT(a.tryPop(&valueA),
					  "something is wrong with tryPop()");
			TEST_EQUALS(valueA, valueB,
					  "the elements poped from the copy should be equal "
					  "to the original");
		}
	}
	
	void testCopyConstructorOverlap()
	{
		int size = 100;
		RingBuffer<int> a(size);
		
		for (int i = 0; i < (size * 3); i++)
			a.push(i);
		
		TEST_EQUALS(size - 1, a.getSize(), "just checking");
		
		RingBuffer<int> b(a);
		
		TEST_EQUALS(size - 1, a.getSize(),
				  "copied buffer's size should not change");
		TEST_EQUALS(a.getSize(), b.getSize(),
				  "copy's size should be the same as original");
		
		for (int i = 0; i < a.getSize(); i++) {
			int valueA;
			int valueB;
			TEST_ASSERT(b.tryPop(&valueB),
					  "there should still be elements in the buffer");
			TEST_ASSERT(a.tryPop(&valueA),
					  "something is wrong with tryPop()");
			TEST_EQUALS(valueA, valueB,
					  "the elements poped from the copy should be equal "
					  "to the original");
		}

	}
	
	void testPush()
	{
		int size = 100;
		RingBuffer<int> buffer(size);
		
		for (int i = 0; i < (size * 5); i++) {
			buffer.push(i);
			
			TEST_ASSERT(!buffer.isEmpty(),
					  "after append, buffer should not be empty");
			if ((i + 1) >= (size - 1)) {
				TEST_EQUALS(size - 1, buffer.getSize(),
						  "buffer size should increase with each append until "
						  "it is full");
				TEST_ASSERT(buffer.isFull(),
						  "after append certain number of elements, the buffer "
						  "should be full");
			} else {
				TEST_EQUALS(i + 1, buffer.getSize(),
						  "buffer size should increase with each append until "
						  "it is full");
				TEST_ASSERT(!buffer.isFull(),
						  "buffer should not be full until its capacity is filled");
			}
		}
	}

	void testTryPop()
	{
		int size = 100;
		RingBuffer<int> buffer(size);
		
		for (int i = 0; i < 20; i++) {
			buffer.push(i);
		}
		
		for (int i = 0; i < 20; i++) {
			int value;
			TEST_ASSERT(buffer.tryPop(&value),
					  "all of the appended elements should be pop-able");
			TEST_EQUALS(i, value,
					  "the popped elements should have the right value");
		}

		TEST_ASSERT(buffer.isEmpty(), "buffer should be empty now");
		
		for (int i = 0; i < size - 1; i++) {
			buffer.push(i);
		}

		TEST_ASSERT(buffer.isFull(),
				  "buffer should be full now");
		
		for (int i = 0; i < size - 1; i++) {
			int value;
			TEST_ASSERT(buffer.tryPop(&value),
					  "all of the appended elements should be pop-able");
			TEST_EQUALS(i, value,
					  "the popped elements should have the right value");
		}
		
		TEST_ASSERT(buffer.isEmpty(),
				  "buffer should be empty now");
	}
};

RUN_SUITE(RingBufferTest);


class FragmentedBufferTest : public TestCase {
public:
	//FragmentedBufferTest(const char *name) : TestCase(name)
	//{
	//}
	
	virtual void initTests()
	{
		TEST_ADD(FragmentedBufferTest, testConstructor);
		TEST_ADD(FragmentedBufferTest, testPush);
	}
	
	void testConstructor(int capacity, int chunkSize)
	{
		FragmentedRingBuffer<int> buffer(capacity, chunkSize);
		TEST_ASSERT(buffer.getCapacity() >= capacity,
				  "created buffer has wrong capacity");
		TEST_EQUALS(0, buffer.getSize(), "created buffer has wrong size");
		TEST_ASSERT(buffer.isEmpty(), "created buffer should be empty");
		TEST_ASSERT(!buffer.isFull(), "created buffer should not be full");
	}
	
	void testConstructor()
	{
		// Capacity == Chunk Size
		testConstructor(1024, 1024);
		// Capacity == n * Chunk Size
		testConstructor(1024, 256);
		// Capacity > Chunk Size
		testConstructor(1024, 200);
		// Capacity < Chunk Size
		testConstructor(1024, 2048);
	}

	void testPush(int capacity, int chunkSize)
	{
		FragmentedRingBuffer<int> buffer(capacity, chunkSize);
		capacity = buffer.getCapacity();
		
		int itemCount = buffer.getCapacity() * 7 / 3;
		
		for (int i = 0; i < itemCount; i++) {
			buffer.push(i);
			
			TEST_EQUALS((i >= capacity - 1 ? capacity - 1 : i + 1),
					  buffer.getSize(),
					  "buffer has the wrong size");
			TEST_EQUALS(i, buffer.at(-1), "buffer has the wrong content");
			
			for (int j = 0; j < buffer.getSize(); j++) {
				TEST_EQUALS(i - j, buffer.at(-1 - j),
						  "buffer has the wrong content");
			}
		}
	}
	
	void testPush()
	{
		int capacity = 256;
		
		// capacity == chunk size
		testPush(capacity, capacity);
		// capacity == n * chunk size
		testPush(capacity, capacity / 4);
		// capacity > chunk size
		testPush(capacity, 100);
		// n * capacity == chunk size
		testPush(capacity, capacity * 2);
		// capacity < chunk size
		testPush(capacity, 300);
	}
};

RUN_SUITE(FragmentedBufferTest);


#endif /* end of include guard: RINGBUFFERTEST_TEKSJAT1 */

