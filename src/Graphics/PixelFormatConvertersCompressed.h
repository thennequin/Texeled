#ifndef __GRAPHICS_PIXELFORMAT_CONVERTERS_COMPRESSED_H__
#define __GRAPHICS_PIXELFORMAT_CONVERTERS_COMPRESSED_H__

#include "PixelFormat.h"

namespace Graphics
{
	namespace PixelFormat
	{
		namespace Converters
		{
			void Convert_BC1_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_BC1(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC2_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_BC2(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC3_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_BC3(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC4_To_R8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_R8_To_BC4(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC5_To_RG8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RG8_To_BC5(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC6H_To_RGB16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB16F_To_BC6H(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);

			void Convert_BC7_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_BC7(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
		}
	}
}

#endif // __GRAPHICS_PIXELFORMAT_CONVERTERS_COMPRESSED_H__