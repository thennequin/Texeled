#include "Texture/TextureLoaders/TextureLoaderEXR.h"

#include "Texture/TextureLoader.h"

#include "Core/Array.h"

#include "tinyexr.h"

namespace Texture
{
	namespace TextureLoader
	{
		ErrorCode TextureLoaderEXR(IO::Stream* pStream, Graphics::Texture* pTexture)
		{
			const char* pErr = NULL;

			// Read EXR version
			EXRVersion oExrVersion;
			unsigned char pBufferVersion[8];

			if (pStream->Read(pBufferVersion, 8) != 8
				|| ParseEXRVersionFromMemory(&oExrVersion, pBufferVersion, 8) != 0)
			{
				return ErrorCode::Fail;
				//return ErrorCode(1, "Invalid EXR file");
			}

			if (oExrVersion.multipart)
			{
				return ErrorCode(1, "Multipart not supported");
			}

			if (oExrVersion.non_image )
			{
				return ErrorCode(1, "Deep image not supported");
			}

			if (oExrVersion.tiled)
			{
				return ErrorCode(1, "Tiled image not supported");
			}

			Core::Array<unsigned char> oStreamContent;
			pStream->Seek(0, IO::Stream::SeekModeEnum::END);

			if (oStreamContent.resize(pStream->Tell(), false) == false)
			{
				return ErrorCode(1, "Not enought free memory");
			}

			pStream->Seek(0, IO::Stream::SeekModeEnum::BEGIN);

			if (pStream->Read(oStreamContent.begin(), oStreamContent.size()) != oStreamContent.size())
			{
				return ErrorCode(1, "Stream reading error");
			}

			// Read EXR header
			EXRHeader oExrHeader;
			InitEXRHeader(&oExrHeader);

			if (ParseEXRHeaderFromMemory(&oExrHeader, &oExrVersion, oStreamContent.begin(), oStreamContent.size(), &pErr) != 0)
			{
				ErrorCode oErr = ErrorCode(1, "Parse EXR error: %s", pErr);
				FreeEXRErrorMessage(pErr);
				return oErr;
			}

			//Force float
			for (int i = 0; i < oExrHeader.num_channels; i++)
			{
				oExrHeader.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
			}

			EXRImage oExrImage;
			InitEXRImage(&oExrImage);

			if (LoadEXRImageFromMemory(&oExrImage, &oExrHeader, oStreamContent.begin(), oStreamContent.size(), &pErr) != 0) {
				ErrorCode oErr = ErrorCode(1, "Load EXR error: %s", pErr);
				FreeEXRErrorMessage(pErr);
				FreeEXRHeader(&oExrHeader);
				return oErr;
			}

			oStreamContent.clear();

			int iChannels[4] = { -1, -1, -1, -1 };
			for (int i = 0; i < oExrImage.num_channels; i++)
			{
				switch (oExrHeader.channels[i].name[0])
				{
				case 'R': iChannels[0] = i; break;
				case 'G': iChannels[1] = i; break;
				case 'B': iChannels[2] = i; break;
				case 'A': iChannels[3] = i; break;
				}
			}

			Graphics::Texture::Desc oDesc;
			oDesc.ePixelFormat = (iChannels[3] != -1) ? Graphics::PixelFormatEnum::RGBA32_FLOAT : Graphics::PixelFormatEnum::RGB32_FLOAT;
			oDesc.iWidth = oExrImage.width;
			oDesc.iHeight = oExrImage.height;
			oDesc.iFaceCount = 1;
			oDesc.iMipCount = 1;

			int iChannelCount = (iChannels[3] != -1) ? 4 : 3;
			ErrorCode oErr = pTexture->Create(oDesc);
			if (oErr == ErrorCode::Ok)
			{
				float* pData = (float*)pTexture->GetData().GetFaceData(0, 0).pData;
				float** pSource = (float**)oExrImage.images;

				for (uint32_t i = 0; i < (uint32_t)oExrImage.width * (uint32_t)oExrImage.height; i++)
				{
					for (int c = 0; c < iChannelCount; c++)
					{
						if (iChannels[c] >= 0)
						{
							pData[i * iChannelCount + c] = pSource[iChannels[c]][i];
						}
						else
						{
							pData[i * iChannelCount + c] = 0.0f;
						}
					}
				}
			}

			FreeEXRImage(&oExrImage);
			FreeEXRHeader(&oExrHeader);

			return ErrorCode(oErr);
		}

		void RegisterLoaderEXR()
		{
			RegisterTextureLoader("OpenEXR", "*.exr\0", TextureLoaderEXR);
		}
	}
	//namespace TextureLoader
}
//namespace Texture