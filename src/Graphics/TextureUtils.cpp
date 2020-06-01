#include "Graphics/TextureUtils.h"

#include "Core/Assert.h"
#include "Core/StringUtils.h"

#include "IO/FileStream.h"

#include "Math/Arithmetic.h"
#include "Math/Constants.h"

#include <string.h> //memcpy/memset
#include <emmintrin.h> //SIMD
#include <math.h> // sqrt/cos/sin/..

#include "stb_image_resize.h"

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
		else if (fRatio == 1.f)
		{
			// Spherical map
			// Not supported
		}

		return false;
	}

	bool GetCubemapFacePos(int iWidth, int iHeight, ECubemapFormat eFormat, Texture::FaceFlag eFace, int* pOutX, int* pOutY)
	{
		if (eFormat == E_CUBEMAPFORMAT_STRIP_VERTICAL)
		{
			*pOutX = 0;
			*pOutY = (eFace - Texture::FaceFlag::POS_X) * iWidth;
			return true;
		}
		else if (eFormat == E_CUBEMAPFORMAT_STRIP_HORIZONTAL)
		{
			*pOutX = (eFace - Texture::FaceFlag::POS_X) * iHeight;
			*pOutY = 0;
			return true;
		}
		else if (eFormat == E_CUBEMAPFORMAT_CROSS_HORIZONTAL)
		{
			int iFaceSize = iWidth / 4;
			int iX, iY;
			if (eFace == Texture::FaceFlag::POS_X)
			{
				iX = 2;
				iY = 1;
			}
			else if (eFace == Texture::FaceFlag::NEG_X)
			{
				iX = 0;
				iY = 1;
			}
			else if (eFace == Texture::FaceFlag::POS_Y)
			{
				iX = 1;
				iY = 0;
			}
			else if (eFace == Texture::FaceFlag::NEG_Y)
			{
				iX = 1;
				iY = 2;
			}
			else if (eFace == Texture::FaceFlag::POS_Z)
			{
				iX = 1;
				iY = 1;
			}
			else if (eFace == Texture::FaceFlag::NEG_Z)
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
			if (eFace == Texture::FaceFlag::POS_X)
			{
				iX = 1;
				iY = 1;
			}
			else if (eFace == Texture::FaceFlag::NEG_X)
			{
				iX = 1;
				iY = 3;
			}
			else if (eFace == Texture::FaceFlag::POS_Y)
			{
				iX = 1;
				iY = 0;
			}
			else if (eFace == Texture::FaceFlag::NEG_Y)
			{
				iX = 1;
				iY = 2;
			}
			else if (eFace == Texture::FaceFlag::POS_Z)
			{
				iX = 0;
				iY = 1;
			}
			else if (eFace == Texture::FaceFlag::NEG_Z)
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

		if (oTexture.GetSliceCount() != 1)
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
		oDesc.iSliceCount = 6;
		oDesc.eFaces = Texture::FaceFlag::ALL;
		oDesc.iMipCount = oTexture.GetMipCount();

		size_t iBits = PixelFormat::BitPerPixel(oTexture.GetPixelFormat());
		for (int iMip = 0, iMipCount = oTexture.GetMipCount(); iMip < iMipCount; ++iMip)
		{
			int iCubeMapSourceWidthMinusOne = iSourceWidth - 1;
			int iCubeMapSourceHeighthMinusOne = iSourceHeight - 1;
			for (int iFace = 0; iFace < Texture::FaceFlag::_COUNT; ++iFace)
			{
				const Texture::SliceData oSliceData = pOutCubemap->GetSliceData(0, iMip, iFace);

				if (eCubemapFormat != E_CUBEMAPFORMAT_LATLONG)
				{
					int iX, iY;
					if (GetCubemapFacePos(iSourceWidth, iSourceHeight, eCubemapFormat, (Texture::FaceFlag)iFace, &iX, &iY) == false)
					{
						CORE_ASSERT(false);
					}
					for (int iLine = 0; iLine < iFaceSize; ++iLine)
					{
						char* pSource = (char*)oTexture.GetSliceData(0, iMip, 0).pData + (iX + (iY + iLine) * iSourceWidth) * iBits / 8;
						char* pDest = (char*)oSliceData.pData + iLine * iFaceSize * iBits / 8;
						memcpy(pDest, pSource, iFaceSize * iBits / 8);
					}
				}
				else
				{
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
							fUV[0] = ((Math::c_fPi + fPhi) / Math::c_fPi) * 0.5f;
							fUV[1] = fTheta * Math::c_fInvPi;

							float fSrcX = fUV[0] * iCubeMapSourceWidthMinusOne;
							float fSrcY = fUV[1] * iCubeMapSourceHeighthMinusOne;

							//Bilinear
							int iX0 = (int)floor(fSrcX);
							int iX1 = Math::Min(iX0 + 1, iCubeMapSourceWidthMinusOne);
							int iY0 = (int)floor(fSrcY);
							int iY1 = Math::Min(iY0 + 1, iCubeMapSourceHeighthMinusOne);

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

	ErrorCode ConvertPixelFormat(const Texture* pTexture, Texture* pOutTexture, PixelFormatEnum eWantedPixelFormat)
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
			oNewDesc.iMipCount = pTexture->GetMipCount();
			oNewDesc.iSliceCount = pTexture->GetSliceCount();
			if (oNewTexture.Create(oNewDesc) != ErrorCode::Ok)
			{
				return ErrorCode(1, "Can't create new Texture");
			}

			size_t iTotalSize = 0;
			for (int iMipIndex = 0, iMipCount = pTexture->GetMipCount(); iMipIndex < iMipCount; ++iMipIndex)
			{
				for (int iFaceIndex = 0, iFaceCount = oNewDesc.iSliceCount; iFaceIndex < iFaceCount; ++iFaceIndex)
				{
					const Texture::SliceData oNewSliceData = oNewTexture.GetSliceData(0, iMipIndex, iFaceIndex);
					const Texture::SliceData oSliceData = pTexture->GetSliceData(0, iMipIndex, iFaceIndex);

					int iSourceBits = PixelFormat::BitPerPixel(pTexture->GetPixelFormat());
					int iDestBits = PixelFormat::BitPerPixel(eWantedPixelFormat);

					const PixelFormatInfos& oSrcPFInfos = PixelFormatEnumInfos[pTexture->GetPixelFormat()];
					const PixelFormatInfos& oDstPFInfos = PixelFormatEnumInfos[eWantedPixelFormat];

					uint16_t iSrcMipBlockX;
					uint16_t iSrcMipBlockY;
					PixelFormat::GetBlockCount(pTexture->GetPixelFormat(), oSliceData.iWidth, oSliceData.iHeight, &iSrcMipBlockX, &iSrcMipBlockY);

					uint16_t iDstMipBlockX;
					uint16_t iDstMipBlockY;
					PixelFormat::GetBlockCount(eWantedPixelFormat, oNewSliceData.iWidth, oNewSliceData.iHeight, &iDstMipBlockX, &iDstMipBlockY);

					uint16_t iMipWidth = Math::Min<uint16_t>(iSrcMipBlockX * oSrcPFInfos.iBlockWidth, iDstMipBlockX * oDstPFInfos.iBlockWidth);
					uint16_t iMipHeight = Math::Min<uint16_t>(iSrcMipBlockY * oSrcPFInfos.iBlockHeight, iDstMipBlockY * oDstPFInfos.iBlockHeight);
					uint8_t iPaddingX = Math::Max<uint8_t>(oSrcPFInfos.iBlockWidth , oDstPFInfos.iBlockWidth);
					uint8_t iPaddingY = Math::Max<uint8_t>(oSrcPFInfos.iBlockHeight, oDstPFInfos.iBlockHeight);

#ifndef DEBUG
#pragma omp parallel for
#endif
					for (int iY = 0; iY < iMipHeight; iY += iPaddingY)
					{
#ifndef DEBUG
#pragma omp parallel for
#endif
						for (int iX = 0; iX < iMipWidth; iX += iPaddingX)
						{
							PixelFormat::ConvertionTemporaryData oConvertionTempData[2];
							void* pSourceData = (char*)oSliceData.pData + (size_t)(iY / oSrcPFInfos.iBlockHeight * iSrcMipBlockX * oSrcPFInfos.iBlockSize + iX / oSrcPFInfos.iBlockWidth * oSrcPFInfos.iBlockSize);
							void* pNewData = (char*)oNewSliceData.pData + (size_t)(iY / oDstPFInfos.iBlockHeight * iDstMipBlockX * oDstPFInfos.iBlockSize + iX / oDstPFInfos.iBlockWidth * oDstPFInfos.iBlockSize);

							PixelFormatEnum eCurrentFormat = pTexture->GetPixelFormat();
							uint32_t iCurrentBits = PixelFormat::BitPerPixel(eCurrentFormat);
							uint32_t iCurrentPaddingX, iCurrentPaddingY;

							iCurrentPaddingX = PixelFormatEnumInfos[eCurrentFormat].iBlockWidth;
							iCurrentPaddingY = PixelFormatEnumInfos[eCurrentFormat].iBlockHeight;

							for (int iChain = 0; iChain < iConvertionChainLength; ++iChain)
							{
								void* pInputData = (iChain == 0) ? pSourceData : ((iChain % 2 == 0) ? &oConvertionTempData[0] : &oConvertionTempData[1]);
								size_t iInputSize = (iChain == 0) ? (oSliceData.iSize - size_t((char*)pSourceData - (char*)oSliceData.pData)) : sizeof(PixelFormat::ConvertionTemporaryData);
								void* pOutputData = (iChain == (iConvertionChainLength - 1)) ? pNewData : ((iChain % 2 == 0) ? &oConvertionTempData[1] : &oConvertionTempData[0]);
								size_t iOutputSize = (iChain == (iConvertionChainLength - 1)) ? (oNewSliceData.iSize - size_t((char*)pNewData - (char*)oNewSliceData.pData )) : sizeof(PixelFormat::ConvertionTemporaryData);

								uint32_t iInputPitch = (iChain == 0) ? (iSrcMipBlockX * oSrcPFInfos.iBlockWidth) : iPaddingX;
								uint32_t iOutputPitch = (iChain == (iConvertionChainLength - 1)) ? (iDstMipBlockX * oDstPFInfos.iBlockWidth) : iPaddingX;

								PixelFormat::ConvertionFuncInfo oFunc = oConvertionFuncChain[iChain];

								uint32_t iNextBits = PixelFormat::BitPerPixel(oFunc.eFormat);

								uint32_t iNextPaddingX, iNextPaddingY;
								iNextPaddingX = PixelFormatEnumInfos[oFunc.eFormat].iBlockWidth;
								iNextPaddingY = PixelFormatEnumInfos[oFunc.eFormat].iBlockHeight;

								uint32_t iFuncPaddingX = Math::Max(iCurrentPaddingX, iNextPaddingX);
								uint32_t iFuncPaddingY = Math::Max(iCurrentPaddingY, iNextPaddingY);

								int iTW = Math::Min<int>(iPaddingX, iMipWidth - iX);
								int iTH = Math::Min<int>(iPaddingY, iMipHeight - iY);

								for (int iTY = 0; iTY < iTH; iTY += iFuncPaddingY)
								{
									for (int iTX = 0; iTX < iTW; iTX += iFuncPaddingX)
									{
										void* pTempInput = (char*)pInputData + (size_t)(iTY * iInputPitch + iTX) * iCurrentBits / 8;
										void* pTempOutput = (char*)pOutputData + (size_t)(iTY * iOutputPitch + iTX) * iNextBits / 8;

										CORE_ASSERT(pTempInput >= pInputData && pTempInput <= ((char*)pInputData + iInputSize ));
										CORE_ASSERT(pTempOutput >= pOutputData && pTempOutput <= ((char*)pOutputData + iOutputSize));
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

	bool IsPixelFormatResizable(PixelFormatEnum ePixelFormat)
	{
		switch (ePixelFormat)
		{
		case PixelFormatEnum::R8_UNORM:
		case PixelFormatEnum::RG8_UNORM:
		case PixelFormatEnum::RGB8_UNORM:
		case PixelFormatEnum::BGR8_UNORM:
		case PixelFormatEnum::RGBA8_UNORM:
		case PixelFormatEnum::BGRA8_UNORM:
		case PixelFormatEnum::RGB32_FLOAT:
		case PixelFormatEnum::RGBA32_FLOAT:
			return true;
		default:
			return false;
		}
	}

	ErrorCode ResizeTexture(const Texture* pTexture, Texture* pOutTexture, int iNewWidth, int iNewHeight)
	{
		if (pTexture == NULL || pOutTexture == NULL || iNewWidth <= 0 || iNewHeight <= 0)
		{
			return ErrorCode(1, "Invalid argument");
		}

		if (IsPixelFormatResizable(pTexture->GetPixelFormat()) == false)
		{
			return ErrorCode(1, "'%s' Pixel format can't be resized", PixelFormatEnumInfos[pTexture->GetPixelFormat()].pName);
		}

		Texture oTemp;
		Texture::Desc oDesc;
		oDesc.ePixelFormat = pTexture->GetPixelFormat();
		oDesc.iWidth = iNewWidth;
		oDesc.iHeight = iNewHeight;
		oDesc.iMipCount = 1;
		oDesc.iSliceCount = pTexture->GetSliceCount();
		ErrorCode oErr = oTemp.Create(oDesc);
		if (oErr != ErrorCode::Ok)
			return oErr;

		const PixelFormatInfos& oFormatInfos = PixelFormatEnumInfos[pTexture->GetPixelFormat()];

		for (int iFace = 0; iFace < pTexture->GetSliceCount(); ++iFace)
		{
			int iRes;
			const Texture::SliceData oSrcSliceData = pTexture->GetSliceData(0, 0, iFace);
			const Texture::SliceData oDstSliceData = oTemp.GetSliceData(0, 0, iFace);

			if (oFormatInfos.eEncoding == ComponentEncodingEnum::UNORM)
			{
				iRes = stbir_resize_uint8(
					(unsigned char*)oSrcSliceData.pData, oSrcSliceData.iWidth, oSrcSliceData.iHeight, oSrcSliceData.iPitch,
					(unsigned char*)oDstSliceData.pData, oDstSliceData.iWidth, oDstSliceData.iHeight, oDstSliceData.iPitch,
					oFormatInfos.iComponentCount
				);
			}
			else if (oFormatInfos.eEncoding == ComponentEncodingEnum::FLOAT)
			{
				iRes = stbir_resize_float(
					(float*)oSrcSliceData.pData, oSrcSliceData.iWidth, oSrcSliceData.iHeight, oSrcSliceData.iPitch,
					(float*)oDstSliceData.pData, oDstSliceData.iWidth, oDstSliceData.iHeight, oDstSliceData.iPitch,
					oFormatInfos.iComponentCount
				);
			}
			else
			{
				return ErrorCode::NotImplemented;
			}

			if (iRes == 0)
			{
				return ErrorCode(2, "Internal error");
			}
		}

		oTemp.Swap(*pOutTexture);

		return ErrorCode::Ok;
	}

	ErrorCode GenerateMips(const Texture* pTexture, Texture* pOutTexture, uint16_t iMipsMask)
	{
		if (pTexture == NULL || pOutTexture == NULL)
		{
			return ErrorCode(1, "Invalid argument");
		}

		if (IsPixelFormatResizable(pTexture->GetPixelFormat()) == false)
		{
			return ErrorCode(1, "'%s' Pixel format can't be resized", PixelFormatEnumInfos[pTexture->GetPixelFormat()].pName);
		}

		int iSize = Math::Max(pTexture->GetWidth(), pTexture->GetHeight());
		int iMaxMipCount = Math::HighBitLast(iSize);
		int iMipCount = Math::Max(pOutTexture->GetMipCount(), Math::HighBitLast(iMipsMask));
		if (iMipCount > iMaxMipCount)
			iMipCount = iMaxMipCount;

		Texture oTemp;
		Texture::Desc oDesc;
		oDesc.ePixelFormat = pTexture->GetPixelFormat();
		oDesc.iWidth = pTexture->GetWidth();
		oDesc.iHeight = pTexture->GetHeight();
		oDesc.iMipCount = iMipCount;
		oDesc.iSliceCount = pTexture->GetSliceCount();
		ErrorCode oErr = oTemp.Create(oDesc);
		if (oErr != ErrorCode::Ok)
			return oErr;

		const PixelFormatInfos& oFormatInfos = PixelFormatEnumInfos[pTexture->GetPixelFormat()];

		for (int iFace = 0; iFace < pTexture->GetSliceCount(); ++iFace)
		{
			for (int iMip = 0; iMip < iMipCount; ++iMip)
			{
				const Texture::SliceData oDstSliceData = oTemp.GetSliceData(0, iMip, iFace);

				if (iMip == 0 || (((1 << iMip) & iMipsMask) == 0 && iMip < pTexture->GetMipCount()))
				{
					const Texture::SliceData oSrcSliceData = pTexture->GetSliceData(0, iMip, iFace);
					Core::MemCpy(oDstSliceData.pData, oSrcSliceData.pData, oSrcSliceData.iSize);
				}
				else
				{
					const Texture::SliceData oSrcSliceData = oTemp.GetSliceData(0, iMip - 1, iFace);

					int iRes;

					if (oFormatInfos.eEncoding == ComponentEncodingEnum::UNORM)
					{
						iRes = stbir_resize_uint8(
							(unsigned char*)oSrcSliceData.pData, oSrcSliceData.iWidth, oSrcSliceData.iHeight, oSrcSliceData.iPitch,
							(unsigned char*)oDstSliceData.pData, oDstSliceData.iWidth, oDstSliceData.iHeight, oDstSliceData.iPitch,
							oFormatInfos.iComponentCount
						);
					}
					else if (oFormatInfos.eEncoding == ComponentEncodingEnum::FLOAT)
					{
						iRes = stbir_resize_float(
							(float*)oSrcSliceData.pData, oSrcSliceData.iWidth, oSrcSliceData.iHeight, oSrcSliceData.iPitch,
							(float*)oDstSliceData.pData, oDstSliceData.iWidth, oDstSliceData.iHeight, oDstSliceData.iPitch,
							oFormatInfos.iComponentCount
						);
					}
					else
					{
						return ErrorCode::NotImplemented;
					}

					if (iRes == 0)
					{
						return ErrorCode(2, "Internal error");
					}
				}
			}
		}

		oTemp.Swap(*pOutTexture);

		return ErrorCode::Ok;
	}

	ErrorCode GenerateMissingMips(const Texture* pTexture, Texture* pOutTexture)
	{
		if (pTexture == NULL || pOutTexture == NULL || pTexture->IsValid() == false)
			return ErrorCode::InvalidArgument;

		// Generate mip mask
		uint16_t iMissingMipsMask = pTexture->GetMipCount();
		iMissingMipsMask = (0xFFFF >> iMissingMipsMask) << iMissingMipsMask;

		return GenerateMips(pTexture, pOutTexture, iMissingMipsMask);
	}

	int MipLevelDiff(int iWidthA, int iHeightA, int iWidthB, int iHeightB)
	{
		int iMipLevelA = Math::HighBitLast(Math::Max(iWidthA, iHeightA));
		int iMipLevelB = Math::HighBitLast(Math::Max(iWidthB, iHeightB));

		return iMipLevelA - iMipLevelB;
	}

	bool IsSizeRatioEqual(int iWidthA, int iHeightA, int iWidthB, int iHeightB)
	{
		int iMipLevelA = Math::HighBitLast(Math::Max(iWidthA, iHeightA));
		int iMipLevelB = Math::HighBitLast(Math::Max(iWidthB, iHeightB));
		if (iMipLevelA > iMipLevelB)
		{
			return Math::Max(1, (iWidthA >> (iMipLevelA - iMipLevelB))) == iWidthB
				&& Math::Max(1, (iHeightA >> (iMipLevelA - iMipLevelB))) == iHeightB;
		}
		else
		{
			return Math::Max(1, (iWidthB >> (iMipLevelB - iMipLevelA))) == iWidthA
				&& Math::Max(1, (iHeightB >> (iMipLevelB - iMipLevelA))) == iHeightA;
		}
	}

	ErrorCode AssembleTextureMipmap(Texture* pOutTexture, const Texture* pTextures, uint8_t iTextureCount, bool bRegenerateIntermediate)
	{
		PixelFormatEnum ePixelFormat = PixelFormatEnum::_NONE;
		uint32_t iWidth = 0;
		uint32_t iHeight = 0;

		uint8_t iMipLevelMin = Texture::c_iMaxMip;
		uint8_t iMipLevelMax = 0;

		// Check textures specs
		for (int i = 0; i < iTextureCount; ++i)
		{
			const Texture& oTexture = pTextures[i];

			if (oTexture.IsValid() == false)
			{
				//return ErrorCode(1, "Invalid Texture");
				continue;
			}

			if (ePixelFormat == PixelFormatEnum::_NONE)
			{
				ePixelFormat = oTexture.GetPixelFormat();
				iWidth = oTexture.GetWidth();
				iHeight = oTexture.GetHeight();
			}
			else
			{
				if (ePixelFormat != oTexture.GetPixelFormat())
				{
					return ErrorCode(1, "Different pixel format");
				}

				if (IsSizeRatioEqual(iWidth, iHeight, oTexture.GetWidth(), oTexture.GetHeight()) == false)
				{
					return ErrorCode(1, "Different texture size");
				}

				if (iWidth < oTexture.GetWidth())
					iWidth = oTexture.GetWidth();
				if (iHeight < oTexture.GetHeight())
					iHeight = oTexture.GetHeight();
			}

			uint8_t iTextureMipLevelMax = Math::HighBitLast(Math::Max(oTexture.GetWidth(), oTexture.GetHeight()));
			uint8_t iTextureMipLevelMin = iTextureMipLevelMax - (oTexture.GetMipCount());

			if (iMipLevelMax < iTextureMipLevelMax)
				iMipLevelMax = iTextureMipLevelMax;
			if (iMipLevelMin > iTextureMipLevelMin)
				iMipLevelMin = iTextureMipLevelMin;
		}

		if (ePixelFormat == PixelFormatEnum::_NONE)
		{
			return ErrorCode(1, "No valid input texture");
		}

		const uint8_t iMaxMipCount = Math::HighBitLast(Math::Max(iWidth, iHeight));

		Graphics::Texture oAssemblyTexture;
		Graphics::Texture::Desc oDesc;
		oDesc.ePixelFormat = ePixelFormat;
		oDesc.iWidth = iWidth;
		oDesc.iHeight = iHeight;
		oDesc.iMipCount = iMipLevelMax - iMipLevelMin;
		ErrorCode oErr = oAssemblyTexture.Create(oDesc);
		if (oErr != ErrorCode::Ok)
		{
			return ErrorCode(1, "Can't create temp texture");
		}

		uint16_t iImportedMipsMask = 0;
		for (int i = iTextureCount; i > 0; --i)
		{
			const Texture& oTexture = pTextures[i - 1];

			if (oTexture.IsValid() == false)
			{
				continue;
			}

			int iMipDiff = MipLevelDiff(oTexture.GetWidth(), oTexture.GetHeight(), iWidth, iHeight);

			for (int iMip = 0; iMip < oTexture.GetMipCount(); ++iMip)
			{
				uint8_t iDstMip = iMip - iMipDiff;
				uint16_t iMipMask = 1 << iDstMip;

				if ((iImportedMipsMask & iMipMask) != 0)
				{
					// Mip level already copied, skip it
					continue;
				}

				iImportedMipsMask |= iMipMask;

				Graphics::Texture::SliceData oDst = oAssemblyTexture.GetSliceData(0, iDstMip, 0);
				Graphics::Texture::SliceData oSrc = oTexture.GetSliceData(0, iMip, 0);

				Core::MemCpy(oDst.pData, oSrc.pData, oSrc.iSize);
			}
		}

		if (bRegenerateIntermediate)
		{
			uint16_t iIntermediateMips = 0;
			for (int i = 0; i < iTextureCount; ++i)
			{
				const Texture& oTexture = pTextures[i - 1];

				if (oTexture.IsValid() == false)
				{
					continue;
				}

				int iMipDiff = MipLevelDiff(oTexture.GetWidth(), oTexture.GetHeight(), iWidth, iHeight);

				int iMip = oTexture.GetMipCount() - 1;
				uint8_t iDstMip = iMip - iMipDiff;
				uint16_t iMipMask = 1 << (iDstMip);

				iIntermediateMips |= Math::HighBitFill(Texture::c_iMaxMip, iDstMip + 1);
				iIntermediateMips &= ~iMipMask;
			}

			iIntermediateMips &= Math::HighBitFill(oDesc.iMipCount, 0);

			ErrorCode iRes = GenerateMips(&oAssemblyTexture, &oAssemblyTexture, iIntermediateMips);
			if (iRes != ErrorCode::Ok)
			{
				pOutTexture->Swap(oAssemblyTexture);
				return iRes;
			}
		}

		pOutTexture->Swap(oAssemblyTexture);

		return ErrorCode::Ok;
	}
}