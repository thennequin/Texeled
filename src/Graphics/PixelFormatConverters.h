#ifndef __GRAPHICS_PIXELFORMAT_CONVERTERS_H__
#define __GRAPHICS_PIXELFORMAT_CONVERTERS_H__

#include "PixelFormat.h"

namespace Graphics
{
	namespace PixelFormat
	{
		namespace Converters
		{
			void HalfToFloat(uint16_t iValue, float* pOutFloat);
			void FloatToHalf(float fValue, half* pOutHalf);

			void FloatToByte(float fValue, uint8_t* pOutByte);
			void ByteToFloat(uint8_t iValue, float* pOutFloat);

			void FloatToShort(float fValue, uint16_t* pOutShort);
			void ShortToFloat(uint16_t iValue, float* pOutFloat);

			void Convert_R8_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_R8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RG8_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_RG8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_BGR8_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_BGR8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_BGRA8_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_BGRA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB565_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_RGB565(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_BGR565_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_BGR565(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_RGBA16(RGBA8* pIn, RGBA16* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA16_To_RGBA8(RGBA16* pIn, RGBA8* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_R32F_To_RG32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RG32F_To_R32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RG32F_To_RGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB32F_To_RG32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB32F_To_RGBA32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA32F_To_RGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB16F_To_RGBA16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA16F_To_RGB16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA16_To_RGBA16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB16F_To_RGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB32F_To_RGB16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA16F_To_RGBA32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA32F_To_RGBA16F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA8_To_RGBA32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA32F_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB8_To_RGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGB32F_To_RGB8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void ConvertRGBA16ToRGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_R10G10B10A2_To_RGBA32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_RGBA32F_To_R10G10B10A2(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
			void Convert_R9G9B9E5_To_RGB32F(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut);
		}
	}
}

#endif // __GRAPHICS_PIXELFORMAT_CONVERTERS_H__