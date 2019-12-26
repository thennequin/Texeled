#include "Graphics/PixelFormatConverters.h"

#include "Math/Math.h"

#include <string.h> //memset

namespace Graphics
{
	namespace PixelFormat
	{
		namespace Converters
		{
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
				pOut88[0] = pIn888->r;
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

			void Convert_R32F_To_RG32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
			{
				float* pInR = (float*)pIn;
				RG32F* pOutRGB = (RG32F*)pOut;
				pOutRGB->r = *pInR;
				pOutRGB->g = 0.f;
			}

			void Convert_RG32F_To_R32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
			{
				RG32F* pInRG = (RG32F*)pIn;
				float* pOutR = (float*)pOut;
				*pOutR = pInRG->r;
			}

			void Convert_RG32F_To_RGB32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
			{
				RG32F* pInRG = (RG32F*)pIn;
				RGB32F* pOutRGB = (RGB32F*)pOut;
				pOutRGB->r = pInRG->r;
				pOutRGB->g = pInRG->g;
				pOutRGB->b = 0.f;
			}

			void Convert_RGB32F_To_RG32F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t /*iPitchOut*/)
			{
				RGB32F* pInRGB = (RGB32F*)pIn;
				RG32F* pOutRG = (RG32F*)pOut;
				pOutRG->r = pInRGB->r;
				pOutRG->g = pInRGB->g;
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
		}
	}
}