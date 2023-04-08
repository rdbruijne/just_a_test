#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>

template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS = 10>
class RingBuffer
{
public:
	RingBuffer();
	~RingBuffer();

	RingBuffer(RingBuffer&) = delete;
	RingBuffer(RingBuffer&&) = delete;
	RingBuffer& operator =(RingBuffer&) = delete;
	RingBuffer& operator =(RingBuffer&&) = delete;

	bool push(const TYPE& item);
	bool push(const TYPE&& item);
	std::optional<TYPE> pop();

	bool peek();

	size_t count();
	constexpr size_t size();

private:
	bool waitForPush();
	size_t safeAddress(size_t n);

	TYPE* mData = nullptr;
	size_t mReadIx = 0;
	size_t mWriteIx = 0;

	// multi-thread support
	std::mutex mMutex;
	std::mutex mConditionMutex;
	std::condition_variable mCondition;
};



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
RingBuffer<TYPE, SIZE, TIMEOUT_MS>::RingBuffer() :
	mData(static_cast<TYPE*>(malloc(SIZE * sizeof(TYPE))))
{
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
RingBuffer<TYPE, SIZE, TIMEOUT_MS>::~RingBuffer()
{
	free(mData);
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
bool RingBuffer<TYPE, SIZE, TIMEOUT_MS>::push(const TYPE& item)
{
	if(!waitForPush())
		return false;

	std::lock_guard<std::mutex> lock(mMutex);

	mData[safeAddress(mWriteIx)] = item;
	mWriteIx++;

	return true;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
bool RingBuffer<TYPE, SIZE, TIMEOUT_MS>::push(const TYPE&& item)
{
	if(!waitForPush())
		return false;

	std::lock_guard<std::mutex> lock(mMutex);

	mData[safeAddress(mWriteIx)] = std::move(item);
	mWriteIx++;

	return true;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
std::optional<TYPE> RingBuffer<TYPE, SIZE, TIMEOUT_MS>::pop()
{
	if(!peek())
		return std::nullopt;

	std::lock_guard<std::mutex> lock(mMutex);

	if(!peek())
		return std::nullopt;

	const TYPE& item = mData[safeAddress(mReadIx)];

	mReadIx++;
	mCondition.notify_one();
	return item;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
bool RingBuffer<TYPE, SIZE, TIMEOUT_MS>::peek()
{
	return mReadIx != mWriteIx;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
size_t RingBuffer<TYPE, SIZE, TIMEOUT_MS>::count()
{
	if(mWriteIx > mReadIx)
		return mWriteIx - mReadIx;
	return mReadIx - mWriteIx;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
constexpr size_t RingBuffer<TYPE, SIZE, TIMEOUT_MS>::size()
{
	return SIZE;
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
bool RingBuffer<TYPE, SIZE, TIMEOUT_MS>::waitForPush()
{
	std::unique_lock<std::mutex> uniqueLock(mConditionMutex);
	return mCondition.wait_for(uniqueLock, std::chrono::milliseconds(TIMEOUT_MS), [this]() -> bool { return count() < size(); });
}



template<typename TYPE, size_t SIZE, size_t TIMEOUT_MS>
size_t RingBuffer<TYPE, SIZE, TIMEOUT_MS>::safeAddress(size_t n)
{
	if constexpr ((SIZE & (SIZE - 1)) == 0)
	{
		return n & (SIZE - 1);
	}
	else
	{
		return n % SIZE;
	}
}
