#include "Graphics/TextureUtils.h"

#include "Core/Assert.h"
#include "Core/FileStream.h"
#include "Core/StringUtils.h"

#include "Math/Math.h"

#include <string.h> //memcpy/memset
#include <emmintrin.h> //SIMD
#include <math.h> // sqrt/cos/sin/..

namespace Graphics
{
	const char* const ECubemapFormat_string[_E_CUBEMAPFORMAT_COUNT] = {
		"None",
		"StripVertical",
		"StripHorizontal",
		"CrossVertical",
		"CrossHorizontal",
		"LatLong",
		};

	bool DetermineCubemapFormatFromImageSize(int iWidth, int iHeight, ECubemapFormat* pOutFormat, int* pOutFaceSize)
	{
		float fRatio = iWidth / (float)iHeight;
		if (fRatio == (1.f / 6.f))
		{
			if (pOutFormat) *pOutFormat = E_CUBEMAPFORMAT_STRIP_VERTICAL;
			if (pOutFaceSize) *pOutFaceSize = iWidth;
			return true;
		}
		else if (fRatio == (6.f / 1.f))
		{
			if (pOutFormat) *pOutFormat = E_CUBEMAPFORMAT_STRIP_HORIZONTAL;
			if (pOutFaceSize) *pOutFaceSize = iHeight;
			return true;
		}
		else if (fRatio == (3.f / 4.f))
		{
			if (pOutFormat) *pOutFormat = E_CUBEMAPFORMAT_CROSS_VERTICAL;
			if (pOutFaceSize) *pOutFaceSize = iHeight / 4;
			return true;
		}
		else if (fRatio == (4.f / 3.f))
		{
			if (pOutFormat) *pOutFormat = E_CUBEMAPFORMAT_CROSS_HORIZONTAL;
			if (pOutFaceSize) *pOutFaceSize = iWidth / 4;
			return true;
		}
		else if (fRatio == (2.f / 1.f))
		{
			if (pOutFormat) *pOutFormat = E_CUBEMAPFORMAT_LATLONG;
			if (pOutFaceSize) *pOutFaceSize = iHeight / 2;
			return true;
		}
		else if (fRatio == (1.f / 1.f))
		{
			// Spherical map
			// Not supported
		}

		return false;
	}

	bool GetCubemapFacePos(int iWidth, int iHeight, ECubemapFormat eFormat, Texture::EFace eFace, int* pOutX, int* pOutY)
	{
		if (eFormat == E_CUBEMAPFORMAT_STRIP_VERTICAL)
		{
			*pOutX = 0;
			*pOutY = (eFace - Texture::E_FACE_POS_X) * iWidth;
			return true;
		}
		else if (eFormat == E_CUBEMAPFORMAT_STRIP_HORIZONTAL)
		{
			*pOutX = (eFace - Texture::E_FACE_POS_X) * iHeight;
			*pOutY = 0;
			return true;
		}
		else if (eFormat == E_CUBEMAPFORMAT_CROSS_HORIZONTAL)
		{
			int iFaceSize = iWidth / 4;
			int iX, iY;
			if (eFace == Texture::E_FACE_POS_X)
			{
				iX = 2;
				iY = 1;
			}
			else if (eFace == Texture::E_FACE_NEG_X)
			{
				iX = 0;
				iY = 1;
			}
			else if (eFace == Texture::E_FACE_POS_Y)
			{
				iX = 1;
				iY = 0;
			}
			else if (eFace == Texture::E_FACE_NEG_Y)
			{
				iX = 1;
				iY = 2;
			}
			else if (eFace == Texture::E_FACE_POS_Z)
			{
				iX = 1;
				iY = 1;
			}
			else if (eFace == Texture::E_FACE_NEG_Z)
			{
				iX = 3;
				iY = 1;
			}

			*pOutX = iX * iFaceSize;
			*pOutY = iY * iFaceSize;
			return true;
		}
		else if (eFormat == E_CUBEMAPFORMAT_CROSS_VERTICAL)
		{
			int iFaceSize = iHeight / 4;
			int iX, iY;
			if (eFace == Texture::E_FACE_POS_X)
			{
				iX = 1;
				iY = 1;
			}
			else if (eFace == Texture::E_FACE_NEG_X)
			{
				iX = 1;
				iY = 3;
			}
			else if (eFace == Texture::E_FACE_POS_Y)
			{
				iX = 1;
				iY = 0;
			}
			else if (eFace == Texture::E_FACE_NEG_Y)
			{
				iX = 1;
				iY = 2;
			}
			else if (eFace == Texture::E_FACE_POS_Z)
			{
				iX = 0;
				iY = 1;
			}
			else if (eFace == Texture::E_FACE_NEG_Z)
			{
				iX = 2;
				iY = 1;
			}
			*pOutX = iX * iFaceSize;
			*pOutY = iY * iFaceSize;
			return true;
		}
		return false;
	}

