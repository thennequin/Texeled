#ifndef __MATH_VERTEX_H__
#define __MATH_VERTEX_H__

namespace Math
{
	template<typename T>
	class Vertex
	{
	public:
		Vertex()
		{
			x = T();
			y = T();
			z = T();
		}

		Vertex(T X, T Y, T Z)
		{
			x = X;
			y = Y;
			z = Z;
		}

		Vertex(const Vertex& oSrc)
		{
			x = oSrc.x;
			y = oSrc.y;
			z = oSrc.z;
		}

		double			Length() { return sqrt(x*x + y * y + z * z); }
		double			LengthSquare() { return x * x + y * y + z * z; }

		T				x;
		T				y;
		T				z;
	};

	typedef Vertex<int>    VertexI;
	typedef Vertex<float>  VertexF;
	typedef Vertex<double> VertexD;
}

#endif //__MATH_VERTEX_H__