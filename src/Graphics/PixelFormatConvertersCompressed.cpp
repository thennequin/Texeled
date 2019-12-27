#include "Graphics/PixelFormatConvertersCompressed.h"

#include "CMP_Core.h"

#include <string.h> //memcpy

namespace Graphics
{
	namespace PixelFormat
	{
		namespace Converters
		{
			// BC1
			void Convert_BC1_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oColors[64];
				DecompressBlockBC1((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut * 4, oColors + iY * 16, 16);
				}
			}

			void Convert_RGBA8_To_BC1(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC1((unsigned char*)pIn, (unsigned int)iPitchIn * 4, (unsigned char*)pOut, NULL);
			}

			// BC2
			void Convert_BC2_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oColors[64];
				DecompressBlockBC2((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut * 4, oColors + iY * 16, 16);
				}
			}

			void Convert_RGBA8_To_BC2(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC2((unsigned char*)pIn, (unsigned int)iPitchIn * 4, (unsigned char*)pOut, NULL);
			}

			// BC3
			void Convert_BC3_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oColors[64];
				DecompressBlockBC3((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut * 4, oColors + iY * 16, 16);
				}
			}

			void Convert_RGBA8_To_BC3(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC3((unsigned char*)pIn, (unsigned int)iPitchIn * 4, (unsigned char*)pOut, NULL);
			}

			// BC4
			void Convert_BC4_To_R8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oColors[16];
				DecompressBlockBC4((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut, oColors + iY * 4, 4);
				}
			}

			void Convert_R8_To_BC4(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC4((unsigned char*)pIn, (unsigned int)iPitchIn, (unsigned char*)pOut, NULL);
			}

			// BC5
			void Convert_BC5_To_RG8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oChannelA[16];
				unsigned char oChannelB[16];
				DecompressBlockBC5((unsigned char*)pIn, oChannelA, oChannelB, NULL);

				for (int iY = 0; iY < 4; ++iY)
				{
					for (int iX = 0; iX < 4; ++iX)
					{
						((unsigned char*)pOut)[iY * (unsigned int)iPitchOut * 2 + iX * 2 + 0] = oChannelA[iY * 4 + iX];
						((unsigned char*)pOut)[iY * (unsigned int)iPitchOut * 2 + iX * 2 + 1] = oChannelB[iY * 4 + iX];
					}
				}
			}

			void Convert_RG8_To_BC5(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				unsigned char oChannelA[16];
				unsigned char oChannelB[16];
				for (int iY = 0; iY < 4; ++iY)
				{
					for (int iX = 0; iX < 4; ++iX)
					{
						oChannelA[iY * 4 + iX] = ((unsigned char*)pIn)[iY * iPitchIn * 2 + iX * 2 + 0];
						oChannelB[iY * 4 + iX] = ((unsigned char*)pIn)[iY * iPitchIn * 2 + iX * 2 + 1];
					}
				}
				CompressBlockBC5(oChannelA, 4, oChannelB, 4, (unsigned char*)pOut, NULL);
			}

			// BC6H
			void Convert_BC6H_To_RGB16F(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned short oColors[48];
				DecompressBlockBC6((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut * 3 * 2, oColors + iY * 4 * 3, 4 * 3 * 2);
				}
			}

			void Convert_RGB16F_To_BC6H(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC6((unsigned short*)pIn, (unsigned int)iPitchIn * 3, (unsigned char*)pOut, NULL);
			}

			// BC7
			void Convert_BC7_To_RGBA8(void* pIn, void* pOut, size_t /*iPitchIn*/, size_t iPitchOut)
			{
				unsigned char oColors[64];
				DecompressBlockBC7((unsigned char*)pIn, oColors, NULL);
				for (int iY = 0; iY < 4; ++iY)
				{
					memcpy((unsigned char*)pOut + iY * (unsigned int)iPitchOut * 4, oColors + iY * 16, 16);
				}
			}

			void Convert_RGBA8_To_BC7(void* pIn, void* pOut, size_t iPitchIn, size_t /*iPitchOut*/)
			{
				CompressBlockBC7((unsigned char*)pIn, (unsigned int)iPitchIn * 4, (unsigned char*)pOut, NULL);
			}
		}
	}
}