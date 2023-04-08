#pragma once

// C++
#include <assert.h>
#include <string>

class BitArray
{
public:
	class Reference
	{
		friend class BitArray;

		Reference() = delete;

		// Constructs a reference to a bit in a BitArray.
		inline explicit Reference(uint8_t& byte, uint8_t mask);

		~Reference() = default;

	public:
		// Constructs a copy from another BitArray Reference.
		inline Reference(const Reference& rhs);

		// Assigns a copy from another BitArray Reference.
		inline Reference& operator =(const Reference& rhs);

		// Assigns a vale to the referenced bit.
		inline Reference& operator =(bool value);

		// Returns the referenced bit.
		inline operator bool() const;

		// Returns inverted reference bit.
		inline bool operator ~();

		// Flips the referenced bit.
		inline Reference& Flip();

	private:
		uint8_t& mByte;
		uint8_t mMask;
	};

	// Constructs an empty BitArray.
	BitArray() = default;

	// Constructs a BitArray with all bits set to zero.
	explicit BitArray(size_t size);

	// Constructs a BitArray with all bits set to one if defaultSet = true, 0 otherwise.
	explicit BitArray(size_t size, bool defaultSet);

	// Construct a BitArray from an initializer list.
	explicit BitArray(const std::initializer_list<bool>& list);

	// Construct a BitArray using the characters in str.
	// An optional starting position, number of characters to use and alternate values for zero and one can be provided.
	explicit BitArray(const std::string& str, size_t pos = 0, size_t n = std::string::npos, char zero = '0', char one = '1');

	// Constructs a copy from another BitArray.
	BitArray(const BitArray& rhs);

	// Constructs a move from another BitArray.
	BitArray(BitArray&& rhs) noexcept;

	// Destructor.
	~BitArray();

	// Assigns a copy from another BitArray.
	BitArray& operator = (const BitArray& rhs);

	// Assigns a move from another BitArray.
	BitArray& operator = (BitArray&& rhs) noexcept;

	// Compare
	inline bool operator ==(const BitArray& rhs) const;
	inline bool operator !=(const BitArray& rhs) const;

	// Returns the number of bits this BitArray holds.
	inline size_t Size() const;

	// Returns the size of the underlying data.
	inline size_t DataSize() const;

	// Access a specific bit.
	inline bool operator [] (size_t index) const;
	inline Reference operator [] (size_t index);

	// Accesses specific bit.
	inline bool Test(size_t index) const;

	// Checks if all of the bits are set to true.
	bool All() const;

	// Checks if any of the bits is set to true.
	bool Any() const;

	// Checks if none of the bits are set to true.
	bool None() const;

	// Returns the number of bits set to true.
	size_t Count() const;

	// Get the value of a bit.
	inline bool Get(size_t index) const;

	// Sets all the bit to true.
	inline void Set();

	// Set a bit to true.
	inline void Set(size_t index);

	// Set a bit to a specific value.
	inline void Set(size_t index, bool value);

	// Sets all the bit to false.
	inline void Reset();

	// Set a bit to false.
	inline void Reset(size_t index);

	// Toggles the value of a bit.
	inline void Flip(size_t index);

	// Performs binary AND.
	BitArray& operator &=(const BitArray& rhs);
	friend BitArray operator &(const BitArray& a, const BitArray& b);

	// Performs binary OR.
	BitArray& operator |=(const BitArray& rhs);
	friend BitArray operator |(const BitArray& a, const BitArray& b);

	// Performs binary XOR.
	BitArray& operator ^=(const BitArray& rhs);
	friend BitArray operator ^(const BitArray& a, const BitArray& b);

	// Performs binary NOT.
	BitArray operator ~();

	// Perform binary left shift.
	BitArray& operator <<=(size_t shift);
	friend BitArray operator <<(const BitArray& a, size_t shift);

	// Perform binary right shift.
	BitArray& operator >>=(size_t shift);
	friend BitArray operator >>(const BitArray& a, size_t shift);

	// Returns a string representation of the data.
	// Alternate values for zero and one can be provided.
	std::string ToString(char zero = '0', char one = '1') const;

	// Performs output to stream.
	friend std::ostream& operator <<(std::ostream& os, const BitArray& ba);

	// Performs input from stream.
	friend std::istream& operator >>(std::istream& is, BitArray& ba);

private:
	// Number of bits.
	size_t mSize = 0;
	// Number of bytes used to store the bits.
	size_t mDataSize = 0;
	// The bits.
	uint8_t* mBits = nullptr;
};

#include "BitArray.inl"
