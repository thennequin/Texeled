#include "Graphics/PixelFormat.h"

#include "Graphics/Texture.h"

#include "Math/Math.h"

namespace Graphics
{
	extern const char* const EComponentEncoding_string[_E_COMPONENT_ENCODING_COUNT] = {
		"None",

		"UNorm",
		"Int",
		"UInt",
		"Float",
		"SNorm"
	};

	const char* const EPixelFormat_string[_E_PIXELFORMAT_COUNT] = {
			"None",

			"R8 UNorm",
			"RG8 UNorm",

			"RGB8 UNorm",
			"BGR8 UNorm",

			"RGBA8 UNorm",
			"BGRA8 UNorm",

			"R5G6B5 UNorm",
			"B5G6R5 UNorm",

			"RGB16 SHalf",

			"RGBA16 UNorm",
			"RGBA16 Half",

			"RGB32 Float",
			"RGBA32 Float",

			"R10G10B10A2 UNorm",

			"R9G9B9E5 Float (shared exp)",

			"BC1 (DXT1)",
			"BC2 (DXT2/3)",
			"BC3 (DXT4/5)",
			"BC4",
			"BC5",
			"BC6H",
			"BC7"
	};

	const PixelFormatInfos EPixelFormatInfos[_E_PIXELFORMAT_COUNT] =
	{
		//  /--------------------------------------------------------- iBitsPerPixel
		//  |    /---------------------------------------------------- iBlockWidth
		//  |    |    /----------------------------------------------- iBlockHeight
		//  |    |    |    /------------------------------------------ iBlockSize
		//  |    |    |    |    /------------------------------------- iComponents
		//  |    |    |    |    |    /-------------------------------- eEncoding
		//  |    |    |    |    |    |
		//  |    |    |    |    |    |
		{   0,   0,   0,   0,   0,   E_COMPONENT_ENCODING_NONE  }, // E_PIXELFORMAT_NONE

		{   8,   1,   1,   1,   1,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_R8_UNORM

		{  16,   1,   1,   2,   2,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_RG8_UNORM

		{  24,   1,   1,   3,   3,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_RGB8_UNORM
		{  24,   1,   1,   3,   3,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BGR8_UNORM

		{  32,   1,   1,   4,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_RGBA8_UNORM
		{  32,   1,   1,   4,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BGRA8_UNORM

		{  24,   1,   1,   2,   3,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_R5G6B5_UNORM
		{  24,   1,   1,   2,   3,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_B5G6BR_UNORM

		{  48,   1,   1,   6,   3,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_RGB16_FLOAT

		{  64,   1,   1,   6,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_RGBA16_UNORM
		{  64,   1,   1,   8,   4,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_RGBA16_FLOAT

		{  96,   1,   1,  12,   3,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_RGB32_FLOAT

		{ 128,   1,   1,  16,   4,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_RGBA32_FLOAT

		{  32,   1,   1,   4,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_R10G10B10A2_UNORM

		{  32,   1,   1,   4,   3,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_R9G9B9E5_SHAREDEXP

		{   4,   4,   4,   8,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BC1
		{   8,   4,   4,  16,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BC2
		{   8,   4,   4,  16,   4,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BC3
		{   4,   4,   4,   8,   1,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BC4
		{   8,   4,   4,  16,   2,   E_COMPONENT_ENCODING_UNORM }, // E_PIXELFORMAT_BC5
		{   8,   4,   4,  16,   3,   E_COMPONENT_ENCODING_FLOAT }, // E_PIXELFORMAT_BC6H
		{   8,   4,   4,  16,   3,   E_COMPONENT_ENCODING_UNORM }  // E_PIXELFORMAT_BC7
	};

	namespace PixelFormat
	{
		bool IsCompressed(EPixelFormat ePixelFormat)
		{
			return (ePixelFormat >= E_PIXELFORMAT_BC1 && ePixelFormat <= E_PIXELFORMAT_BC7);
		}

		int BitPerPixel(EPixelFormat ePixelFormat)
		{
			return EPixelFormatInfos[ePixelFormat].iBitsPerPixel;
		}

		//Return Bytes
		int BlockSize(EPixelFormat ePixelFormat)
		{
			return EPixelFormatInfos[ePixelFormat].iBlockSize;
		}

		int ComponentCount(EPixelFormat ePixelFormat)
		{
			return EPixelFormatInfos[ePixelFormat].iComponents;
		}

		void GetBlockCount(EPixelFormat ePixelFormat, uint32_t iWidth, uint32_t iHeight, uint32_t* pOutCountX, uint32_t* pOutCountY)
		{
			if (pOutCountX != NULL)
			{
				const PixelFormatInfos& oInfos = EPixelFormatInfos[ePixelFormat];
				uint32_t iBlockCount = ((iWidth + (oInfos.iBlockWidth - 1)) / oInfos.iBlockWidth);
				if (iBlockCount == 0)
					iBlockCount = 1;
				*pOutCountX = iBlockCount;
			}

			if (pOutCountY != NULL)
			{
				const PixelFormatInfos& oInfos = EPixelFormatInfos[ePixelFormat];
				uint32_t iBlockCount = ((iHeight + (oInfos.iBlockHeight - 1)) / oInfos.iBlockHeight);
				if (iBlockCount == 0)
					iBlockCount = 1;
				*pOutCountY = iBlockCount;
			}
		}

		uint32_t GetPitch(EPixelFormat ePixelFormat, uint32_t iWidth)
		{
			const PixelFormatInfos& oInfos = EPixelFormatInfos[ePixelFormat];
			uint32_t iBlockCount = ((iWidth + (oInfos.iBlockWidth - 1)) / oInfos.iBlockWidth);
			if (iBlockCount == 0)
				iBlockCount = 1;
			return iBlockCount * oInfos.iBlockSize;
		}

		void HalfToFloat(uint16_t iValue, float* pOutFloat)
		{
			uint32_t iMantissa = (uint32_t)(iValue & 0x03FF);

			uint32_t iExponent = (iValue & 0x7C00);
			if (iExponent == 0x7C00) // INF/NAN
			{
				iExponent = (uint32_t)0x8f;
			}
			else if (iExponent != 0)  // The value is normalized
			{
				iExponent = (uint32_t)((iValue >> 10) & 0x1F);
			}
			else if (iMantissa != 0)     // The value is denormalized
			{
				// Normalize the value in the resulting float
				iExponent = 1;

				do
				{
					iExponent--;
					iMantissa <<= 1;
				} while ((iMantissa & 0x0400) == 0);

				iMantissa &= 0x03FF;
			}
			else                        // The value is zero
			{
				iExponent = (uint32_t)-112;
			}

			uint32_t iResult = ((iValue & 0x8000) << 16) | // Sign
				((iExponent + 112) << 23) | // Exponent
				(iMantissa << 13);          // Mantissa

			*pOutFloat = reinterpret_cast<float*>(&iResult)[0];
		}

		void FloatToHalf(float fValue, half* pOutHalf)
		{
			uint32_t iResult;

			uint32_t iValue = reinterpret_cast<uint32_t *>(&fValue)[0];
			uint32_t Sign = (iValue & 0x80000000U) >> 16U;
			iValue = iValue & 0x7FFFFFFFU;      // Hack off the sign

			if (iValue > 0x477FE000U)
			{
				// The number is too large to be represented as a half.  Saturate to infinity.
				if (((iValue & 0x7F800000) == 0x7F800000) && ((iValue & 0x7FFFFF) != 0))
				{
					iResult = 0x7FFF; // NAN
				}
				else
				{
					iResult = 0x7C00U; // INF
				}
			}
			else
			{
				if (iValue < 0x38800000U)
				{
					// The number is too small to be represented as a normalized half.
					// Convert it to a denormalized value.
					uint32_t iShift = 113U - (iValue >> 23U);
					iValue = (0x800000U | (iValue & 0x7FFFFFU)) >> iShift;
				}
				else
				{
					// Rebias the exponent to represent the value as a normalized half.
					iValue += 0xC8000000U;
				}

				iResult = ((iValue + 0x0FFFU + ((iValue >> 13U) & 1U)) >> 13U) & 0x7FFFU;
			}
			pOutHalf->u = (uint16_t)(iResult | Sign);
		}

		void FloatToByte(float fValue, uint8_t* pOutByte)
		{
			if (fValue > 1.f) fValue = 1.f;
			if (fValue < 0.f) fValue = 0.f;
			*pOutByte = (unsigned char)(fValue * 255);
		}

		void ByteToFloat(uint8_t iValue, float* pOutFloat)
		{
			*pOutFloat = iValue / 255.f;
		}

		void FloatToShort(float fValue, uint16_t* pOutShort)
		{
			if (fValue > 1.f) fValue = 1.f;
			if (fValue < 0.f) fValue = 0.f;
			*pOutShort = (unsigned char)(fValue * 65535);
		}

		void ShortToFloat(uint16_t iValue, float* pOutFloat)
		{
			*pOutFloat = iValue / 65535.f;
		}

		void Convert_R8_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			uint8_t* pIn8 = (uint8_t*)pIn;
			RGB8* pOut888 = (RGB8*)pOut;
			pOut888->r = *pIn8;
			pOut888->g = 0;
			pOut888->b = 0;
		}

		void Convert_RGB8_To_R8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn888 = (RGB8*)pIn;
			uint8_t* pOut8 = (uint8_t*)pOut;
			*pOut8 = pIn888->r;
		}

		void Convert_RG8_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			uint8_t* pIn8 = (uint8_t*)pIn;
			RGB8* pOut888 = (RGB8*)pOut;
			pOut888->r = pIn8[0];
			pOut888->g = pIn8[1];
			pOut888->b = 0;
		}

		void Convert_RGB8_To_RG8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn888 = (RGB8*)pOut;
			uint8_t* pOut88 = (uint8_t*)pIn;
			pOut88[0]= pIn888->r;
			pOut88[1] = pIn888->g;
		}

		void Convert_RGB8_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn8 = (RGB8*)pIn;
			RGBA8* pOut888 = (RGBA8*)pOut;
			pOut888->r = pIn8->r;
			pOut888->g = pIn8->g;
			pOut888->b = pIn8->b;
			pOut888->a = 255;
		}

		void Convert_RGBA8_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA8* pIn8888 = (RGBA8*)pIn;
			RGB8* pOut888 = (RGB8*)pOut;
			pOut888->r = pIn8888->r;
			pOut888->g = pIn8888->g;
			pOut888->b = pIn8888->b;
		}

		void Convert_BGR8_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			BGR8* pIn8 = (BGR8*)pIn;
			RGB8* pOut8 = (RGB8*)pOut;
			pOut8->r = pIn8->r;
			pOut8->g = pIn8->g;
			pOut8->b = pIn8->b;
		}

		void Convert_RGB8_To_BGR8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn8 = (RGB8*)pIn;
			BGR8* pOut8 = (BGR8*)pOut;
			pOut8->r = pIn8->r;
			pOut8->g = pIn8->g;
			pOut8->b = pIn8->b;
		}

		void Convert_BGRA8_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			BGRA8* pIn8 = (BGRA8*)pIn;
			RGBA8* pOut8 = (RGBA8*)pOut;
			pOut8->r = pIn8->r;
			pOut8->g = pIn8->g;
			pOut8->b = pIn8->b;
			pOut8->a = pIn8->a;
		}

		void Convert_RGBA8_To_BGRA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA8* pIn8 = (RGBA8*)pIn;
			BGRA8* pOut8 = (BGRA8*)pOut;
			pOut8->r = pIn8->r;
			pOut8->g = pIn8->g;
			pOut8->b = pIn8->b;
			pOut8->a = pIn8->a;
		}

		void Convert_RGB565_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			R5G6B5* pIn565 = (R5G6B5*)pIn;
			RGB8* pOut888 = (RGB8*)pOut;
			pOut888->b = (uint8_t)((pIn565->v.b << 3) | (pIn565->v.b >> 2));
			pOut888->g = (uint8_t)((pIn565->v.g << 2) | (pIn565->v.g >> 4));
			pOut888->r = (uint8_t)((pIn565->v.r << 3) | (pIn565->v.r >> 2));
		}

		void Convert_RGB8_To_RGB565(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn888 = (RGB8*)pIn;
			R5G6B5* pOut565 = (R5G6B5*)pOut;
			pOut565->v.r = (pIn888->r >> 3) & 0x1F;
			pOut565->v.g = (pIn888->g >> 2) & 0x3F;
			pOut565->v.b = (pIn888->b >> 3) & 0x1F;
		}

		void Convert_BGR565_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			B5G6R5* pIn565 = (B5G6R5*)pIn;
			RGB8* pOut888 = (RGB8*)pOut;
			pOut888->b = (uint8_t)((pIn565->v.b << 3) | (pIn565->v.b >> 2));
			pOut888->g = (uint8_t)((pIn565->v.g << 2) | (pIn565->v.g >> 4));
			pOut888->r = (uint8_t)((pIn565->v.r << 3) | (pIn565->v.r >> 2));
		}

		void Convert_RGB8_To_BGR565(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pIn888 = (RGB8*)pIn;
			B5G6R5* pOut565 = (B5G6R5*)pOut;
			pOut565->v.r = (pIn888->r >> 3) & 0x1F;
			pOut565->v.g = (pIn888->g >> 2) & 0x3F;
			pOut565->v.b = (pIn888->b >> 3) & 0x1F;
		}

		void Convert_RGB32F_To_RGBA32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB32F* pInRGB = (RGB32F*)pIn;
			RGBA32F* pOutRGBA = (RGBA32F*)pOut;
			pOutRGBA->r = pInRGB->r;
			pOutRGBA->g = pInRGB->g;
			pOutRGBA->b = pInRGB->b;
			pOutRGBA->a = 1.f;
		}

		void Convert_RGBA32F_To_RGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA32F* pInRGBA = (RGBA32F*)pIn;
			RGB32F* pOutRGB = (RGB32F*)pOut;
			pOutRGB->r = pInRGBA->r;
			pOutRGB->g = pInRGBA->g;
			pOutRGB->b = pInRGBA->b;
		}

		void Convert_RGB16F_To_RGBA16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB16F* pInRGB = (RGB16F*)pIn;
			RGBA16F* pOutRGBA = (RGBA16F*)pOut;
			pOutRGBA->r = pInRGB->r;
			pOutRGBA->g = pInRGB->g;
			pOutRGBA->b = pInRGB->b;
			FloatToHalf(1.f, &pOutRGBA->a);
		}

		void Convert_RGBA16F_To_RGB16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA16F* pInRGBA = (RGBA16F*)pIn;
			RGB16F* pOutRGB = (RGB16F*)pOut;
			pOutRGB->r = pInRGBA->r;
			pOutRGB->g = pInRGBA->g;
			pOutRGB->b = pInRGBA->b;
		}

		void Convert_RGBA16_To_RGBA16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA16* pInRGB = (RGBA16*)pIn;
			RGBA16F* pOutRGBA = (RGBA16F*)pOut;
			FloatToHalf(pInRGB->r / 65535.f, &pOutRGBA->r);
			FloatToHalf(pInRGB->g / 65535.f, &pOutRGBA->g);
			FloatToHalf(pInRGB->b / 65535.f, &pOutRGBA->b);
			FloatToHalf(pInRGB->a / 65535.f, &pOutRGBA->a);
		}

		void Convert_RGB16F_To_RGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB16F* pInRGB = (RGB16F*)pIn;
			RGB32F* pOutRGBA = (RGB32F*)pOut;
			HalfToFloat(pInRGB->r.u, &pOutRGBA->r);
			HalfToFloat(pInRGB->g.u, &pOutRGBA->g);
			HalfToFloat(pInRGB->b.u, &pOutRGBA->b);
		}

		void Convert_RGB32F_To_RGB16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB32F* pInRGB = (RGB32F*)pIn;
			RGB16F* pOutRGBA = (RGB16F*)pOut;
			FloatToHalf(pInRGB->r, &pOutRGBA->r);
			FloatToHalf(pInRGB->g, &pOutRGBA->g);
			FloatToHalf(pInRGB->b, &pOutRGBA->b);
		}

		void Convert_RGBA16F_To_RGBA32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA16F* pInRGB = (RGBA16F*)pIn;
			RGBA32F* pOutRGBA = (RGBA32F*)pOut;
			HalfToFloat(pInRGB->r.u, &pOutRGBA->r);
			HalfToFloat(pInRGB->g.u, &pOutRGBA->g);
			HalfToFloat(pInRGB->b.u, &pOutRGBA->b);
			HalfToFloat(pInRGB->a.u, &pOutRGBA->a);
		}

