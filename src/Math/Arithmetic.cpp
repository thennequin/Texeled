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
}
