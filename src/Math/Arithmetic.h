#ifndef __MATH_ARITHMETIC_H__
#define __MATH_ARITHMETIC_H__

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
}

#endif //__MATH_ARITHMETIC_H__