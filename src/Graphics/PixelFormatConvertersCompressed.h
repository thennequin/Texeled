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
			void Convert_BC3_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
		}
	}
}

#endif // __GRAPHICS_PIXELFORMAT_CONVERTERS_COMPRESSED_H__