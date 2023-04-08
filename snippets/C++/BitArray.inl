BitArray::Reference::Reference(uint8_t& byte, uint8_t mask) :
	mByte(byte),
	mMask(mask)
{
}



BitArray::Reference::Reference(const Reference& rhs) :
	mByte(rhs.mByte),
	mMask(rhs.mMask)
{
}



BitArray::Reference& BitArray::Reference::operator =(const Reference& rhs)
{
	mByte = rhs.mByte;
	mMask = rhs.mMask;
	return *this;
}



BitArray::Reference& BitArray::Reference::operator =(bool value)
{
	if(value)
		mByte |= mMask;
	else
		mByte &= ~mMask;
	return *this;
}



BitArray::Reference::operator bool() const
{
	return (mByte & mMask) != 0;
}



bool BitArray::Reference::operator ~()
{
	return (mByte & mMask) == 0;
}



BitArray::Reference& BitArray::Reference::Flip()
{
	mByte ^= mMask;
	return *this;
}



bool BitArray::operator ==(const BitArray& rhs) const
{
	return (mSize == rhs.mSize) && (memcmp(mBits, rhs.mBits, mDataSize) == 0);
}



bool BitArray::operator !=(const BitArray& rhs) const
{
	return !(*this == rhs);
}



size_t BitArray::Size() const
{
	return mSize;
}



size_t BitArray::DataSize() const
{
	return mDataSize;
}



bool BitArray::operator [] (size_t index) const
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	return (mBits[offset] & mask) != 0;
}



BitArray::Reference BitArray::operator [] (size_t index)
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	return Reference(mBits[offset], mask);
}



bool BitArray::Test(size_t index) const
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	return (mBits[offset] & mask) != 0;
}



bool BitArray::Get(size_t index) const
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	return (mBits[offset] & mask) != 0;
}



void BitArray::Set()
{
	memset(mBits, 0xFF, mDataSize);
}



void BitArray::Set(size_t index)
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	mBits[offset] |= mask;
}



void BitArray::Set(size_t index, bool value)
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	if(value)
		mBits[offset] |= mask;
	else
		mBits[offset] &= ~mask;
}



void BitArray::Reset()
{
	memset(mBits, 0x00, mDataSize);
}



void BitArray::Reset(size_t index)
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	mBits[offset] &= ~mask;
}



void BitArray::Flip(size_t index)
{
	assert(mSize > index);
	const size_t offset = index >> 3;
	const uint8_t mask = 1 << (index & 7);
	mBits[offset] ^= mask;
}
