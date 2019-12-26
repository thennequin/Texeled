#include "Graphics/PixelFormatConvertersCompressed.h"

#include "Graphics/PixelFormatConverters.h"

#include "Core/Assert.h"
#include "Math/Math.h"

#include <string.h> //memset

namespace Graphics
{
	namespace PixelFormat
	{
		namespace Converters
		{
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

			void Convert_BC1_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
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

			void Convert_RGBA8_To_BC1(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
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

				if (Math::Abs(iLowestColor.r - iHighestColor.r) <= 8 && Math::Abs(iLowestColor.g - iHighestColor.g) <= 4 && Math::Abs(iLowestColor.b - iHighestColor.b) <= 8)
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

					RGB8Lerp(*(RGB8*)&iColors[0], *(RGB8*)&iColors[1], (RGB8*)&iColors[2], 1, 1, 2, 0);
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

						int d0 = Math::Abs(iColors[0].r - oPixel.r) + Math::Abs(iColors[0].g - oPixel.g) + Math::Abs(iColors[0].b - oPixel.b);
						int d1 = Math::Abs(iColors[1].r - oPixel.r) + Math::Abs(iColors[1].g - oPixel.g) + Math::Abs(iColors[1].b - oPixel.b);
						int d2 = Math::Abs(iColors[2].r - oPixel.r) + Math::Abs(iColors[2].g - oPixel.g) + Math::Abs(iColors[2].b - oPixel.b);

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

						int d0 = Math::Abs(iColors[0].r - oPixel.r) + Math::Abs(iColors[0].g - oPixel.g) + Math::Abs(iColors[0].b - oPixel.b);
						int d1 = Math::Abs(iColors[1].r - oPixel.r) + Math::Abs(iColors[1].g - oPixel.g) + Math::Abs(iColors[1].b - oPixel.b);
						int d2 = Math::Abs(iColors[2].r - oPixel.r) + Math::Abs(iColors[2].g - oPixel.g) + Math::Abs(iColors[2].b - oPixel.b);
						int d3 = Math::Abs(iColors[3].r - oPixel.r) + Math::Abs(iColors[3].g - oPixel.g) + Math::Abs(iColors[3].b - oPixel.b);

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

			void Convert_BC2_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
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

			void Convert_BC3_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
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
		}
	}
}