	ErrorCode ConvertTextureToCubemap(const Graphics::Texture& oTexture, Graphics::Texture* pOutCubemap)
	{
		if (pOutCubemap == NULL)
		{
			return ErrorCode(1, "Out cubemap texture argument is null");
		}

		int iSourceWidth = oTexture.GetWidth();
		int iSourceHeight = oTexture.GetHeight();

		ECubemapFormat eCubemapFormat;
		int iFaceSize;

		if (oTexture.GetFaceCount() != 1)
		{
			return ErrorCode(1, "Source texture need to have only one face data when bDataIsCubemap is true");
		}

		if (DetermineCubemapFormatFromImageSize(iSourceWidth, iSourceHeight, &eCubemapFormat, &iFaceSize) == false)
		{
			return ErrorCode(1, "Source is not a valid cubemap format");
		}

		Texture::Desc oDesc;
		oDesc.ePixelFormat = oTexture.GetPixelFormat();
		oDesc.iWidth = oDesc.iHeight = iFaceSize;
		oDesc.iFaceCount = 6;
		oDesc.iMipCount = oTexture.GetMipCount();

		size_t iBits = PixelFormat::BitPerPixel(oTexture.GetPixelFormat());
		for (int iMip = 0, iMipCount = oTexture.GetMipCount(); iMip < iMipCount; ++iMip)
		{
			int iCubeMapSourceWidthMinusOne = iSourceWidth - 1;
			int iCubeMapSourceHeighthMinusOne = iSourceHeight - 1;
			for (int iFace = 0; iFace < Texture::_E_FACE_COUNT; ++iFace)
			{
				const Texture::TextureFaceData& oFaceData = pOutCubemap->GetData().GetFaceData(iMip, iFace);

				if (eCubemapFormat != E_CUBEMAPFORMAT_LATLONG)
				{
					int iX, iY;
					if (GetCubemapFacePos(iSourceWidth, iSourceHeight, eCubemapFormat, (Texture::EFace)iFace, &iX, &iY) == false)
					{
						CORE_ASSERT(false);
					}
					for (int iLine = 0; iLine < iFaceSize; ++iLine)
					{
						char* pSource = (char*)oTexture.GetData().GetFaceData(iMip, 0).pData + (iX + (iY + iLine) * iSourceWidth) * iBits / 8;
						char* pDest = (char*)oFaceData.pData + iLine * iFaceSize * iBits / 8;
						memcpy(pDest, pSource, iFaceSize * iBits / 8);
					}
				}
				else
				{
					const float c_fPi = 3.14159265358979323846f;
					const float c_fInvPi = 0.31830988618379067153f;
					const float c_f2Pi = 6.28318530717958647692f;

					const float c_fFaceUvVectors[6][3][4] = // Last value is useless but necessary for sse loading
					{
						{ // +x
							{ 0.0f,  0.0f, -1.0f, 0.f },
							{ 0.0f, -1.0f,  0.0f, 0.f },
							{ 1.0f,  0.0f,  0.0f, 0.f }
						},
						{ // -x
							{ 0.0f,  0.0f,  1.0f, 0.f },
							{ 0.0f, -1.0f,  0.0f, 0.f },
							{ -1.0f, 0.0f,  0.0f, 0.f }
						},
						{ // +y
							{ 1.0f,  0.0f,  0.0f, 0.f },
							{ 0.0f,  0.0f,  1.0f, 0.f },
							{ 0.0f,  1.0f,  0.0f, 0.f }
						},
						{ // -y
							{ 1.0f,  0.0f,  0.0f, 0.f },
							{ 0.0f,  0.0f, -1.0f, 0.f },
							{ 0.0f, -1.0f,  0.0f, 0.f }
						},
						{ // +z
							{ 1.0f,  0.0f,  0.0f, 0.f },
							{ 0.0f, -1.0f,  0.0f, 0.f },
							{ 0.0f,  0.0f,  1.0f, 0.f }
						},
						{ // -z
							{ -1.0f, 0.0f,  0.0f, 0.f },
							{ 0.0f, -1.0f,  0.0f, 0.f },
							{ 0.0f,  0.0f, -1.0f, 0.f }
						}
					};

					const float fInverseFaceSize = 1.f / (iFaceSize - 1);

					__m128 xFaceVec0 = _mm_loadu_ps(c_fFaceUvVectors[iFace][0]);
					__m128 xFaceVec1 = _mm_loadu_ps(c_fFaceUvVectors[iFace][1]);
					__m128 xFaceVec2 = _mm_loadu_ps(c_fFaceUvVectors[iFace][2]);

					#pragma omp parallel for
					for (int iY = 0; iY < iFaceSize; ++iY)
					{
						float fV = 2.f * fInverseFaceSize * iY - 1.f;
						__m128 xV = _mm_set1_ps(fV);

						#pragma omp parallel for
						for (int iX = 0; iX < iFaceSize; ++iX)
						{
							float fU = 2.f * fInverseFaceSize * iX - 1.f;
							__m128 xU = _mm_set1_ps(fU);

							__m128 xVec0 = _mm_mul_ps(xFaceVec0, xU);
							__m128 xVec1 = _mm_mul_ps(xFaceVec1, xU);
							__m128 xVec2 = _mm_add_ps(xVec0, xVec1);
							__m128 xVecF = _mm_add_ps(xVec2, xFaceVec1);

							__m128 xVecFSqr = _mm_mul_ps(xVecF, xVecF);
							//__m128 xVecFSqr1 = _mm_shuffle_ps(xVecFSqr, xVecFSqr, _MM_SHUFFLE(0, 0, 0, 0));
							//__m128 xVecFSqr2 = _mm_shuffle_ps(xVecFSqr, xVecFSqr, _MM_SHUFFLE(1, 1, 1, 0));
							//__m128 xVecFSqr3 = _mm_shuffle_ps(xVecFSqr, xVecFSqr, _MM_SHUFFLE(2, 2, 2, 0));
							//__m128 xLen = _mm_sqrt_ps()
							float fVecfSqr[4];
							_mm_storeu_ps(fVecfSqr, xVecFSqr);
							xVecFSqr = _mm_set1_ps(fVecfSqr[0] + fVecfSqr[1] + fVecfSqr[2]);
							xVecFSqr = _mm_rsqrt_ps(xVecFSqr);
							xVecFSqr = _mm_mul_ps(xVecFSqr, xVecF);
							float fVec[4];
							_mm_storeu_ps(fVec, xVecFSqr);

							const float fPhi = atan2f(fVec[0], fVec[2]);
							const float fTheta = acosf(fVec[1]);

							float fUV[2];
							fUV[0] = ((c_fPi + fPhi) / c_fPi) * 0.5f;
							fUV[1] = fTheta * c_fInvPi;

							float fSrcX = fUV[0] * iCubeMapSourceWidthMinusOne;
							float fSrcY = fUV[1] * iCubeMapSourceHeighthMinusOne;

							//Bilinear
							int iX0 = (int)floor(fSrcX);
							int iX1 = fmin(iX0 + 1, iCubeMapSourceWidthMinusOne);
							int iY0 = (int)floor(fSrcY);
							int iY1 = fmin(iY0 + 1, iCubeMapSourceHeighthMinusOne);

							/*char* pSource = (char*)oDesc.pData[iMip][0] (iX + iY * iSourceWidth) * iBits / 8;

							const float* pSrcPixel00 = pSource + oDesc.iComponentCount * (iX0 + iY0 * iSourceWidth);
							const float* pSrcPixel01 = pSource + oDesc.iComponentCount * (iX0 + iY1 * iSourceWidth);
							const float* pSrcPixel10 = pSource + oDesc.iComponentCount * (iX1 + iY0 * iSourceWidth);
							const float* pSrcPixel11 = pSource + oDesc.iComponentCount * (iX1 + iY1 * iSourceWidth);

							float fTX = fSrcX - iX0;
							float fTY = fSrcY - iY0;

							float fMinusTX = 1.f - fTX;
							float fMinusTY = 1.f - fTY;

							pDstPixel[0] = pSrcPixel00[0] * fMinusTX * fMinusTY + pSrcPixel01[0] * fMinusTX * fTY + pSrcPixel10[0] * fTX * fMinusTY + pSrcPixel11[0] * fTX * fTY;
							pDstPixel[1] = pSrcPixel00[1] * fMinusTX * fMinusTY + pSrcPixel01[1] * fMinusTX * fTY + pSrcPixel10[1] * fTX * fMinusTY + pSrcPixel11[1] * fTX * fTY;
							pDstPixel[2] = pSrcPixel00[2] * fMinusTX * fMinusTY + pSrcPixel01[2] * fMinusTX * fTY + pSrcPixel10[2] * fTX * fMinusTY + pSrcPixel11[2] * fTX * fTY;
							pDstPixel[3] = pSrcPixel00[3] * fMinusTX * fMinusTY + pSrcPixel01[3] * fMinusTX * fTY + pSrcPixel10[3] * fTX * fMinusTY + pSrcPixel11[3] * fTX * fTY;
							*/
						}
					}
				}
			}
		}

		return ErrorCode::Fail;
	}

