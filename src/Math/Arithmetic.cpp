#include "Math/Arithmetic.h"

namespace Math
{
	inline float Sqrt(float fValue)
	{
		union { int iX; float fX; };
		fX = fValue;
		float fXHalf = 0.5f * fX;
		iX = 0x5F37599E - (iX >> 1);
		fX = fX * (1.5f - fXHalf * fX * fX);
		fX = fX * (1.5f - fXHalf * fX * fX);
		return fX;
	}

	uint8_t HighBitFirst(uint64_t iNumber)
	{
		for (uint8_t iBit = 0; iBit < 64; ++iBit)
		{
			if (((iNumber >> iBit) & 1) != 0)
				return iBit + 1;
		}

		return 0;
	}

	uint8_t HighBitLast(uint64_t iNumber)
	{
		uint8_t iBit = 63;
		do
		{
			if (((iNumber >> iBit) & 1) != 0)
				return iBit + 1;
		} while ((--iBit) > 0);

		return 0;
	}

	uint8_t HighBitCount(uint64_t iNumber)
	{
		uint8_t iCount = 0;
		for (uint8_t iBit = 0; iBit < 64; ++iBit)
		{
			if (((iNumber >> iBit) & 1) != 0)
			{
				iCount++;
			}
		}

		return iCount;
	}

	uint64_t HighBitFill(uint8_t iBitCount, uint8_t iFirstBit)
	{
		if (iBitCount == 0 || iFirstBit > 63)
			return 0;

		// Clamp to last poossible bit
		if (iBitCount > 64)
			iBitCount = 64;

		uint8_t iLastBit = ((iFirstBit + iBitCount - 1) <= 63 ? (iFirstBit + iBitCount - 1) : 63);

		uint64_t iValue = 0;
		for (uint8_t iBit = iFirstBit; iBit <= iLastBit; ++iBit)
		{
			iValue |= ((uint64_t)1) << iBit;
		}

		return iValue;
	}
}
