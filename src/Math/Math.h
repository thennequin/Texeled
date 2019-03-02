#ifndef __MATH_H__
#define __MATH_H__

namespace Math
{
	template<typename T>
	inline T Min(const T a, const T b) { return a < b ? a : b; }

	template<typename T>
	inline T Max(const T a, const T b) { return a > b ? a : b; }

	template<typename T>
	inline T Clamp(T oValue, T oMin, T oMax)
	{
		return ((oValue > oMax) ? oMax : ((oValue < oMin) ? oMin : oValue));
	}
}

#endif //__MATH_H__