	ErrorCode ConvertPixelFormat(const Texture* pTexture, Texture* pOutTexture, EPixelFormat eWantedPixelFormat)
	{
		if (pTexture == NULL || pOutTexture == NULL)
		{
			return ErrorCode(1, "Invalid argument");
		}

		if (eWantedPixelFormat != pTexture->GetPixelFormat() )
		{
			PixelFormat::ConvertionFuncChain oConvertionFuncChain;
			int iConvertionChainLength;
			int iAdditionalBits;
			if (PixelFormat::GetConvertionChain(pTexture->GetPixelFormat(), eWantedPixelFormat, &oConvertionFuncChain, &iConvertionChainLength, &iAdditionalBits) == false)
			{
				return ErrorCode(1, "Format convertion not implemented");
			}

			Texture oNewTexture;
			Texture::Desc oNewDesc;
			oNewDesc.ePixelFormat = eWantedPixelFormat;
			oNewDesc.iWidth = pTexture->GetWidth();
			oNewDesc.iHeight = pTexture->GetHeight();
			oNewDesc.iFaceCount = pTexture->GetFaceCount();
			oNewDesc.iMipCount = pTexture->GetMipCount();
			if (oNewTexture.Create(oNewDesc) != ErrorCode::Ok)
			{
				return ErrorCode(1, "Can't create new Texture");
			}

			size_t iTotalSize = 0;
			for (int iMipIndex = 0, iMipCount = pTexture->GetMipCount(); iMipIndex < iMipCount; ++iMipIndex)
			{
				for (int iFaceIndex = 0, iFaceCount = oNewDesc.iFaceCount; iFaceIndex < iFaceCount; ++iFaceIndex)
				{
					const Texture::TextureFaceData& oNewFaceData = oNewTexture.GetData().GetFaceData(iMipIndex, iFaceIndex);
					const Texture::TextureFaceData& oFaceData = pTexture->GetData().GetFaceData(iMipIndex, iFaceIndex);

					int iSourceBits = PixelFormat::BitPerPixel(pTexture->GetPixelFormat());
					int iDestBits = PixelFormat::BitPerPixel(eWantedPixelFormat);

					uint32_t iSrcMipWidth = oFaceData.iWidth;
					uint32_t iSrcMipHeight = oFaceData.iHeight;
					uint32_t iDestMipWidth = oNewFaceData.iWidth;
					uint32_t iDestMipHeight = oNewFaceData.iHeight;
					uint32_t iSrcPaddingX, iSrcPaddingY, iDestPaddingX, iDestPaddingY;

					PixelFormat::GetDataSize(pTexture->GetPixelFormat(), &iSrcMipWidth, &iSrcMipHeight, &iSrcPaddingX, &iSrcPaddingY);
					PixelFormat::GetDataSize(eWantedPixelFormat, &iDestMipWidth, &iDestMipHeight, &iDestPaddingX, &iDestPaddingY);

					uint32_t iMipWidth = Math::Min(iSrcMipWidth, iDestMipWidth);
					uint32_t iMipHeight = Math::Min(iSrcMipHeight, iDestMipHeight);
					uint32_t iPaddingX = Math::Max(iSrcPaddingX, iDestPaddingX);
					uint32_t iPaddingY = Math::Max(iSrcPaddingY, iDestPaddingY);

#pragma omp parallel for
					for (int iY = 0; iY < iMipHeight; iY += iPaddingY)
					{
#pragma omp parallel for
						for (int iX = 0; iX < iMipWidth; iX += iPaddingX)
						{
							PixelFormat::ConvertionTemporaryData oConvertionTempData[2];
							void* pSourceData = (char*)oFaceData.pData + (size_t)(iY * iSrcMipWidth + iX * iSrcPaddingY) * iSourceBits / 8;
							void* pNewData = (char*)oNewFaceData.pData + (size_t)(iY * iDestMipWidth + iX * iDestPaddingY) * iDestBits / 8;

							EPixelFormat eCurrentFormat = pTexture->GetPixelFormat();
							uint32_t iCurrentBits = PixelFormat::BitPerPixel(eCurrentFormat);
							uint32_t iCurrentPaddingX, iCurrentPaddingY;
							PixelFormat::GetDataSize(eCurrentFormat, NULL, NULL, &iCurrentPaddingX, &iCurrentPaddingY);

							for (int iChain = 0; iChain < iConvertionChainLength; ++iChain)
							{
								void* pInputData = (iChain == 0) ? pSourceData : ((iChain % 2 == 0) ? &oConvertionTempData[0] : &oConvertionTempData[1]);
								void* pOutputData = (iChain == (iConvertionChainLength - 1)) ? pNewData : ((iChain % 2 == 0) ? &oConvertionTempData[1] : &oConvertionTempData[0]);

								uint32_t iInputPitch = (iChain == 0) ? iSrcMipWidth : iPaddingX;
								uint32_t iOutputPitch = (iChain == (iConvertionChainLength - 1)) ? iDestMipWidth : iPaddingX;

								PixelFormat::ConvertionFuncInfo oFunc = oConvertionFuncChain[iChain];

								uint32_t iNextBits = PixelFormat::BitPerPixel(oFunc.eFormat);

								uint32_t iNextPaddingX, iNextPaddingY;
								PixelFormat::GetDataSize(oFunc.eFormat, NULL, NULL, &iNextPaddingX, &iNextPaddingY);

								uint32_t iFuncPaddingX = Math::Max(iCurrentPaddingX, iNextPaddingX);
								uint32_t iFuncPaddingY = Math::Max(iCurrentPaddingY, iNextPaddingY);

								int iTW = Math::Min(iPaddingX, iMipWidth - iX);
								int iTH = Math::Min(iPaddingY, iMipHeight - iY);

								for (int iTY = 0; iTY < iTH; iTY += iFuncPaddingY)
								{
									for (int iTX = 0; iTX < iTW; iTX += iFuncPaddingX)
									{
										void* pTempInput = (char*)pInputData + (size_t)(iTY * iInputPitch + iTX) * iCurrentBits / 8;
										void* pTempOutput = (char*)pOutputData + (size_t)(iTY * iOutputPitch + iTX) * iNextBits / 8;

										oFunc.pFunc(pTempInput, pTempOutput, iInputPitch, iOutputPitch);
									}
								}

								eCurrentFormat = oFunc.eFormat;
								iCurrentBits = iNextBits;
								iCurrentPaddingX = iNextPaddingX;
								iCurrentPaddingY = iNextPaddingY;
							}
						}
					}
				}
			}
			pOutTexture->Swap(oNewTexture);
			return ErrorCode::Ok;
		}
		return ErrorCode(1, "Same format");
	}

