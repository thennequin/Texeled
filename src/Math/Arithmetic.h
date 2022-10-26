#ifndef __MATH_ARITHMETIC_H__
#define __MATH_ARITHMETIC_H__

#include <stdint.h> // uint8_t / uint64_t

namespace Math
{
	template<typename T>
	inline T Abs(const T a) { return a < 0 ? -a : a; }

	template<typename T>
	inline T Min(const T a, const T b) { return a < b ? a : b; }

	template<typename T>
	inline T Max(const T a, const T b) { return a > b ? a : b; }

	template<typename T>
	inline T Clamp(T oValue, T oMin, T oMax)
	{
		return ((oValue > oMax) ? oMax : ((oValue < oMin) ? oMin : oValue));
	}

	template<typename T>
	inline T Lerp(T oMin, T oMax, float fLerp)
	{
		return (oMin * (1.f - fLerp)) + oMax * fLerp;
	}

	inline float Sqrt(float fValue);

	// Return first high bit
	// Return 0 if no high bit;
	// Example : HighBitFirst of 26 (11010) => 2
	//                               ...|.
	uint8_t HighBitFirst(uint64_t iNumber);

	// Return last high bit
	// Return 0 if no high bit;
	// Example : HighBitLast of 26 (11010) => 5
	//                              |....
	uint8_t HighBitLast(uint64_t iNumber);

	// Extract high bit count in unsigned number
	// Example : HighBitCount of 26 (11010) => 3
	//                               ||.|.
	uint8_t HighBitCount(uint64_t iNumber);

	// Fill uint64_t
	// Example : HighBitFill(5,3) = 11111000
	//                              |...|
	//                              5   3
	uint64_t HighBitFill(uint8_t iBitCount, uint8_t iFirstBit);

	// Return previous high bit
	// if iCurrentBit == 0, return last bit
	// Return 0 if no previous bit
	// Example : HighBitPrevious(0, 10110) => 5
	//                              |....
	// Example : HighBitPrevious(5, 10110) => 3
	//                              ^.|..
	uint8_t HighBitPrevious(uint8_t iCurrentBit, uint64_t iBits);

	// Return next high bit
	// if iCurrentBit == 0, return first bit
	// Return 0 if no next bit
	// Example : HighBitNext of 0 and 11010 => 2
	//                                ...|.
	// Example : HighBitNext of 2 and 11010 => 4
	//                                .|.^.
	uint8_t HighBitNext(uint8_t iCurrentBit, uint64_t iBits);

	// Replace part of iDestValue by a part of iSourceValue
	// Return replaced value
	// Example :
	//           iBitCount    = 4
	//           iDestShift   = 2
	//           iDestValue   = 1111 1011
	//                            ^^ ^^
	//           iSourceShift = 1
	//           iSourceValue = 0000 0011
	//                             ^ ^^^
	//           Return       = 1100 0111
	//                            ^^ ^^
	uint64_t ReplaceBits(uint64_t iDestValue, uint64_t iSourceValue, uint8_t iBitCount, uint8_t iDestBit, uint8_t iSourceBit);
}

#endif //__MATH_ARITHMETIC_H__