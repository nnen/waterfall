/**
 * \file   RingBuffer.h
 * \author Jan Milík <milikjan@fit.cvut.cz>
 * \date   2013-05-15
 *
 * \brief  Header file for the RingBuffer class.
 */

#ifndef RINGBUFFER_HSQMLSDG
#define RINGBUFFER_HSQMLSDG

/**
 * \todo Write documentation for class RingBuffer.
 *
 * Items enter at the HEAD of the buffer and exit at the TAIL.
 */
template<class T>
class RingBuffer {
private:
	T   *buffer_;
	int  capacity_;
	T   *head_;
	T   *tail_;
	
public:
	/**
	 * Constructor.
	 */
	RingBuffer(int capacity) :
		buffer_(NULL), capacity_(capacity), head_(NULL), tail_(NULL)
	{
		assert(capacity >= 0);

		if (capacity > 0) {
			buffer_ = new T[capacity];
			head_ = buffer_;
			tail_ = buffer_;
		}
	}
	/**
	 * Copy constructor.
	 */
	RingBuffer(const RingBuffer& other) :
		buffer_(NULL), capacity_(other.capacity_), head_(NULL), tail_(NULL)
	{
		if (capacity_ > 0) {
			buffer_ = new T[capacity_];
			head_ = buffer_ + (other.head_ - other.buffer_);
			tail_ = buffer_ + (other.tail_ - other.buffer_);
			
			if (head_ >= tail_) {
				memcpy(tail_, other.tail_, sizeof(T) * getSize());
			} else {
				memcpy(tail_, other.tail_,
					  sizeof(T) * (buffer_ + capacity_ - tail_));
				memcpy(tail_, other.buffer_,
					  sizeof(T) * (head_ - buffer_));
			}
		}
	}
	/**
	 * Destructor.
	 */
	~RingBuffer()
	{
		delete [] buffer_;
		buffer_ = NULL;
		capacity_ = 0;
		head_ = NULL;
		tail_ = NULL;
	}
	
	inline bool isEmpty()     const { return head_ == tail_; }
	inline bool isFull()      const
	{
		if (head_ + 1 == buffer_ + capacity_)
			return (tail_ == buffer_);
		return (head_ + 1 == tail_);
	}
	inline int  getCapacity() const { return capacity_; }
	
	int  getSize()     const
	{
		if (head_ >= tail_)
			return head_ - tail_;
		return capacity_ - (tail_ - head_);
	}
	
	bool tryPop(T *item)
	{
		if (isEmpty()) return false;
		
		if (item != NULL)
			*item = *tail_;
		tail_++;
		if (tail_ >= buffer_ + capacity_)
			tail_ = buffer_;

		return true;
	}
	
	T pop(T defaultValue)
	{
		T result;
		if (tryPop(&result)) return result;
		return defaultValue;
	}
	
	void push(const T &item)
	{
		if (isFull()) tryPop(NULL);
		
		*head_ = item;
		head_++;
		
		if (head_ >= buffer_ + capacity_)
			head_ = buffer_;
	}
};


template<class T>
class FragmentedRingBuffer {
public:
	typedef T  ValueType;
	typedef T* Ptr;

private:
	int  capacity_;
	
	int  chunkSize_;
	int  chunkCount_;
	Ptr *chunks_;
	
	struct ChunkItem {
		Ptr *chunk;
		Ptr  item;
		
		ChunkItem() : chunk(NULL), item(NULL) {}
		ChunkItem(Ptr *chunk, Ptr item) : chunk(chunk), item(item) {}
		
		inline int getOffset() const
		{
			return (item - *chunk);
		}
		
		inline ChunkItem advance(const FragmentedRingBuffer<T> &buffer)
		{
			ChunkItem result = *this;
			
			result.item++;
			
			if (result.item >= (*result.chunk + buffer.chunkSize_)) {
				result.chunk++;
				if (result.chunk >= (buffer.chunks_ + buffer.chunkCount_))
					result.chunk = buffer.chunks_;
				result.item = *result.chunk;
			}
			
			return result;
		}