	typedef struct
	{
		const char*				pName;
		const char*				pExts;
		TextureLoaderFunc		pLoader;
	} TextureLoaderInfo;

	typedef struct
	{
		const char*				pName;
		const char*				pExt;
		TextureWriterFunc				pWriter;
		TextureWriterSupportedFunc		pTester;
	} TextureWriterInfo;

	static Core::Array<TextureLoaderInfo>	s_oTextureLoaders;
	static Core::Array<TextureWriterInfo>	s_oTextureWriters;

	void RegisterTextureLoader(const char* pName, const char* pExts, TextureLoaderFunc pLoader)
	{
		CORE_ASSERT(pName != NULL);
		CORE_ASSERT(pExts != NULL && pExts[strlen(pExts)] == 0);
		CORE_ASSERT(pLoader != NULL);

		TextureLoaderInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExts = pExts;
		oInfo.pLoader = pLoader;
		s_oTextureLoaders.push_back(oInfo);
	}

	void RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester)
	{
		TextureWriterInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExt = pExts;
		oInfo.pWriter = pWriter;
		oInfo.pTester = pWriterTester;
		s_oTextureWriters.push_back(oInfo);
	}

	ErrorCode LoadFromStream(Texture* pTexture, Core::Stream* pStream)
	{
		if (pTexture != NULL && pStream != NULL && pStream->IsReadable())
		{
			Texture oTemp;
			for (Core::Array<TextureLoaderInfo>::iterator it = s_oTextureLoaders.begin(), itEnd = s_oTextureLoaders.end(); it != itEnd; ++it)
			{
				pStream->Seek(0, Core::Stream::E_SEEK_MODE_BEGIN);
				if (it->pLoader(pStream, &oTemp))
				{
					pTexture->Swap(oTemp);
					return ErrorCode::Ok;
				}
			}
		}
		return ErrorCode::Fail;
	}

	ErrorCode LoadFromFile(Texture* pTexture, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			Core::FileStream oFileStream;
			if (oFileStream.Open(pFilename, Core::FileStream::E_ACCESS_MODE_READ))
			{
				ErrorCode oErr = LoadFromStream(pTexture, &oFileStream);

				oFileStream.Close();
				return oErr;
			}
		}

		return ErrorCode::Fail;
	}

	ErrorCode SaveToStream(Texture* pTexture, const WriterSettings* pSettings, Core::Stream* pStream, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			for (Core::Array<TextureWriterInfo>::iterator it = s_oTextureWriters.begin(), itEnd = s_oTextureWriters.end(); it != itEnd; ++it)
			{
				if (Core::StringUtils::EndsWith(pFilename, it->pExt, false))
				{
					WriterSettings oSettings;
					if (it->pWriter(pTexture, pSettings, pStream))
					{
						return ErrorCode::Ok;
					}
					else
					{
						return ErrorCode::Fail;
					}
				}
			}
		}
		return ErrorCode::Fail;
	}

	ErrorCode SaveToFile(Texture* pTexture, const WriterSettings* pSettings, const char* pFilename)
	{
		if (pTexture != NULL)
		{
			Core::FileStream oFileStream;
			if (oFileStream.Open(pFilename, Core::FileStream::E_ACCESS_MODE_WRITE))
			{
				ErrorCode oErr = SaveToStream(pTexture, pSettings, &oFileStream, pFilename);

				oFileStream.Close();
				return oErr;
			}
		}
		return ErrorCode::Fail;
	}
}