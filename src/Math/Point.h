#ifndef __MATH_POINT_H__
#define __MATH_POINT_H__

namespace Math
{
	template<typename T>
	class Point
	{
	public:
		Point()
		{
			x = T();
			y = T();
		}

		Point(T X, T Y)
		{
			x = X;
			y = Y;
		}

		Point(const Point& oSrc)
		{
			x = oSrc.x;
			y = oSrc.y;
		}

		double			Length() { return sqrt(x*x + y * y); }
		double			LengthSquare() { return x * x + y * y; }

		T				x;
		T				y;
	};

	typedef Point<int>    PointI;
	typedef Point<float>  PointF;
	typedef Point<double> PointD;
}

#endif //__MATH_POINT_H__