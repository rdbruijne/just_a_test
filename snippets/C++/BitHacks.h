// https://graphics.stanford.edu/~seander/bithacks.html
// https://www.youtube.com/watch?v=ZRNO-ewsNcQ
#pragma once

#include <stdint.h>

// check
inline uint32_t CheckBit(uint32_t x, uint32_t ix)
{
	return x & (uint32_t{1} << ix);
}

// toggle bits
inline uint32_t SetBit(uint32_t x, uint32_t ix)
{
	return x | (uint32_t{1} << ix);
}

inline uint32_t ClearBit(uint32_t x, uint32_t ix)
{
	return x & ~(uint32_t{1} << ix);
}

inline uint32_t ToggleBit(uint32_t x, uint32_t ix)
{
	return x ^ (uint32_t{1} << ix);
}

inline uint32_t ConvertTrailingZeroToOne(uint32_t x)
{
	return (x - 1) | x;
}

inline uint32_t ClearLowestSetBit(uint32_t x)
{
	return x & (x - 1);
}

inline uint32_t SetLowestZeroBit(uint32_t x)
{
	return x | (x + 1);
}

inline uint32_t SetAllBitRightOfLowestSetBit(uint32_t x)
{
	return x | (x - 1);
}

inline uint32_t SetAllBitRightOfLowestZeroBit(uint32_t x)
{
	return x & (x + 1);
}

// least significant bit
inline uint32_t lsb(uint32_t x)
{
	return x & -x;
}

inline uint32_t lowestZeroBitAsOne(uint32_t x)
{
	return ~x & (x + 1);
}

// copying
inline uint32_t MaskedCopy(uint32_t src, uint32_t dst, uint32_t mask)
{
	return (src & mask) | (dst & ~mask);
}

inline uint32_t SwapBits(uint32_t x, uint32_t a, uint32_t b)
{
	const uint32_t p = (x >> a) ^ (x >> b) & 1;
	x ^= (p << a);
	x ^= (p << b);
	return x;
}

// population count
inline uint32_t popcnt(uint32_t x)
{
	uint32_t c = 0;
	while(x != 0)
	{
		x = x & (x - 1);
		c++;
	}
	return c;
}

inline uint32_t CountBitIsands(uint32_t x)
{
	
	return (x & 1) + (popcnt(x ^ (x >> 1)) >> 1);
}

// bit scan forward
inline uint32_t bsf(uint32_t x)
{
	if(x == 0) return -1;
	x = x & -x;
	uint32_t count = 0;
	if((x & 0xFFFF0000) != 0) count += 16;
	if((x & 0xFF00FF00) != 0) count += 8;
	if((x & 0xF0F0F0F0) != 0) count += 4;
	if((x & 0xCCCCCCCC) != 0) count += 2;
	if((x & 0xAAAAAAAA) != 0) count += 1;
	return count;
}


inline uint32_t NextPerm(uint32_t x)
{
	const uint32_t t = x | (x - 1);
	return (t + 1) | ((~t & -(~t)) - 1) >> (bsf(x) + 1);
}

// bit mask creation
inline uint32_t BitMask(uint32_t size)
{
	return (uint32_t{1} << size) - 1;
}

inline uint32_t MaskForAllBitsLowerThanLowestSetBit(uint32_t x)
{
	return ~x | (x - 1);
}

inline uint32_t MaskForAllBitsLowerThanLowestZeroBit(uint32_t x)
{
	return x | ~(x + 1);
}

inline uint32_t MaskForBitsLeftOfLowestOneBit_Inclusive(uint32_t x)
{
	return x | -x;
}

inline uint32_t MaskForBitsLeftOfLowestOneBit_Exclusive(uint32_t x)
{
	return x ^ -x;
}

inline uint32_t MaskForBitsLeftOfLowestZeroBit_Inclusive(uint32_t x)
{
	return ~x | (x + 1);
}

inline uint32_t MaskForBitsLeftOfLowestZeroBit_Exclusive(uint32_t x)
{
	return ~x ^ (x + 1);
}

inline uint32_t MaskForBitsRightOfLowestOneBit_Inclusive(uint32_t x)
{
	return x ^ (x - 1);
}

inline uint32_t MaskForBitsRightOfLowestOneBit_Exclusive(uint32_t x)
{
	return ~x & (x - 1);
}

inline uint32_t MaskForBitsRightOfLowestZeroBit_Inclusive(uint32_t x)
{
	return x ^ (x + 1);
}

inline uint32_t MaskForBitsRightOfLowestZeroBit_Exclusive(uint32_t x)
{
	return x & (x + 1);
}

// (de)construct: floating point <--> signBit, exponent, mantissa
#if false
// 16-bit floating point [1, 5, 10]
inline half Construct(uint16_t signBit, uint16_t exp, uint16_t mantissa)
{
	union { uint16_t u; half f16; };
	u = ((signBit & 1) << 15) | ((exp & 0x001F) << 10) | (mantissa & 0x03FF);
	return f16;
}

inline void Deconstruct(half f, uint16_t& signBit, uint16_t& exp, uint16_t& mantissa)
{
	union { uint16_t u; half f16; };
	f16 = f;

	signBit = u >> 15;
	exp = (u >> 10) & 0x001F;
	mantissa = u & 0x03FF;
}
#endif

// 32-bit floating point [1, 8, 23]
inline float Construct(uint32_t signBit, uint32_t exp, uint32_t mantissa)
{
	union { uint32_t u; float f32; };
	u = ((signBit & 1) << 31) | ((exp & 0x00FF) << 23) | (mantissa & 0x007F'FFFF);
	return f32;
}

inline void Deconstruct(float f, uint32_t& signBit, uint32_t& exp, uint32_t& mantissa)
{
	union { uint32_t u; float f32; };
	f32 = f;

	signBit = u >> 31;
	exp = (u >> 23) & 0xFF;
	mantissa = u & 0x7FFFFF;
}

// 64-bit floating point [1, 11, 52]
inline double Construct(uint64_t signBit, uint64_t exp, uint64_t mantissa)
{
	union { uint32_t u; double f64; };
	u = ((signBit & 1) << 63) | ((exp & 0x07FF) << 52) | (mantissa & 0x000F'FFFF'FFFF'FFFF);
	return f64;
}

inline void Deconstruct(double f, uint64_t& signBit, uint64_t& exp, uint64_t& mantissa)
{
	union { uint32_t u; double f64; };
	f64 = f;

	signBit = u >> 63;
	exp = (u >> 52) & 0x07FF;
	mantissa = u & 0x000F'FFFF'FFFF'FFFF;
}
