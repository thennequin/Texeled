#include "Texture/TextureLoaders/TextureLoaderPNG.h"

#include "Core/Logger.h"

#include "Texture/TextureLoader.h"

#include <png.h>
#ifdef _MSC_VER
// warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4611)
#endif


namespace Texture
{
	namespace TextureLoader
	{
		void user_error_fn(png_structp /*pPng*/, png_const_charp pErrorMsg)
		{
			Core::LogError("TextureLoaderPNG", "%s", pErrorMsg);
		}

		void user_warning_fn(png_structp /*pPng*/, png_const_charp pWarningMsg)
		{
			Core::LogWarning("TextureLoaderPNG", "%s\n", pWarningMsg);
		}

		void TextureLoaderPNG_ReadCallback(png_structp pPng, png_bytep pOut, size_t iSize)
		{
			IO::Stream* pStream = static_cast<IO::Stream*>(png_get_io_ptr(pPng));

			if (pStream->Read(pOut, iSize) != iSize)
			{
				png_error(pPng, "read error");
			}
		}

		ErrorCode TextureLoaderPNG(IO::Stream* pStream, Graphics::Texture* pTexture)
		{
			png_byte pHeader[8];
			if (pStream->Read(pHeader, 8) != 8)
				return ErrorCode::Fail;

			if (png_sig_cmp(pHeader, 0, 8) != 0)
				return ErrorCode::Fail;

			png_structp pPNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn);
			if (pPNG == NULL)
			{
				return ErrorCode(1, "png_create_read_struct failed");
			}

			png_infop pPNGInfo = png_create_info_struct(pPNG);
			if (pPNGInfo == NULL)
			{
				png_destroy_read_struct(&pPNG, NULL, NULL);
				return ErrorCode(1, "PNG error");
			}

			if (setjmp(png_jmpbuf(pPNG)))
			{
				png_destroy_read_struct(&pPNG, &pPNGInfo, NULL);

				return ErrorCode(1, "Error during init_io %d", 0);
			}

			png_set_read_fn(pPNG, (void *)pStream, TextureLoaderPNG_ReadCallback);
			png_set_sig_bytes(pPNG, 8);

			png_read_info(pPNG, pPNGInfo);

			png_uint_32 iWidth = png_get_image_width(pPNG, pPNGInfo);
			png_uint_32 iHeight = png_get_image_height(pPNG, pPNGInfo);
			png_byte iColorType = png_get_color_type(pPNG, pPNGInfo);
			png_byte iBitDepth = png_get_bit_depth(pPNG, pPNGInfo);
			int iPassCount = png_set_interlace_handling(pPNG);

			if (iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			{
				// Convert 16 bits PNG_COLOR_TYPE_GRAY or PNG_COLOR_TYPE_GRAY_ALPHA to PNG_COLOR_TYPE_RGB or PNG_COLOR_TYPE_RGBA
				if (iBitDepth == 16)
					png_set_gray_to_rgb(pPNG);

				// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
				if (iBitDepth < 8)
					png_set_expand_gray_1_2_4_to_8(pPNG);
			}

			// Convert palette color type to RGB
			if (iColorType == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(pPNG);

			if (png_get_valid(pPNG, pPNGInfo, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(pPNG);

			png_read_update_info(pPNG, pPNGInfo);

			png_byte iNewiColorType = png_get_color_type(pPNG, pPNGInfo);
			png_byte iNewBitDepth = png_get_bit_depth(pPNG, pPNGInfo);

			size_t iPNGRowSize = png_get_rowbytes(pPNG, pPNGInfo);

			CORE_PTR_VOID pImage = Core::Malloc(iPNGRowSize * iHeight);

			if (setjmp(png_jmpbuf(pPNG)))
			{
				png_destroy_read_struct(&pPNG, &pPNGInfo, NULL);
				Core::Free(pImage);

				return ErrorCode(1, "Error during png_read");
			}

			for (int iPass = 0; iPass < iPassCount; ++iPass)
			{
				for (png_uint_32 iY = 0; iY < iHeight; iY++)
				{
					CORE_PTR(png_byte) pRow = CORE_PTR_CAST(png_byte, pImage) + iPNGRowSize * iY;
					png_read_row(pPNG, pRow, NULL);
				}
			}

			Graphics::Texture::Desc oDesc;
			oDesc.iWidth = iWidth;
			oDesc.iHeight = iHeight;
			int iPixelSize;
			switch (iNewiColorType)
			{
			case PNG_COLOR_TYPE_GRAY:
				if (iNewBitDepth == 8)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::R8_UNORM;
					iPixelSize = 1;
				}
				else if (iNewBitDepth == 16)
				{
					// Converted to RGB
					//TODO
				}
				break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				if (iNewBitDepth == 8)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RG8_UNORM;
					iPixelSize = 2;
				}
				else if (iNewBitDepth == 16)
				{
					// Converted to RGBA
					//TODO
				}
				break;
			case PNG_COLOR_TYPE_RGB:
				if (iNewBitDepth == 8)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
					iPixelSize = 3;
				}
				else if (iNewBitDepth == 16)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB16_UNORM;
					iPixelSize = 6;
				}
				break;
			case PNG_COLOR_TYPE_RGBA:
				if (iNewBitDepth == 8)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
					iPixelSize = 4;
				}
				else if (iNewBitDepth == 16)
				{
					oDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA16_UNORM;
					iPixelSize = 8;
				}
				break;
			default:
				break;
			}

			if (oDesc.ePixelFormat != Graphics::PixelFormatEnum::_NONE)
			{
				ErrorCode oErr = pTexture->Create(oDesc);

				Core::MemCpy(pTexture->GetSliceData(0, 0, 0).pData, pImage, iPNGRowSize * iHeight);

				if (oErr != ErrorCode::Ok)
				{
					png_destroy_read_struct(&pPNG, &pPNGInfo, NULL);
					Core::Free(pImage);

					return oErr;
				}
			}

			png_destroy_read_struct(&pPNG, &pPNGInfo, NULL);
			Core::Free(pImage);

			return ErrorCode::Ok;
		}

		static TextureLoaderHandle s_hTextureLoaderPNG = TextureLoaderHandleInvalid;

		void RegisterLoaderPNG()
		{
			CORE_ASSERT(s_hTextureLoaderPNG == TextureLoaderHandleInvalid);
			s_hTextureLoaderPNG = RegisterTextureLoader("Portable Network Graphics", "*.png\0", TextureLoaderPNG);
		}

		const TextureLoaderInfo* GetLoaderPNG()
		{
			return GetTextureLoader(s_hTextureLoaderPNG);
		}
	}
	//namespace TextureLoader
}
//namespace Texture