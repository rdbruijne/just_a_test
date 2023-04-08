#include "BitArray.h"

// C++
#include <bit>
#include <istream>
#include <ostream>



BitArray::BitArray(size_t size) :
	mSize(size),
	mDataSize((size + 7) / 8)
{
	if(mDataSize > 0)
	{
		mBits = static_cast<uint8_t*>(malloc(mDataSize));
		assert(mBits);
		memset(mBits, 0, mDataSize);
	}
}



BitArray::BitArray(size_t size, bool defaultSet) :
	mSize(size),
	mDataSize((size + 7) / 8)
{
	if(mDataSize > 0)
	{
		mBits = static_cast<uint8_t*>(malloc(mDataSize));
		assert(mBits);
		memset(mBits, defaultSet ? 0xFF : 0x00, mDataSize);
	}
}



BitArray::BitArray(const std::initializer_list<bool>& list) :
	mSize(list.size()),
	mDataSize((list.size() + 7) / 8)
{
	if(mDataSize > 0)
	{
		mBits = static_cast<uint8_t*>(malloc(mDataSize));
		assert(mBits);
		size_t i = mSize - 1;
		for(bool b : list)
			Set(i--, b);
	}
}



BitArray::BitArray(const std::string& str, size_t pos, size_t n, char zero, char one) :
	mSize(std::min(n, str.length() - pos)),
	mDataSize((mSize + 7) / 8)
{
	if(mDataSize > 0)
	{
		mBits = static_cast<uint8_t*>(malloc(mDataSize));
		assert(mBits);
		const char* src = str.data() + pos;
		for(size_t i = 0; i < mSize; i++, src++)
		{
			assert((*src == zero) || (*src == one));
			Set(mSize - i - 1, *src == one);
		}
	}
}



BitArray::BitArray(const BitArray& rhs) :
	mSize(rhs.mSize),
	mDataSize(rhs.mDataSize)
{
	if(mDataSize > 0)
	{
		mBits = static_cast<uint8_t*>(malloc(mDataSize));
		assert(mBits);
		memcpy(mBits, rhs.mBits, mDataSize);
	}
}



BitArray::BitArray(BitArray&& rhs) noexcept :
	mSize(std::exchange(rhs.mSize, 0)),
	mDataSize(std::exchange(rhs.mDataSize, 0)),
	mBits(std::exchange(rhs.mBits, nullptr))
{
}



BitArray::~BitArray()
{
	free(mBits);
}



BitArray& BitArray::operator=(BitArray&& rhs) noexcept
{
	mSize = std::exchange(rhs.mSize, 0);
	mDataSize = std::exchange(rhs.mDataSize, 0);
	mBits = std::exchange(rhs.mBits, nullptr);
	return *this;
}



BitArray& BitArray::operator=(const BitArray& rhs)
{
	mSize = rhs.mSize;
	mDataSize = rhs.mDataSize;
	mBits = static_cast<uint8_t*>(malloc(mDataSize));
	assert(mBits);
	memcpy(mBits, rhs.mBits, mDataSize);
	return *this;
}



bool BitArray::All() const
{
	if(mSize == 0)
		return false;

	const size_t lastByteIx = mDataSize - 1;
	for(size_t i = 0; i < lastByteIx; i++)
		if(std::popcount(mBits[i]) != 8)
			return false;

	for(size_t i = lastByteIx * 8, mask = 1; i < mSize; i++, mask <<= 1)
		if((mBits[lastByteIx] & mask) == 0)
			return false;

	return true;
}



bool BitArray::Any() const
{
	if(mSize == 0)
		return false;

	const size_t lastByteIx = mDataSize - 1;
	for(size_t i = 0; i < lastByteIx; i++)
		if(std::popcount(mBits[i]) != 0)
			return true;

	for(size_t i = lastByteIx * 8, mask = 1; i < mSize; i++, mask <<= 1)
		if((mBits[lastByteIx] & mask) != 0)
			return true;

	return false;
}



bool BitArray::None() const
{
	if(mSize == 0)
		return false;

	const size_t lastByteIx = mDataSize - 1;
	for(size_t i = 0; i < lastByteIx; i++)
		if(std::popcount(mBits[i]) != 0)
			return false;

	for(size_t i = lastByteIx * 8, mask = 1; i < mSize; i++, mask <<= 1)
		if((mBits[lastByteIx] & mask) != 0)
			return false;

	return true;
}



size_t BitArray::Count() const
{
	if(mSize == 0)
		return 0;

	size_t count = 0;

	const size_t lastByteIx = mDataSize - 1;
	for(size_t i = 0; i < lastByteIx; i++)
		count += std::popcount(mBits[i]);

	for(size_t i = lastByteIx * 8, mask = 1; i < mSize; i++, mask <<= 1)
		if((mBits[lastByteIx] & mask) != 0)
			count++;

	return count;
}



BitArray operator&(const BitArray& a, const BitArray& b)
{
	if(a.Size() > b.Size())
	{
		BitArray result(a);
		result &= b;
		return result;
	}

	BitArray result(b);
	result &= a;
	return result;
}



