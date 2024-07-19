#include "Texture/TextureLoaders/TextureLoaderDDS.h"

#include "Core/Logger.h"

#include "Math/Arithmetic.h"

#include "Texture/DDS.h"
#include "Texture/TextureLoader.h"

#include <stdlib.h>
#include <stdio.h>
#include <algorithm> // memcpy

namespace Texture
{
	namespace TextureLoader
	{
		ErrorCode TextureLoaderDDS(IO::Stream* pStream, Graphics::Texture* pTexture)
		{
			Graphics::Texture::Desc oDesc;
			uint32_t iDDSMagic;
			DDS_HEADER oDDSHeader;
			DDS_HEADER_DXT10 oDDSHeaderDX10;

			if (pStream->Read(&iDDSMagic) == false || iDDSMagic != DDS_MAGIC)
			{
				return ErrorCode::Fail;
				//return ErrorCode(1, "Not a DDS");
			}

			if (pStream->Read(&oDDSHeader) == false
				|| oDDSHeader.iSize != sizeof(DDS_HEADER)
				|| oDDSHeader.oPixelFormat.iSize != sizeof(DDS_PIXELFORMAT))
			{
				return ErrorCode(1, "Invalid DDS header");
			}

			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_CAPS) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_CAPS");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_HEIGHT) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_HEIGHT");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_WIDTH) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_WIDTH");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PITCH) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_PITCH");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PIXELFORMAT) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_PIXELFORMAT");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_MIPMAPCOUNT) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_MIPMAPCOUNT");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_LINEARSIZE");
			if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_DEPTH) != 0)
				Core::LogDebug("TextureLoaderDDS", "DDS_HEADER_FLAGS_DEPTH");

			char pFourCC[5] = "0000";
			pFourCC[0] = oDDSHeader.oPixelFormat.iFourCC & 0xff;
			pFourCC[1] = (oDDSHeader.oPixelFormat.iFourCC >> 8) & 0xff;
			pFourCC[2] = (oDDSHeader.oPixelFormat.iFourCC >> 16) & 0xff;
			pFourCC[3] = (oDDSHeader.oPixelFormat.iFourCC >> 24) & 0xff;

			oDesc.iWidth = oDDSHeader.iWidth;
			oDesc.iHeight = oDDSHeader.iHeight;
			oDesc.iMipCount = ((oDDSHeader.iSurfaceFlags & DDS_SURFACE_FLAGS_MIPMAP) && (oDDSHeader.iMipMapCount > 0)) ? oDDSHeader.iMipMapCount : 1;

			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEX)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::POS_X;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEX)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::NEG_X;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEY)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::POS_Y;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEY)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::NEG_Y;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_POSITIVEZ)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::POS_Z;
			if (oDDSHeader.iCubemapFlags & DDS_CUBEMAP_NEGATIVEZ)
				oDesc.eFaces |= Graphics::Texture::FaceFlag::NEG_Z;

			if (oDDSHeader.iDepth > 1 && oDesc.eFaces != Graphics::Texture::FaceFlag::NONE)
			{
				return ErrorCode(1, "Not supported or invalid DDS, contains depth and cubemap faces");
			}

			uint8_t iFaceCount = Math::HighBitCount(oDesc.eFaces);
			oDesc.iSliceCount = iFaceCount > 0 ? iFaceCount : Math::Max((uint16_t)1, (uint16_t)oDDSHeader.iDepth);

			if ((oDDSHeader.iHeaderFlags & DDS_FOURCC)
				&& oDDSHeader.oPixelFormat.iFourCC == DDSPF_DX10.iFourCC
				//&& memcmp(&oDDSHeader.oPixelFormat, &DDSPF_DX10, sizeof(DDS_PIXELFORMAT)) == 0
				)
			{
				if (pStream->Read(&oDDSHeaderDX10) == false)
				{
					return ErrorCode(1, "Invalid DDS DX10 header");
				}

				if (   oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8_UINT)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
				}
				else if (  oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8G8B8A8_TYPELESS
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R8G8B8A8_UINT)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_B5G6R5_UNORM)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::B5G6BR_UNORM;
				}
				else if (  oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_TYPELESS
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R10G10B10A2_UINT)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::R10G10B10A2_UNORM;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA16_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32_FLOAT )
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::R32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32_FLOAT )
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32B32_FLOAT )
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB32_FLOAT;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT )
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::R9G9B9E5_SHAREDEXP;
				}
				else if( oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_D16_UNORM )
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::D16_UNORM;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC1_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC1_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC1_UNORM_SRGB)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC1;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC2_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC2_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC2_UNORM_SRGB)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC2;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC3_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC3_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC3_UNORM_SRGB)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC3;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC4_SNORM)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC4;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_UNORM
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC5_SNORM)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_TYPELESS
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_UF16
					|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC6H_SF16)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC6H;
				}
				else if (oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_TYPELESS
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM
						|| oDDSHeaderDX10.oDxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC7;
				}
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BGRA8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_R8G8B8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_B8G8R8, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BGR8_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::R5G6B5_UNORM;
			}
			else if (memcmp(&oDDSHeader.oPixelFormat, &DDSPF_B5G6R5, sizeof(DDS_PIXELFORMAT)) == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::B5G6BR_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_DXT1.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC1;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_DXT2.iFourCC
				  || oDDSHeader.oPixelFormat.iFourCC == DDSPF_DXT3.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC2;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_DXT4.iFourCC
				  || oDDSHeader.oPixelFormat.iFourCC == DDSPF_DXT5.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC3;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_BC5U.iFourCC
				  || oDDSHeader.oPixelFormat.iFourCC == DDSPF_ATI2.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC5;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_BC6H.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BC6H;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_UNORM.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA16_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R16G16B16A16_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA16_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::R32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32G32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == DDSPF_R32G32B32A32_FLOAT.iFourCC)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 8
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
				//TODO : copy channel R to GBA
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
			}
			else if (oDDSHeader.oPixelFormat.iFourCC == 0
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 16
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xffff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0
				&& oDDSHeader.oPixelFormat.iBBitMask == 0
				&& oDDSHeader.oPixelFormat.iABitMask == 0) // Declaration seems weird
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
			}
			else if ((oDDSHeader.oPixelFormat.iFourCC == 0 || oDDSHeader.oPixelFormat.iFourCC == 20)
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 24
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BGR8_UNORM;
			}
			else if ((oDDSHeader.oPixelFormat.iFourCC == 0 || oDDSHeader.oPixelFormat.iFourCC == 21)
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BGRA8_UNORM;
			}
			else if ((oDDSHeader.oPixelFormat.iFourCC == 0 || oDDSHeader.oPixelFormat.iFourCC == 21)
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0xff000000)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
			}
			else if ((oDDSHeader.oPixelFormat.iFourCC == 0 || oDDSHeader.oPixelFormat.iFourCC == 21)
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::BGRA8_UNORM;
			}
			else if ((oDDSHeader.oPixelFormat.iFourCC == 0 || oDDSHeader.oPixelFormat.iFourCC == 21)
				&& oDDSHeader.oPixelFormat.iRGBBitCount == 32
				&& oDDSHeader.oPixelFormat.iRBitMask == 0xff
				&& oDDSHeader.oPixelFormat.iGBitMask == 0xff00
				&& oDDSHeader.oPixelFormat.iBBitMask == 0xff0000
				&& oDDSHeader.oPixelFormat.iABitMask == 0)
			{
				oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
			}

			if (oDesc.ePixelFormat == Graphics::PixelFormatEnum::_NONE)
			{
				return ErrorCode(1, "Not supported pixel format");
			}

			ErrorCode oErr = pTexture->Create(oDesc);
			if (oErr != ErrorCode::Ok)
			{
				return oErr;
			}

			const Graphics::PixelFormatInfos& oInfos = Graphics::PixelFormatEnumInfos[oDesc.ePixelFormat];

			for (int iFace = 0; iFace < oDesc.iSliceCount; ++iFace)
			{
				for (int iMip = 0; iMip < oDesc.iMipCount; ++iMip)
				{
					const Graphics::Texture::SliceData oSliceData = pTexture->GetSliceData(0, iMip, iFace);

					uint16_t iBlockCountX, iBlockCountY;
					Graphics::PixelFormat::GetBlockCount(oDesc.ePixelFormat, oSliceData.iWidth, oSliceData.iHeight, &iBlockCountX, &iBlockCountY);

					int iBlockCount = iBlockCountX * iBlockCountY;

					size_t iBlocksSize = iBlockCount * oInfos.iBlockSize;

					void* pBlocks = oSliceData.pData;

					if (oSliceData.iSize != iBlocksSize)
						return ErrorCode(2, "Internal : Invalid block size");

					size_t iRowSize = iBlockCountX * oInfos.iBlockSize;

					if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_PITCH) != 0)
					{
						size_t iPitchSize = oDDSHeader.iPitchOrLinearSize;
						if (iRowSize > iPitchSize)
						{
							return ErrorCode(2, "Internal : Invalid pitch size");
						}

						size_t iDiff = iPitchSize - iRowSize;
						size_t iRemainingPitch = iPitchSize;
						for (uint32_t iLine = 0; iLine < iBlockCountY; ++iLine)
						{
							if (iRowSize > iRemainingPitch)
							{
								pStream->Seek(iRemainingPitch, IO::Stream::SeekModeEnum::OFFSET);
								iRemainingPitch = iPitchSize;
							}

							{
								if (pStream->Read((char*)pBlocks + iLine * iRowSize, iRowSize) != iRowSize)
								{
									return ErrorCode(1, "Incomplete file");
								}
								iRemainingPitch -= iRowSize;
							}
						}
					}
					else if ((oDDSHeader.iHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE) != 0)
					{
						/*size_t iLinearSize = oDDSHeader.iPitchOrLinearSize;
						if (iMip == 0 && (iBlockCountY * oInfos.iBlockSize) != iLinearSize)
						{
							return false;
						}*/
						if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
						{
							return ErrorCode(1, "Incomplete file");
						}
					}
					else
					{
						if (pStream->Read(pBlocks, iBlocksSize) != iBlocksSize)
						{
							return ErrorCode(1, "Incomplete file");
						}
					}
				}
			}

			return ErrorCode::Ok;
		}

		void RegisterLoaderDDS()
		{
			RegisterTextureLoader("DirectDraw Surface", "*.dds\0", TextureLoaderDDS);
		}
	}
	//namespace TextureLoader
}
//namespace Texture