		inline const ChunkItem& operator=(const ChunkItem& other)
		{
			chunk = other.chunk;
			item = other.item;
			return other;
		}
		
		//inline bool operator==(const ChunkItem &other) { return item == other.item; }
	};
	
	ChunkItem head_;
	ChunkItem tail_;
	int size_;
	
public:
	FragmentedRingBuffer(int capacity, int chunkSize) :
		capacity_(capacity),
		chunkSize_(chunkSize), chunkCount_(0), chunks_(NULL),
		head_(), tail_(), size_(0)
	{
		chunkCount_ = capacity_ / chunkSize_;
		if ((capacity % chunkSize_) > 0) chunkCount_ += 1;
		capacity_ = chunkCount_ * chunkSize_;
		
		chunks_ = new Ptr[chunkCount_];
		
		for (int i = 0; i < chunkCount_; i++) {
			chunks_[i] = new T[chunkSize_];
		}
		
		head_ = ChunkItem(chunks_, chunks_[0]);
		tail_ = head_;
		
		assert(head_.chunk == chunks_);
		assert(head_.item == chunks_[0]);
	}
	
	FragmentedRingBuffer(const FragmentedRingBuffer<T>& other) :
		capacity_(other.capacity_),
		chunkSize_(other.chunkSize_), chunkCount_(other.chunkCount_), chunks_(NULL),
		head_(), tail_(), size_(other.size_)
	{
		chunks_ = new Ptr[chunkCount_];
		
		for (int i = 0; i < chunkCount_; i++) {
			chunks_[0] = new T[chunkSize_];
		}
	}
	
	~FragmentedRingBuffer()
	{
		for (int i = 0; i < chunkCount_; i++) {
			delete [] chunks_[i];
			chunks_[i] = NULL;
		}
		
		delete [] chunks_;
		chunks_ = NULL;
	}
	
	inline int  getCapacity() const { return capacity_; }
	inline int  getSize() const { return size_; }
	inline bool isEmpty() const { return (size_ == 0); }
	inline bool isFull() const
	{
		if (head_.item < (*head_.chunk + chunkSize_ - 1)) {
			return ((head_.item + 1) == (tail_.item));
		} else {
			Ptr *nextChunk = chunks_ + (((head_.chunk + 1) - chunks_) % chunkCount_);
			return ((nextChunk == tail_.chunk) && (tail_.item == *tail_.chunk));
		}
	}
	
	bool tryPop(T *item)
	{
		// Return if the buffer is empty.
		if (isEmpty()) return false;
		
		// Get the popped item.
		if (item != NULL)
			*item = *tail_.item;
		
		// Move the tail of the buffer.
		tail_ = tail_.advance(*this);
		//tail_.item++;
		//if (tail_.item >= *tail_.chunk + chunkSize_) {
		//	tail_.chunk++;
		//	if (tail_.chunk >= chunks_ + chunkCount_)
		//		tail_.chunk = chunks_;
		//	tail_.item = *tail_.chunk;
		//}
		
		size_--;
		assert(size_ >= 0);
		
		return true;
	}
	
	void push(const T &item)
	{
		if (isFull()) tryPop(NULL);
		
		// Copy the pushed item.
		*head_.item = item;
		
		// Move the head fo the buffer.
		head_ = head_.advance(*this);
		//head_.item++;
		//if (head_.item >= *head_.chunk + chunkSize_) {
		//	head_.chunk++;
		//}
		//
		//if (head_ >= buffer_ + capacity_)
		//	head_ = buffer_;
		
		size_++;
		assert(size_ < capacity_);
	}
	
	inline ValueType& at(int index)
	{
		while (index < 0)
			index += getSize();
		
		int chunkOffset = tail_.getOffset() + index;
		int chunkIndex = ((tail_.chunk - chunks_) +
			(chunkOffset / chunkSize_)) % chunkCount_;
		int itemIndex = chunkOffset % chunkSize_;
		
		return chunks_[chunkIndex][itemIndex];
	}
};


#endif /* end of include guard: RINGBUFFER_HSQMLSDG */

