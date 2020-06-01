#include "IO/Clipboard.h"

#include "Core/Assert.h"
#include "Core/StringUtils.h"

#include "IO/MemoryStream.h"

#include "Texture/TextureLoader.h"
#include "Texture/TextureLoaders/TextureLoaderPNG.h"

#include <string.h> // memcpy

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace IO
{
	namespace Clipboard
	{
		bool GetText(Core::String* pOutText)
		{
			CORE_ASSERT(pOutText != NULL);
			if (pOutText == NULL)
			{
				return false;
			}

			if (OpenClipboard(NULL) == FALSE)
			{
				return false;
			}

			HANDLE hClipboardTextData;
			if ((hClipboardTextData = GetClipboardData(CF_UNICODETEXT)) != NULL)
			{
				const wchar_t* pWideText = (const wchar_t*)GlobalLock(hClipboardTextData);
				if (pWideText == NULL)
				{
					GlobalUnlock(hClipboardTextData);
					CloseClipboard();
					return false;
				}

				size_t iTextDataSize = GlobalSize(hClipboardTextData);
				size_t iWideTextLen = wcsnlen_s(pWideText, iTextDataSize);
				size_t iUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pWideText, iWideTextLen, NULL, 0, NULL, NULL);

				pOutText->resize(iUtf8Len);

				WideCharToMultiByte(CP_UTF8, 0, pWideText, iWideTextLen, pOutText->begin(), iUtf8Len, NULL, NULL);
			}
			else if ((hClipboardTextData = GetClipboardData(CF_TEXT)) != NULL)
			{
				const char* pText = (const char*)GlobalLock(hClipboardTextData);
				if (pText == NULL)
				{
					GlobalUnlock(hClipboardTextData);
					CloseClipboard();
					return false;
				}

				size_t iTextDataSize = GlobalSize(hClipboardTextData);
				size_t pTextLen = Core::StringUtils::StrLen(pText, iTextDataSize);

				pOutText->resize(pTextLen);
				memcpy(pOutText->begin(), pText, pTextLen);
			}
			else
			{
				CloseClipboard();
				return false;
			}

			GlobalUnlock(hClipboardTextData);
			CloseClipboard();
			return true;
		}

		bool SetText(const char* pText)
		{
			if (pText == NULL)
			{
				return false;
			}

			size_t iLen = strlen(pText);

			if (OpenClipboard(NULL) == FALSE)
			{
				return false;
			}

			HGLOBAL hClipboardTextData = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)(iLen + 1));
			if (hClipboardTextData == NULL)
			{
				CloseClipboard();
				return false;
			}

			char* pClipboardText = (char*)GlobalLock(hClipboardTextData);
			if (pClipboardText == NULL)
			{
				GlobalUnlock(hClipboardTextData);
				CloseClipboard();
				return false;
			}

			memcpy(pClipboardText, pText, iLen + 1);

			GlobalUnlock(hClipboardTextData);

			if (EmptyClipboard() == false)
			{
				GlobalFree(hClipboardTextData);
				CloseClipboard();
				return false;
			}

			SetClipboardData(CF_TEXT, hClipboardTextData);

			CloseClipboard();
			return true;
		}

		bool CopyTextureFromDIB(Graphics::Texture* pOutTexture)
		{
			HANDLE hClipboardBitmapData = GetClipboardData(CF_DIBV5);
			if (hClipboardBitmapData == NULL)
			{
				return false;
			}

			BITMAPINFO* pBitmapInfo = (BITMAPINFO*)GlobalLock(hClipboardBitmapData);
			if (pBitmapInfo == NULL)
			{
				GlobalUnlock(hClipboardBitmapData);
				return false;
			}

			if (pBitmapInfo->bmiHeader.biCompression == BI_JPEG || pBitmapInfo->bmiHeader.biCompression == BI_PNG)
			{
				CORE_PTR_VOID pData = Core::ToPointer((LPSTR)pBitmapInfo + pBitmapInfo->bmiHeader.biSize, pBitmapInfo->bmiHeader.biSizeImage);
				IO::MemoryStream oMemoryStream(pData, pBitmapInfo->bmiHeader.biSizeImage);
				if (Texture::LoadFromStream(pOutTexture, &oMemoryStream) == ErrorCode::Ok)
				{
					GlobalUnlock(hClipboardBitmapData);
					return true;
				}
			}
			else if (pBitmapInfo->bmiHeader.biCompression == BI_RGB || pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS)
			{
				Graphics::Texture::Desc oTextureDesc;
				oTextureDesc.iWidth = pBitmapInfo->bmiHeader.biWidth;
				oTextureDesc.iHeight = pBitmapInfo->bmiHeader.biHeight;

				uint32_t iRGBAMasks[4];
				uint32_t iRGBAShifts[4];
				int8_t iRGBADefault[4] = { 0, 0, 0, 255 };

				if (pBitmapInfo->bmiHeader.biCompression == BI_BITFIELDS)
				{
					oTextureDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
					iRGBAMasks[0] = *((uint32_t*)&pBitmapInfo->bmiColors[0]);
					iRGBAMasks[1] = *((uint32_t*)&pBitmapInfo->bmiColors[1]);
					iRGBAMasks[2] = *((uint32_t*)&pBitmapInfo->bmiColors[2]);
					iRGBAMasks[3] = 0;
				}
				else if (pBitmapInfo->bmiHeader.biCompression == BI_RGB)
				{
					switch (pBitmapInfo->bmiHeader.biBitCount)
					{
					case 24:
						oTextureDesc.ePixelFormat = Graphics::PixelFormatEnum::RGB8_UNORM;
						break;
					case 32:
						oTextureDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
						break;
					}
					iRGBAMasks[0] = 0x00ff0000;
					iRGBAMasks[1] = 0x0000ff00;
					iRGBAMasks[2] = 0x000000ff;
					iRGBAMasks[3] = 0xff000000;
				}

				for (int i = 0; i < 4; ++i)
				{
					if (iRGBAMasks[i] == 0)
					{
						iRGBAShifts[i] = 0;
					}
					else
					{
						int iShift = 0;
						while (((iRGBAMasks[i] >> iShift) & 1) != 1)
						{
							iShift++;
						}
						iRGBAShifts[i] = iShift;
					}
				}

				if (oTextureDesc.ePixelFormat == Graphics::PixelFormatEnum::_NONE || pOutTexture->Create(oTextureDesc) != ErrorCode::Ok)
				{
					GlobalUnlock(hClipboardBitmapData);
					return false;
				}

				int iInputPixelByteCount = pBitmapInfo->bmiHeader.biBitCount / 8;
				int iOutputPixelByteCount = Graphics::PixelFormatEnumInfos[oTextureDesc.ePixelFormat].iBlockSize;
				int iOutputPixelComponentCount = Graphics::PixelFormatEnumInfos[oTextureDesc.ePixelFormat].iComponentCount;

				CORE_PTR_VOID pBitmapData = Core::ToPointer((LPSTR)pBitmapInfo + pBitmapInfo->bmiHeader.biSize, oTextureDesc.iHeight * oTextureDesc.iWidth * iInputPixelByteCount);

				const Graphics::Texture::SliceData oSliceData = pOutTexture->GetSliceData(0, 0, 0);
				for (int iY = 0; iY < oTextureDesc.iHeight; ++iY)
				{
					CORE_PTR(char) pRowIn = CORE_PTR_CAST(char, pBitmapData) + iY * oTextureDesc.iWidth * iInputPixelByteCount;
					CORE_PTR(char) pRowOut = CORE_PTR_CAST(char, oSliceData.pData) + (oTextureDesc.iHeight - iY - 1) * oSliceData.iPitch;

#ifndef DEBUG
#pragma omp parallel for
#endif
					for (int iX = 0; iX < oTextureDesc.iWidth; ++iX)
					{
						CORE_PTR(char) pPixelIn = pRowIn + iX * iInputPixelByteCount;
						CORE_PTR(char) pPixelOut = pRowOut + iX * iOutputPixelByteCount;

						uint32_t iPixel = 0;
						memcpy(&iPixel, (void*)pPixelIn, iInputPixelByteCount);

						for (size_t iC = 0; iC < iOutputPixelComponentCount; ++iC)
							pPixelOut[iC] = (iRGBAMasks[iC] != 0) ? ((iPixel & iRGBAMasks[iC]) >> iRGBAShifts[iC]) : iRGBADefault[iC];
					}
				}

				GlobalUnlock(hClipboardBitmapData);
				return true;
			}
		}

		bool CopyTextureFromPNG(Graphics::Texture* pOutTexture)
		{
			UINT iFormat = RegisterClipboardFormat("PNG");
			if (iFormat == 0)
				return false;

			HANDLE hClipboardBitmapData = GetClipboardData(iFormat);
			if (hClipboardBitmapData == NULL)
			{
				return false;
			}

			void* pData = GlobalLock(hClipboardBitmapData);
			if (pData == NULL)
				return false;

			size_t iDataSize = GlobalSize(hClipboardBitmapData);

			IO::MemoryStream oMemoryStream(pData, iDataSize);

			const Texture::TextureLoaderInfo* pLoaderPNG = Texture::TextureLoader::GetLoaderPNG();
			if (Texture::LoadFromStream(pOutTexture, &oMemoryStream, pLoaderPNG) != ErrorCode::Ok)
			{
				GlobalUnlock(hClipboardBitmapData);
				return false;
			}

			GlobalUnlock(hClipboardBitmapData);
			return true;
		}

		bool GetTexture(Graphics::Texture* pOutTexture)
		{
			CORE_ASSERT(pOutTexture != NULL);
			if (pOutTexture == NULL)
			{
				return false;
			}

			if (OpenClipboard(NULL) == FALSE)
			{
				return false;
			}

			// Prioritize PNG
			if (CopyTextureFromPNG(pOutTexture))
			{
				CloseClipboard();
				return true;
			}
			else if (CopyTextureFromDIB(pOutTexture))
			{
				CloseClipboard();
				return true;
			}

			CloseClipboard();
			return false;
		}

		bool SetTexture(const Graphics::Texture& /*oTexture*/)
		{
			CORE_NOT_IMPLEMENTED();
			return false;
		}
	}
}
//namespace IO