BitArray& BitArray::operator&=(const BitArray& rhs)
{
	// apply on whole bytes
	const size_t size = mSize < rhs.mSize ? mSize : rhs.mSize;
	const size_t byteSize = size >> 3;
	for(size_t i = 0; i < byteSize; i++)
		mBits[i] = mBits[i] & rhs.mBits[i];

	// remainder bits in most significant byte
	for(size_t i = byteSize; i < mSize; i++)
	{
		if(i == byteSize && size & 7)
			mBits[i] = (mBits[byteSize] & rhs.mBits[byteSize]) & ((1 << (size & 0x7)) - 1);
		else
			mBits[i] = 0;
	}

	return *this;
}



BitArray& BitArray::operator|=(const BitArray& rhs)
{
	// apply on whole bytes
	const size_t size = mSize < rhs.mSize ? mSize : rhs.mSize;
	const size_t byteSize = size >> 3;
	for(size_t i = 0; i < byteSize; i++)
		mBits[i] = mBits[i] & rhs.mBits[i];

	// remainder bits in most significant byte
	if(size & 7)
	{
		const size_t mask = (1 << (size & 0x7)) - 1;
		const size_t keepMask = 0xFF ^ mask;
		mBits[byteSize] = (mBits[byteSize] & keepMask) | ((mBits[byteSize] | rhs.mBits[byteSize]) & mask);
	}

	return *this;
}



BitArray operator|(const BitArray& a, const BitArray& b)
{
	if(a.Size() > b.Size())
	{
		BitArray result(a);
		result |= b;
		return result;
	}

	BitArray result(b);
	result |= a;
	return result;
}



BitArray& BitArray::operator^=(const BitArray& rhs)
{
	// apply on whole bytes
	const size_t size = mSize < rhs.mSize ? mSize : rhs.mSize;
	const size_t byteSize = size >> 3;
	for(size_t i = 0; i < byteSize; i++)
		mBits[i] = mBits[i] ^ rhs.mBits[i];

	// remainder bits in most significant byte
	if(size & 7)
	{
		const size_t mask = (1 << (size & 0x7)) - 1;
		const size_t keepMask = 0xFF ^ mask;
		mBits[byteSize] = (mBits[byteSize] & keepMask) | ((mBits[byteSize] ^ rhs.mBits[byteSize]) & mask);
	}

	return *this;
}



BitArray operator^(const BitArray& a, const BitArray& b)
{
	if(a.Size() > b.Size())
	{
		BitArray result(a);
		result ^= b;
		return result;
	}

	BitArray result(b);
	result ^= a;
	return result;
}



BitArray BitArray::operator ~()
{
	BitArray arr(*this);
	for(size_t i = 0; i < mDataSize; i++)
		arr.mBits[i] = ~arr.mBits[i];
	return arr;
}



BitArray operator<<(const BitArray& a, size_t shift)
{
	BitArray ba(a);
	ba <<= shift;
	return ba;
}



BitArray& BitArray::operator<<=(size_t shift)
{
	size_t i = mSize;
	while(--i > shift)
		Set(i, Get(i - shift));
	while(i--)
		Reset(i);
	return *this;
}



BitArray operator>>(const BitArray& a, size_t shift)
{
	BitArray ba(a);
	ba <<= shift;
	return ba;
}



BitArray& BitArray::operator>>=(size_t shift)
{
	size_t i = 0;
	while(i < mSize - shift - 1)
		Set(i++, Get(i + shift));
	while(++i < mSize)
		Reset(i);
	return *this;
}



std::string BitArray::ToString(char zero /*= '0'*/, char one /*= '1'*/) const
{
	if(mSize == 0)
		return "";

	std::string result(mSize, zero);
	for(size_t i = 0; i < mSize; i++)
		if(Get(i))
			result[mSize - i - 1] = one;
	return result;
}



std::ostream& operator<<(std::ostream& os, const BitArray& ba)
{
	os << ba.ToString();
	return os;
}



std::istream& operator>>(std::istream& is, BitArray& ba)
{
	using traits = std::char_traits<char>;
	const char zero = '0';
	const char one = '1';
	std::istream::iostate state = std::istream::goodbit;
	bool changed = true;

	// construct string from is
	std::string str;
	int meta = is.rdbuf()->sgetc();
	do
	{
		char c;
		if(traits::eq_int_type(traits::eof(), meta))
		{
			// end of file
			state |= std::istream::eofbit;
			break;
		}
		else if((c = traits::to_char_type(meta)) != zero && c != one)
		{
			// invalid element
			break;
		}
		else if(str.size() > str.max_size())
		{
			// max string size reached
			state |= std::istream::failbit;
			break;
		}
		else
		{
			str.push_back('0' + (c == one));
			changed = true;
		}
	} while(meta = is.rdbuf()->snextc());

	if(!changed)
		state |= std::istream::failbit;

	// reconstruct from str
	ba = BitArray(str);

	// update istream state
	is.setstate(state);
	return is;
}
