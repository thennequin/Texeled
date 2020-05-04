#include "Texture/TextureLoaders/TextureLoaderKTX.h"

#include "Texture/TextureLoader.h"

#include "Texture/KTX.h"

#include "Core/Array.h"

#include "Math/Arithmetic.h"

namespace Texture
{
	namespace TextureLoader
	{
		ErrorCode TextureLoaderKTX(IO::Stream* pStream, Graphics::Texture* pTexture);
		void RegisterLoaderKTX()
		{
			RegisterTextureLoader("Kronos Texture", ".ktx\0", TextureLoaderKTX);
		}

		ErrorCode TextureLoaderKTX(IO::Stream* pStream, Graphics::Texture* pTexture)
		{
			KTX::KTXHeader oHeader;

			if (pStream->Read(&oHeader) == false || memcmp(oHeader.pIdentifier, KTX::c_pIdentifier, 12) != 0)
			{
				return ErrorCode::Fail;
			}

			bool bBigEndian;

			uint8_t* pEndianness = (uint8_t*)&oHeader.iEndianness;
			if (pEndianness[0] == 0x01 && pEndianness[1] == 0x02 && pEndianness[2] == 0x03 && pEndianness[3] == 0x04)
			{
				bBigEndian = false;
			}
			else if (pEndianness[0] == 0x04 && pEndianness[1] == 0x03 && pEndianness[2] == 0x02 && pEndianness[3] == 0x01)
			{
				bBigEndian = true;
			}
			else
			{
				return ErrorCode(1, "KTX : Invalid endianness");
			}

			if (bBigEndian == true)
			{
				return ErrorCode(1, "KTX : Big endian file not supported");
			}

			Graphics::PixelFormatEnum ePixelFormat = Graphics::PixelFormatEnum::_NONE;

			if (oHeader.iGLType != 0)
			{
				if (oHeader.iGLFormat == 0)
				{
					return ErrorCode(1, "KTX : Invalid header");
				}

				switch (oHeader.iGLType)
				{
				case KTX::DataTypeEnum::GL_UNSIGNED_BYTE:
				{
					if (oHeader.iGLTypeSize != 1)
					{
						return ErrorCode(1, "KTX : Wrong type size");
					}

					switch (oHeader.iGLFormat)
					{
					case KTX::PixelFormatEnum::GL_LUMINANCE:
					case KTX::PixelFormatEnum::GL_RED:
					case KTX::PixelFormatEnum::GL_GREEN:
					case KTX::PixelFormatEnum::GL_BLUE:
						ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
						break;
					case KTX::PixelFormatEnum::GL_LUMINANCE_ALPHA:
						ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
						break;
					case KTX::PixelFormatEnum::GL_RGB:
						ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
						break;
					case KTX::PixelFormatEnum::GL_RGBA:
						ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
						break;
					default:
						return ErrorCode(1, "KTX : GLFormat is not supported for this type");
					}
					break;
				}
				case KTX::DataTypeEnum::GL_UNSIGNED_SHORT:
				{
					if (oHeader.iGLTypeSize != 2)
					{
						return ErrorCode(1, "KTX : Wrong type size");
					}

					return ErrorCode(1, "KTX : GLFormat is not supported for this type");

					break;
				}
				case KTX::DataTypeEnum::GL_FLOAT:
				{
					if (oHeader.iGLTypeSize != 4)
					{
						return ErrorCode(1, "KTX : Wrong type size");
					}

					switch (oHeader.iGLFormat)
					{
					case KTX::PixelFormatEnum::GL_RGB:
						ePixelFormat = Graphics::PixelFormatEnum::RGB16_FLOAT;
						break;
					case KTX::PixelFormatEnum::GL_RGBA:
						ePixelFormat = Graphics::PixelFormatEnum::RGBA16_FLOAT;
						break;
					default:
						return ErrorCode(1, "KTX : GLFormat is not supported for this type");
					}

					break;
				}
				default:
					return ErrorCode(1, "KTX : GLType not supported");
				}
			}
			else
			{
				switch (oHeader.iGLInternalFormat)
				{
				case KTX::PixelFormatEnum::GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
					ePixelFormat = Graphics::PixelFormatEnum::BC1;
					break;
				case KTX::PixelFormatEnum::GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
					ePixelFormat = Graphics::PixelFormatEnum::BC2;
					break;
				case KTX::PixelFormatEnum::GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
					ePixelFormat = Graphics::PixelFormatEnum::BC3;
					break;
				default:
					return ErrorCode(1, "KTX : GL Internal Format not supported");
				}
			}

			if (oHeader.iNumberOfArrayElements > 1)
			{
				return ErrorCode(1, "KTX : Not supported Array texture");
			}

			if (oHeader.iPixelDepth > 1)
			{
				return ErrorCode(1, "KTX : Not supported 3D texture");
			}

			Graphics::Texture::Desc oDesc;
			oDesc.ePixelFormat = ePixelFormat;
			oDesc.iWidth = oHeader.iPixelWidth > 0 ? oHeader.iPixelWidth : 1;
			oDesc.iHeight = oHeader.iPixelHeight > 0 ? oHeader.iPixelHeight : 1;
			oDesc.iFaceCount = oHeader.iNumberOfFaces > 0 ? oHeader.iNumberOfFaces : 1;
			oDesc.iMipCount = oHeader.iNumberOfMipmapLevels > 0 ? oHeader.iNumberOfMipmapLevels : 1;

			ErrorCode oErr = pTexture->Create(oDesc);
			if (oErr != ErrorCode::Ok)
			{
				return oErr;
			}

			uint32_t iReadKeyValuePair = 0;
			Core::Array<char> oTempKeyValuePair;
			while (iReadKeyValuePair < oHeader.iBytesOfKeyValueData)
			{
				uint32_t iKeyValueByteSize;

				if (pStream->Read(&iKeyValueByteSize) == false)
				{
					return ErrorCode::Fail;
				}

				oTempKeyValuePair.resize(iKeyValueByteSize);

				if (pStream->Read(oTempKeyValuePair.begin(), iKeyValueByteSize) == false)
				{
					return ErrorCode::Fail;
				}

				// Ignore key values
				//TODO

				uint32_t iPadding = 3 - ((iKeyValueByteSize + 3) % 4);
				if (pStream->Seek(iPadding, IO::Stream::SeekModeEnum::OFFSET) == false)
				{
					return ErrorCode::Fail;
				}

				iReadKeyValuePair += sizeof(uint32_t) + iKeyValueByteSize + iPadding;
			}

			const Graphics::PixelFormatInfos& oPixelFormatInfo = Graphics::PixelFormatEnumInfos[ePixelFormat];

			for (int iMip = 0; iMip < oDesc.iMipCount; ++iMip)
			{
				uint32_t iImageSize;
				if (pStream->Read(&iImageSize) == false)
				{
					return ErrorCode::Fail;
				}

				for (int iFace = 0; iFace < oDesc.iFaceCount; ++iFace)
				{
					//TODO z_slice
					{
						const Graphics::Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(iMip, iFace);

						int iBlock = 0;
						for (int iY = 0; iY < oFaceData.iHeight; iY += oPixelFormatInfo.iBlockHeight )
						{
							for (int iX = 0; iX < oFaceData.iWidth; iX += oPixelFormatInfo.iBlockWidth)
							{
								char* pBlock = (char*)oFaceData.pData + iBlock * oPixelFormatInfo.iBlockSize;
								CORE_ASSERT(iBlock * oPixelFormatInfo.iBlockSize < oFaceData.iSize);
								if (pStream->Read(pBlock, oPixelFormatInfo.iBlockSize) == false)
								{
									return ErrorCode::Fail;
								}
								iBlock++;
							}

							//End of line padding
							size_t iPadding = (oFaceData.iWidth * oPixelFormatInfo.iBlockWidth) % 4;
							if (pStream->Seek(iPadding, IO::Stream::SeekModeEnum::OFFSET) == false)
							{
								return ErrorCode::Fail;
							}
						}
					}
					//TODO cube padding[0-3]
				}
				//TODO mip padding[0-3]
			}

			return ErrorCode::Ok;
		}
	}
	//namespace TextureLoader
}
//namespace Texture