		void Convert_RGBA32F_To_RGBA16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA32F* pInRGB = (RGBA32F*)pIn;
			RGBA16F* pOutRGBA = (RGBA16F*)pOut;
			FloatToHalf(pInRGB->r, &pOutRGBA->r);
			FloatToHalf(pInRGB->g, &pOutRGBA->g);
			FloatToHalf(pInRGB->b, &pOutRGBA->b);
			FloatToHalf(pInRGB->a, &pOutRGBA->a);
		}

		void Convert_RGBA8_To_RGBA32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA8* pInRGBAA8 = (RGBA8*)pIn;
			RGBA32F* pOutRGBA32F = (RGBA32F*)pOut;
			ByteToFloat(pInRGBAA8->r, &pOutRGBA32F->r);
			ByteToFloat(pInRGBAA8->g, &pOutRGBA32F->g);
			ByteToFloat(pInRGBAA8->b, &pOutRGBA32F->b);
			ByteToFloat(pInRGBAA8->a, &pOutRGBA32F->a);
		}

		void Convert_RGBA32F_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA32F* pInRGBA32F = (RGBA32F*)pIn;
			RGBA8* pOutRGBA8 = (RGBA8*)pOut;
			FloatToByte(pInRGBA32F->r, &pOutRGBA8->r);
			FloatToByte(pInRGBA32F->g, &pOutRGBA8->g);
			FloatToByte(pInRGBA32F->b, &pOutRGBA8->b);
			FloatToByte(pInRGBA32F->a, &pOutRGBA8->a);
		}

		void Convert_RGB8_To_RGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pInRGBAA8 = (RGB8*)pIn;
			RGB32F* pOutRGBA32F = (RGB32F*)pOut;
			ByteToFloat(pInRGBAA8->r, &pOutRGBA32F->r);
			ByteToFloat(pInRGBAA8->g, &pOutRGBA32F->g);
			ByteToFloat(pInRGBAA8->b, &pOutRGBA32F->b);
		}

		void Convert_RGB32F_To_RGB8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB32F* pInRGBA32F = (RGB32F*)pIn;
			RGB8* pOutRGBAA8 = (RGB8*)pOut;
			FloatToByte(pInRGBA32F->r, &pOutRGBAA8->r);
			FloatToByte(pInRGBA32F->g, &pOutRGBAA8->g);
			FloatToByte(pInRGBA32F->b, &pOutRGBAA8->b);
		}


		void ConvertRGBA16ToRGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGB8* pInRGBAA8 = (RGB8*)pIn;
			RGB32F* pOutRGBA32F = (RGB32F*)pOut;
			ByteToFloat(pInRGBAA8->r, &pOutRGBA32F->r);
			ByteToFloat(pInRGBAA8->g, &pOutRGBA32F->g);
			ByteToFloat(pInRGBAA8->b, &pOutRGBA32F->b);
		}

		void Convert_R10G10B10A2_To_RGBA32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			R10G10B10A2* pInR10G10B10A2 = (R10G10B10A2*)pIn;
			RGBA32F* pOutRGBA32F = (RGBA32F*)pOut;
			pOutRGBA32F->r = pInR10G10B10A2->r / 1023.f;
			pOutRGBA32F->g = pInR10G10B10A2->g / 1023.f;
			pOutRGBA32F->b = pInR10G10B10A2->b / 1023.f;
			pOutRGBA32F->a = pInR10G10B10A2->a / 3.f;
		}

		void Convert_RGBA32F_To_R10G10B10A2(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			RGBA32F* pInRGBA32F = (RGBA32F*)pIn;
			R10G10B10A2* pOutR10G10B10A2 = (R10G10B10A2*)pOut;
			pOutR10G10B10A2->r = (uint32_t)Math::Clamp((int)(pInRGBA32F->r * 1023.f), 0, 1023);
			pOutR10G10B10A2->g = (uint32_t)Math::Clamp((int)(pInRGBA32F->g * 1023.f), 0, 1023);
			pOutR10G10B10A2->b = (uint32_t)Math::Clamp((int)(pInRGBA32F->b * 1023.f), 0, 1023);
			pOutR10G10B10A2->a = (uint32_t)Math::Clamp((int)(pInRGBA32F->a * 3.f), 0, 3);
		}

		void Convert_R9G9B9E5_To_RGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
		{
			R9G9B9E5* pInR9G9B9E5 = (R9G9B9E5*)pIn;
			RGB32F* pOutRGB32F = (RGB32F*)pOut;

			union { float f; int32_t i; } fi;
			fi.i = 0x33800000 + (pInR9G9B9E5->e << 23);
			float fScale = fi.f;

			pOutRGB32F->r = fScale * float(pInR9G9B9E5->r);
			pOutRGB32F->g = fScale * float(pInR9G9B9E5->g);
			pOutRGB32F->b = fScale * float(pInR9G9B9E5->b);
		}

		inline void RGB8Lerp(const RGB8& iColorA, const RGB8& iColorB, RGB8* pOut, int iMulA, int iMulB, int iDiv, int iAdd)
		{
			pOut->r = (uint8_t)((iColorA.r * iMulA + iColorB.r * iMulB + iAdd) / iDiv);
			pOut->g = (uint8_t)((iColorA.g * iMulA + iColorB.g * iMulB + iAdd) / iDiv);
			pOut->b = (uint8_t)((iColorA.b * iMulA + iColorB.b * iMulB + iAdd) / iDiv);

		}
		inline void RGBA8Lerp(const RGBA8& iColorA, const RGBA8& iColorB, RGBA8* pOut, int iMulA, int iMulB, int iDiv, int iAdd)
		{
			pOut->r = (uint8_t)((iColorA.r * iMulA + iColorB.r * iMulB + iAdd) / iDiv);
			pOut->g = (uint8_t)((iColorA.g * iMulA + iColorB.g * iMulB + iAdd) / iDiv);
			pOut->b = (uint8_t)((iColorA.b * iMulA + iColorB.b * iMulB + iAdd) / iDiv);
		}

		void Convert_BC1_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut)
		{
			BlockBC1* pBlock = (BlockBC1*)pIn;
			RGBA8* pOut4x4RGBA = (RGBA8*)pOut;

			RGBA8 iColors[4];
			Convert_BGR565_To_RGB8(&pBlock->iColor[0], &iColors[0], 0, 0);
			Convert_BGR565_To_RGB8(&pBlock->iColor[1], &iColors[1], 0, 0);
			iColors[0].a = iColors[1].a = 255;

			int iComp = 3;

			if (pBlock->iColor[0].u > pBlock->iColor[1].u)
			{
				RGBA8Lerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
				RGBA8Lerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);

				iColors[2].a = iColors[3].a = 255;
			}
			else
			{
				iColors[2].r = (iColors[0].r + iColors[1].r) / 2;
				iColors[2].g = (iColors[0].g + iColors[1].g) / 2;
				iColors[2].b = (iColors[0].b + iColors[1].b) / 2;
				iColors[2].a = 255;

				//iColors[3] = iColors[2]; // More sense but not in specs
				iColors[3].r = iColors[3].g = iColors[3].b = 0;
				iColors[3].a = 0;

				iComp = 4;
			}

			for (int i = 0; i < 16; ++i)
			{
				int y = i / 4;
				int x = i - y * 4;

				int iColorIndex = (pBlock->iPixelIndex >> (i * 2)) & 0x3;

				RGBA8& oOutPixel = pOut4x4RGBA[y * iPitchOut + x];
				RGBA8& oInPixel = iColors[iColorIndex];

				oOutPixel = oInPixel;
			}

			//return iComp;
		}

		void Convert_RGBA8_To_BC1(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut)
		{
			const uint8_t iAlphaThreashold = 128; // 128 or 255?

			RGBA8* pIn4x4RGBA = (RGBA8*)pIn;
			BlockBC1* pOutBlock = (BlockBC1*)pOut;

			RGBA8 iColors[4];
			iColors[0].a = iColors[1].a = iColors[2].a = iColors[3].a = 0;
			RGBA8& iLowestColor = iColors[1];
			RGBA8& iHighestColor = iColors[0];
			bool bHasAlpha = false;

			memset(&iLowestColor, 255, sizeof(RGB8));
			memset(&iHighestColor, 0, sizeof(RGB8));

			for (int iY = 0; iY < 4; ++iY)
			{
				for (int iX = 0; iX < 4; ++iX)
				{
					RGBA8& oPixel = pIn4x4RGBA[iY * iPitchIn + iX];

					if (oPixel.a < iAlphaThreashold)
					{
						bHasAlpha = true;
						if (oPixel.a == 0)
						{
							continue;
						}
					}

					if (oPixel.r < iLowestColor.r)
						iLowestColor.r = oPixel.r;
					if (oPixel.g < iLowestColor.g)
						iLowestColor.g = oPixel.g;
					if (oPixel.b < iLowestColor.b)
						iLowestColor.b = oPixel.b;

					if (oPixel.r > iHighestColor.r)
						iHighestColor.r = oPixel.r;
					if (oPixel.g > iHighestColor.g)
						iHighestColor.g = oPixel.g;
					if (oPixel.b > iHighestColor.b)
						iHighestColor.b = oPixel.b;
				}
			}

			{
				RGB8 iMid;
				iMid.r = (iHighestColor.r - iLowestColor.r) >> 4;
				iMid.g = (iHighestColor.g - iLowestColor.g) >> 4;
				iMid.b = (iHighestColor.b - iLowestColor.b) >> 4;

				iLowestColor.r = (iLowestColor.r + iMid.r <= 255) ? iLowestColor.r + iMid.r : 255;
				iLowestColor.g = (iLowestColor.g + iMid.g <= 255) ? iLowestColor.g + iMid.g : 255;
				iLowestColor.b = (iLowestColor.b + iMid.b <= 255) ? iLowestColor.b + iMid.b : 255;

				iHighestColor.r = (iHighestColor.r >= iMid.r) ? iHighestColor.r - iMid.r : 0;
				iHighestColor.g = (iHighestColor.g >= iMid.g) ? iHighestColor.g - iMid.g : 0;
				iHighestColor.b = (iHighestColor.b >= iMid.b) ? iHighestColor.b - iMid.b : 0;
			}

			if (abs(iLowestColor.r - iHighestColor.r) <= 8 && abs(iLowestColor.g - iHighestColor.g) <= 4 && abs(iLowestColor.b - iHighestColor.b) <= 8)
			{
				if (iLowestColor.r >= (255 - 8))
					iLowestColor.r -= 8;
				else
					iHighestColor.r += 8;

				if (iLowestColor.g >= (255 - 4))
					iLowestColor.g -= 4;
				else
					iHighestColor.g += 4;

				if (iLowestColor.b >= (255 - 8))
					iLowestColor.b -= 8;
				else
					iHighestColor.b += 8;
			}

			pOutBlock->iPixelIndex = 0;

			if (bHasAlpha)
			{
				if (*(uint32_t*)&iLowestColor < *(uint32_t*)&iHighestColor)
				{
					Convert_RGB8_To_BGR565(&iLowestColor, &pOutBlock->iColor[0], 0, 0);
					Convert_RGB8_To_BGR565(&iHighestColor, &pOutBlock->iColor[1], 0, 0);
				}
				else
				{
					Convert_RGB8_To_BGR565(&iHighestColor, &pOutBlock->iColor[0], 0, 0);
					Convert_RGB8_To_BGR565(&iLowestColor, &pOutBlock->iColor[1], 0, 0);

					RGBA8 iTemp = iLowestColor;
					iLowestColor = iHighestColor;
					iHighestColor = iTemp;
				}

				RGB8Lerp(*(RGB8*)&iColors[0], *(RGB8*)&iColors[1], (RGB8*) &iColors[2], 1, 1, 2, 0);
				iColors[3].r = iColors[3].g = iColors[3].b = 0;
			}
			else
			{
				if (*(uint32_t*)&iLowestColor < *(uint32_t*)&iHighestColor)
				{
					Convert_RGB8_To_BGR565(&iHighestColor, &pOutBlock->iColor[0], 0, 0);
					Convert_RGB8_To_BGR565(&iLowestColor, &pOutBlock->iColor[1], 0, 0);
				}
				else
				{
					Convert_RGB8_To_BGR565(&iLowestColor, &pOutBlock->iColor[0], 0, 0);
					Convert_RGB8_To_BGR565(&iHighestColor, &pOutBlock->iColor[1], 0, 0);

					RGBA8 iTemp = iLowestColor;
					iLowestColor = iHighestColor;
					iHighestColor = iTemp;
				}

				RGB8Lerp(*(RGB8*)&iColors[0], *(RGB8*)&iColors[1], (RGB8*)&iColors[2], 2, 1, 3, 0);
				RGB8Lerp(*(RGB8*)&iColors[0], *(RGB8*)&iColors[1], (RGB8*)&iColors[3], 1, 2, 3, 0);
			}

			CORE_ASSERT(pOutBlock->iColor[0].u != pOutBlock->iColor[1].u);

			if (bHasAlpha)
			{
				for (int i = 0; i < 16; ++i)
				{
					int iY = i / 4;
					int iX = i - iY * 4;

					RGBA8& oPixel = pIn4x4RGBA[iY * iPitchIn + iX];

					if (oPixel.a < iAlphaThreashold)
					{
						pOutBlock->iPixelIndex |= 3 << (i << 1);
						continue;
					}

					int d0 = abs(iColors[0].r - oPixel.r) + abs(iColors[0].g - oPixel.g) + abs(iColors[0].b - oPixel.b);
					int d1 = abs(iColors[1].r - oPixel.r) + abs(iColors[1].g - oPixel.g) + abs(iColors[1].b - oPixel.b);
					int d2 = abs(iColors[2].r - oPixel.r) + abs(iColors[2].g - oPixel.g) + abs(iColors[2].b - oPixel.b);

					int b0 = d0 < d2;
					int b1 = d0 < d1;
					int b2 = d2 < d0;
					int b3 = d2 < d1;

					int x0 = b0 & b1;
					int x1 = b2 & b3;

					pOutBlock->iPixelIndex |= (x0 | (x1 << 1)) << (i << 1);
				}
			}
			else
			{
				for (int i = 0; i < 16; ++i)
				{
					int iY = i / 4;
					int iX = i - iY * 4;

					RGBA8& oPixel = pIn4x4RGBA[iY * iPitchIn + iX];

					int d0 = abs(iColors[0].r - oPixel.r) + abs(iColors[0].g - oPixel.g) + abs(iColors[0].b - oPixel.b);
					int d1 = abs(iColors[1].r - oPixel.r) + abs(iColors[1].g - oPixel.g) + abs(iColors[1].b - oPixel.b);
					int d2 = abs(iColors[2].r - oPixel.r) + abs(iColors[2].g - oPixel.g) + abs(iColors[2].b - oPixel.b);
					int d3 = abs(iColors[3].r - oPixel.r) + abs(iColors[3].g - oPixel.g) + abs(iColors[3].b - oPixel.b);

					int b0 = d0 > d3;
					int b1 = d1 > d2;
					int b2 = d0 > d2;
					int b3 = d1 > d3;
					int b4 = d2 > d3;

					int x0 = b1 & b2;
					int x1 = b0 & b3;
					int x2 = b0 & b4;

					pOutBlock->iPixelIndex |= (x2 | ((x0 | x1) << 1)) << (i << 1);
				}
			}
		}

		void Convert_BC2_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut)
		{
			BlockBC2* pBlock = (BlockBC2*)pIn;
			RGBA8* pOut4x4RGBA = (RGBA8*)pOut;

			RGBA8 iColors[4];
			Convert_BGR565_To_RGB8(&pBlock->iColor[0], &iColors[0], 0, 0);
			Convert_BGR565_To_RGB8(&pBlock->iColor[1], &iColors[1], 0, 0);
			iColors[0].a = iColors[1].a = 255;

			RGBA8Lerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
			RGBA8Lerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);

			for (int i = 0; i < 16; ++i)
			{
				int y = i / 4;
				int x = i - y * 4;

				int iColorIndex = (pBlock->iPixelIndex >> (i * 2)) & 0x3;
				int iAlphaIndex = (pBlock->iAlphaIndex >> (i * 4)) & 0xF;

				RGBA8& oOutPixel = pOut4x4RGBA[y * iPitchOut + x];
				RGBA8& oInPixel = iColors[iColorIndex];

				oOutPixel.r = oInPixel.r;
				oOutPixel.g = oInPixel.g;
				oOutPixel.b = oInPixel.b;
				oOutPixel.a = (uint8_t)(iAlphaIndex * 255 / 15);
			}
		}

		void Convert_BC3_To_RGBA8(void* pIn, void* pOut, size_t iPitchIn, size_t iPitchOut)
		{
			BlockBC3* pBlock = (BlockBC3*)pIn;
			RGBA8* pOut4x4RGBA = (RGBA8*)pOut;

			RGB8 iColors[4];
			Convert_BGR565_To_RGB8(&pBlock->iColor[0], &iColors[0], 0, 0);
			Convert_BGR565_To_RGB8(&pBlock->iColor[1], &iColors[1], 0, 0);

			RGB8Lerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
			RGB8Lerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);

			uint8_t iAlphas[8];
			iAlphas[0] = pBlock->iAlpha[0];
			iAlphas[1] = pBlock->iAlpha[1];

			if (iAlphas[0] > iAlphas[1])
			{
				iAlphas[2] = (iAlphas[0] * 6 + iAlphas[1] * 1) / 7;
				iAlphas[3] = (iAlphas[0] * 5 + iAlphas[1] * 2) / 7;
				iAlphas[4] = (iAlphas[0] * 4 + iAlphas[1] * 3) / 7;
				iAlphas[5] = (iAlphas[0] * 3 + iAlphas[1] * 4) / 7;
				iAlphas[6] = (iAlphas[0] * 2 + iAlphas[1] * 5) / 7;
				iAlphas[7] = (iAlphas[0] * 1 + iAlphas[1] * 6) / 7;
			}
			else
			{
				iAlphas[2] = (iAlphas[0] * 4 + iAlphas[1] * 1) / 5;
				iAlphas[3] = (iAlphas[0] * 3 + iAlphas[1] * 2) / 5;
				iAlphas[4] = (iAlphas[0] * 2 + iAlphas[1] * 3) / 5;
				iAlphas[5] = (iAlphas[0] * 1 + iAlphas[1] * 4) / 5;
				iAlphas[6] = 0;
				iAlphas[7] = 255;
			}

			for (int i = 0; i < 16; ++i)
			{
				int y = i / 4;
				int x = i - y * 4;

				int iColorIndex = (pBlock->iPixelIndex >> (i * 2)) & 0x3;
				int iAlphaIndex = (((uint64_t)pBlock->iAlphaIndex + ((uint64_t)pBlock->iAlphaIndex2 << 32)) >> (i * 3)) & 0x7;

				RGBA8& oOutPixel = pOut4x4RGBA[y * iPitchOut + x];
				RGB8& oInPixel = iColors[iColorIndex];

				oOutPixel.r = oInPixel.r;
				oOutPixel.g = oInPixel.g;
				oOutPixel.b = oInPixel.b;
				oOutPixel.a = iAlphas[iAlphaIndex];
			}
		}

		static bool s_bConvertionMatrixInit = false;

		typedef struct
		{
			ConvertionFunc pFunc;
			int iAdditionalBits;
		} ConvertionFuncDeclaration;

		ConvertionFuncDeclaration s_pConvertionMatrix[_E_PIXELFORMAT_COUNT][_E_PIXELFORMAT_COUNT] = {};

		void InitConvertionMatrix()
		{
			if (s_bConvertionMatrixInit)
				return;

			s_bConvertionMatrixInit = true;

			memset(s_pConvertionMatrix, 0, _E_PIXELFORMAT_COUNT * _E_PIXELFORMAT_COUNT * sizeof(ConvertionFunc));

			// RGB8 <=> RGBA8
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_RGB8_To_RGBA8, 8 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA8_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_RGBA8_To_RGB8, -8 };

			// R8 <=> RGB8
			s_pConvertionMatrix[E_PIXELFORMAT_R8_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_R8_To_RGB8, 16 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_R8_UNORM] = { Convert_RGB8_To_R8, -16 };

			// RG8 <=> RGB8
			s_pConvertionMatrix[E_PIXELFORMAT_RG8_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_RG8_To_RGB8, 8 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_RG8_UNORM] = { Convert_RGB8_To_RG8, -8 };

			// BGR8 <=> RGB8
			s_pConvertionMatrix[E_PIXELFORMAT_BGR8_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_BGR8_To_RGB8, 1 }; // Not added bits but different
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_BGR8_UNORM] = { Convert_RGB8_To_BGR8, 1 };

			// BGRA8 <=> RGBA8
			s_pConvertionMatrix[E_PIXELFORMAT_BGRA8_UNORM][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_BGRA8_To_RGBA8, 1 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA8_UNORM][E_PIXELFORMAT_BGRA8_UNORM] = { Convert_RGBA8_To_BGRA8, 1 };

			// RGB565 <=> RGB8
			s_pConvertionMatrix[E_PIXELFORMAT_R5G6B5_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_RGB565_To_RGB8, 8 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_R5G6B5_UNORM] = { Convert_RGB8_To_RGB565, -8 };

			// BGR565 <=> RGB8
			s_pConvertionMatrix[E_PIXELFORMAT_B5G6BR_UNORM][E_PIXELFORMAT_RGB8_UNORM] = { Convert_BGR565_To_RGB8, 8 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_B5G6BR_UNORM] = { Convert_RGB8_To_BGR565, -8 };

			// RGB16F <=> RGB32F
			s_pConvertionMatrix[E_PIXELFORMAT_RGB16_FLOAT][E_PIXELFORMAT_RGB32_FLOAT] = { Convert_RGB16F_To_RGB32F, 48 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB32_FLOAT][E_PIXELFORMAT_RGB16_FLOAT] = { Convert_RGB32F_To_RGB16F, -48 };

			// RGBA16F <=> RGBA32F
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA16_FLOAT][E_PIXELFORMAT_RGBA32_FLOAT] = { Convert_RGBA16F_To_RGBA32F, 64 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA32_FLOAT][E_PIXELFORMAT_RGBA16_FLOAT] = { Convert_RGBA32F_To_RGBA16F, -64 };

			// RGB32F <=> RGBA32F
			s_pConvertionMatrix[E_PIXELFORMAT_RGB32_FLOAT][E_PIXELFORMAT_RGBA32_FLOAT] = { Convert_RGB32F_To_RGBA32F, 32 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA32_FLOAT][E_PIXELFORMAT_RGB32_FLOAT] = { Convert_RGBA32F_To_RGB32F, -32 };

			// RGBA16 => RGBA16F
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA16_UNORM][E_PIXELFORMAT_RGBA16_FLOAT] = { Convert_RGBA16_To_RGBA16F, -1 }; //Losing quality?

			// RGB16F <=> RGBA16F
			s_pConvertionMatrix[E_PIXELFORMAT_RGB16_FLOAT][E_PIXELFORMAT_RGBA16_FLOAT] = { Convert_RGB16F_To_RGBA16F, 16 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA16_FLOAT][E_PIXELFORMAT_RGB16_FLOAT] = { Convert_RGBA16F_To_RGB16F, -16 };

			// RGBA8 <=> RGBA32F
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA8_UNORM][E_PIXELFORMAT_RGBA32_FLOAT] = { Convert_RGBA8_To_RGBA32F, 96 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA32_FLOAT][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_RGBA32F_To_RGBA8, -96 };

			// RGB8 >=> RGB32F
			s_pConvertionMatrix[E_PIXELFORMAT_RGB8_UNORM][E_PIXELFORMAT_RGB32_FLOAT] = { Convert_RGB8_To_RGB32F, 72 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGB32_FLOAT][E_PIXELFORMAT_RGB8_UNORM] = { Convert_RGB32F_To_RGB8, -72 };

			// R10G10B10A2 <=> RGBA32F
			s_pConvertionMatrix[E_PIXELFORMAT_R10G10B10A2_UNORM][E_PIXELFORMAT_RGBA32_FLOAT] = { Convert_R10G10B10A2_To_RGBA32F, 96 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA32_FLOAT][E_PIXELFORMAT_R10G10B10A2_UNORM] = { Convert_RGBA32F_To_R10G10B10A2, -96 };

			// R9G9B9E5 => RGB32F
			s_pConvertionMatrix[E_PIXELFORMAT_R9G9B9E5_SHAREDEXP][E_PIXELFORMAT_RGB32_FLOAT] = { Convert_R9G9B9E5_To_RGB32F, 64 };

			// BC1 => RGBA
			s_pConvertionMatrix[E_PIXELFORMAT_BC1][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_BC1_To_RGBA8, 28 };
			s_pConvertionMatrix[E_PIXELFORMAT_RGBA8_UNORM][E_PIXELFORMAT_BC1] = { Convert_RGBA8_To_BC1, -28 };

			// BC2 => RGBA
			s_pConvertionMatrix[E_PIXELFORMAT_BC2][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_BC2_To_RGBA8, 24 };

			// BC3 => RGBA
			s_pConvertionMatrix[E_PIXELFORMAT_BC3][E_PIXELFORMAT_RGBA8_UNORM] = { Convert_BC3_To_RGBA8, 24 };
		}

		bool GetConvertionChain(EPixelFormat eSourcePixelFormat, EPixelFormat eDestPixelFormat, ConvertionFuncChain* pOutChain, int* pOutChainLength, int* pOutAdditionalBits)
		{
			InitConvertionMatrix();

			CORE_ASSERT(pOutChain != NULL && pOutChainLength != NULL && pOutAdditionalBits != NULL);

			if (pOutChain == NULL || pOutChainLength == NULL || pOutAdditionalBits == NULL)
				return false;

			memset(pOutChain, 0, sizeof(ConvertionFuncChain));

			if (s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].pFunc != NULL)
			{
				(*pOutChain)[0].pFunc = s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].pFunc;
				(*pOutChain)[0].eFormat = eDestPixelFormat;
				*pOutChainLength = 1;
				*pOutAdditionalBits = s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].iAdditionalBits;
				return true;
			}

			EPixelFormat eCurrentFormat = eSourcePixelFormat;
			EPixelFormat eChain[32] = { E_PIXELFORMAT_NONE };
			int iCurrentChain = 0;

			while (iCurrentChain < 32)
			{
				ConvertionFuncDeclaration* pInfos = s_pConvertionMatrix[eCurrentFormat];

				EPixelFormat eBestFormat = E_PIXELFORMAT_NONE;
				int iBestAddBits = -999;
				if (pInfos[eDestPixelFormat].pFunc != NULL)
				{
					eBestFormat = eDestPixelFormat;
				}
				else
				{
					for (int i = 0; i < _E_PIXELFORMAT_COUNT; ++i)
					{
						if (pInfos[i].pFunc != NULL)
						{
							bool bAlreadyInChain = false;
							for (int iChain = 0; iChain < iCurrentChain; ++iChain)
							{
								if (eChain[iChain] == i)
								{
									bAlreadyInChain = true;
									break;
								}
							}

							if (bAlreadyInChain)
								continue;

							if (eBestFormat == E_PIXELFORMAT_NONE || pInfos[i].iAdditionalBits >= 0)
							{
								eBestFormat = (EPixelFormat)i;
								if (pInfos[i].iAdditionalBits == 0)
									break;
							}
						}
					}
				}

				if (eBestFormat != E_PIXELFORMAT_NONE)
				{
					eChain[iCurrentChain] = eBestFormat;
					++iCurrentChain;
					eCurrentFormat = eBestFormat;
					if (eBestFormat == eDestPixelFormat)
					{
						break;
					}
					continue;
				}

				break;
			}

			if (iCurrentChain > 0 && eChain[iCurrentChain - 1] == eDestPixelFormat)
			{
				int iAdditionalBits = 0;
				eCurrentFormat = eSourcePixelFormat;
				for (int i = 0; i < iCurrentChain; ++i)
				{
					ConvertionFuncDeclaration& oInfo = s_pConvertionMatrix[eCurrentFormat][eChain[i]];
					(*pOutChain)[i].pFunc = oInfo.pFunc;
					(*pOutChain)[i].eFormat = eChain[i];
					iAdditionalBits += oInfo.iAdditionalBits;
					eCurrentFormat = eChain[i];
				}
				*pOutChainLength = iCurrentChain;
				*pOutAdditionalBits = iAdditionalBits;
				return true;
			}

			return false;
		}

		int GetAvailableConvertion(EPixelFormat eSourcePixelFormat, bool bIncludeChains, ConvertionInfoList* pOutAvailablePixelFormat)
		{
			InitConvertionMatrix();
			memset(pOutAvailablePixelFormat, 0, sizeof(ConvertionInfoList));
			int iCurrentOutIndex = 0;

			Graphics::PixelFormat::ConvertionFuncChain oConvertionFuncChain;
			int iConvertionChainLength;
			int iAdditionalBits;

			ConvertionFuncDeclaration* pConvertionList = s_pConvertionMatrix[eSourcePixelFormat];
			for (int i = 0; i < _E_PIXELFORMAT_COUNT; ++i)
			{
				if (i == eSourcePixelFormat)
					continue;

				if (pConvertionList[i].pFunc != NULL) //TODO check bLosingQuality
				{
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].eFormat = (EPixelFormat)i;
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].iAdditionalBits = pConvertionList[i].iAdditionalBits;
					++iCurrentOutIndex;
				}
				else if (bIncludeChains && GetConvertionChain(eSourcePixelFormat, (EPixelFormat)i, &oConvertionFuncChain, &iConvertionChainLength, &iAdditionalBits))
				{
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].eFormat = (EPixelFormat)i;
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].iAdditionalBits = iAdditionalBits;
					++iCurrentOutIndex;
				}
			}
			return iCurrentOutIndex;
		}
	}
}
