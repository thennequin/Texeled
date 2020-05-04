#include "Graphics/Texture.h"

#include "IO/FileStream.h"

#include <stdlib.h> //malloc/free/NULL
#include <string.h> //memcpy/memset
#include <assert.h> //asert
#include <algorithm> //std::swap

using namespace Graphics;

////////////////////////////////////////////////////////////////
// Enums
////////////////////////////////////////////////////////////////

const char* const Texture::EFace_string[_E_FACE_COUNT] = {
	"PosX",
	"NegX",
	"PosY",
	"NegY",
	"PosZ",
	"NegZ",
};

////////////////////////////////////////////////////////////////
// Texture::FaceData
////////////////////////////////////////////////////////////////

Texture::TextureFaceData::TextureFaceData()
{
	pData = CORE_PTR_NULL;
	iSize = 0;
	iPitch = 0;
	iWidth = 0;
	iHeight = 0;
}

////////////////////////////////////////////////////////////////
// Texture::TextureData::Desc
////////////////////////////////////////////////////////////////

Texture::TextureData::Desc::Desc()
{
	ePixelFormat = PixelFormatEnum::_NONE;
	iWidth = 0;
	iHeight = 0;
	iMipCount = 1;
	iFaceCount = 1;
}

////////////////////////////////////////////////////////////////
// Texture::TextureData
////////////////////////////////////////////////////////////////

Texture::TextureData::TextureData()
{
	m_pData = CORE_PTR_NULL;
	m_iSize = 0;
}

Texture::TextureData::~TextureData()
{
	Destroy();
}

ErrorCode Texture::TextureData::Create(Desc& oDesc)
{
	Destroy();

	const PixelFormatInfos& oInfos = PixelFormatEnumInfos[oDesc.ePixelFormat];

	size_t iOffset = 0;
	size_t iOffsets[_E_FACE_COUNT][c_iMaxMip];
	for (int iMipIndex = 0; iMipIndex < oDesc.iMipCount; ++iMipIndex)
	{
		uint32_t iMipWidth = oDesc.iWidth >> iMipIndex;
		iMipWidth = iMipWidth > 0 ? iMipWidth : 1;
		uint32_t iMipHeight = oDesc.iHeight >> iMipIndex;
		iMipHeight = iMipHeight > 0 ? iMipHeight : 1;

		uint32_t iBlockCountX, iBlockCountY;
		PixelFormat::GetBlockCount(oDesc.ePixelFormat, iMipWidth, iMipHeight, &iBlockCountX, &iBlockCountY);

		for (int iFaceIndex = 0; iFaceIndex < oDesc.iFaceCount; ++iFaceIndex)
		{
			size_t iSize = (size_t)iBlockCountX * iBlockCountY * oInfos.iBlockSize;

			m_oFaceData[iFaceIndex][iMipIndex].iWidth = (int)iMipWidth;
			m_oFaceData[iFaceIndex][iMipIndex].iHeight = (int)iMipHeight;
			m_oFaceData[iFaceIndex][iMipIndex].iSize = iSize;
			m_oFaceData[iFaceIndex][iMipIndex].iPitch = (size_t)iBlockCountX * oInfos.iBlockSize;
			iOffsets[iFaceIndex][iMipIndex] = iOffset;
			iOffset += iSize;
		}
	}
	m_iSize = iOffset;

	if (iOffset == 0 || (m_pData = Core::Malloc(iOffset)) == NULL)
	{
		Destroy();
		return ErrorCode::Fail;
	}

	for (int iMipIndex = 0; iMipIndex < oDesc.iMipCount; ++iMipIndex)
	{
		for (int iFaceIndex = 0; iFaceIndex < oDesc.iFaceCount; ++iFaceIndex)
		{
			CORE_PTR(char) pDataChar = (CORE_PTR(char))m_pData;
			m_oFaceData[iFaceIndex][iMipIndex].pData = (pDataChar + iOffsets[iFaceIndex][iMipIndex]);
		}
	}

	return ErrorCode::Ok;
}

void Texture::TextureData::Destroy()
{
	if (m_pData != NULL)
	{
		Core::Free(m_pData);
		m_pData = CORE_PTR_NULL;
		m_iSize = 0;
		for (int iFaceIndex = 0; iFaceIndex < _E_FACE_COUNT; ++iFaceIndex)
		{
			for (int iMipIndex = 0; iMipIndex < c_iMaxMip; ++iMipIndex)
			{
				m_oFaceData[iFaceIndex][iMipIndex] = TextureFaceData();
			}
		}
	}
}

bool Texture::TextureData::IsValid() const
{
	return m_pData != NULL;
}

////////////////////////////////////////////////////////////////
// Texture::Desc
////////////////////////////////////////////////////////////////

Texture::Desc::Desc()
{
	for (int iFaceIndex = 0; iFaceIndex < _E_FACE_COUNT; ++iFaceIndex)
	{
		const void** pDataFace = pData[iFaceIndex];
		for (int iMipIndex = 0; iMipIndex < c_iMaxMip; ++iMipIndex)
		{
			pDataFace[iMipIndex] = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////
// Texture
////////////////////////////////////////////////////////////////

Texture::Texture()
	: m_ePixelFormat(PixelFormatEnum::_NONE)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_iMipCount(0)
	, m_iFaceCount(0)
{
}

Texture::~Texture()
{
	Destroy() == ErrorCode::Ok;
}

bool Texture::IsValid() const
{
	return m_oData.IsValid();
}

ErrorCode Texture::Create(Desc& oDesc)
{
	if (!(oDesc.ePixelFormat != PixelFormatEnum::_NONE
		&& oDesc.iWidth > 0 && oDesc.iWidth <= c_iMaxSize
		&& oDesc.iHeight > 0 && oDesc.iHeight <= c_iMaxSize
		&& oDesc.iMipCount > 0 && oDesc.iMipCount <= c_iMaxMip))
	{
		//Invalid desc
		return ErrorCode(1, "Invalid desc");
	}

	if (m_iMipCount > 1
		&& !(oDesc.iWidth && !(oDesc.iWidth & (oDesc.iWidth - 1)))
		&& !(oDesc.iHeight && !(oDesc.iHeight & (oDesc.iHeight - 1))))
	{
		//Need to be power of 2
		return ErrorCode(1, "Need to be power of 2");
	}

	Destroy() == ErrorCode::Ok;

	if (m_oData.Create(oDesc) != ErrorCode::Ok)
	{
		return ErrorCode(1, "Can't alloc memory");
	}

	m_ePixelFormat = oDesc.ePixelFormat;
	m_iWidth = oDesc.iWidth;
	m_iHeight = oDesc.iHeight;
	m_iFaceCount = oDesc.iFaceCount;
	m_iMipCount = oDesc.iMipCount;

	if (oDesc.pData[0][0] != NULL)
	{
		for (int iMip = 0; iMip < oDesc.iMipCount; ++iMip)
		{
			for (int iFace = 0; iFace < _E_FACE_COUNT; ++iFace)
			{
				const TextureFaceData& oFaceData = m_oData.GetFaceData(iMip, iFace);
				memcpy(oFaceData.pData, oDesc.pData[iMip][iFace], oFaceData.iSize);
			}
		}
	}
	else
	{
		memset(m_oData.GetData(), 0, m_oData.GetDataSize());
	}

	return ErrorCode::Ok;
}

ErrorCode Texture::Destroy()
{
	if (m_oData.IsValid())
	{
		m_oData.Destroy();
		m_ePixelFormat = PixelFormatEnum::_NONE;
		m_iWidth = 0;
		m_iHeight = 0;
		m_iMipCount = 0;
		return ErrorCode::Ok;
	}

	return ErrorCode::Fail;
}

void Texture::Swap(Texture& oOtherTexture)
{
	std::swap(m_iWidth, oOtherTexture.m_iWidth);
	std::swap(m_iHeight, oOtherTexture.m_iHeight);
	std::swap(m_ePixelFormat, oOtherTexture.m_ePixelFormat);
	std::swap(m_iFaceCount, oOtherTexture.m_iFaceCount);
	std::swap(m_iMipCount, oOtherTexture.m_iMipCount);

	std::swap(m_oData.m_pData, oOtherTexture.m_oData.m_pData);
	std::swap(m_oData.m_iSize, oOtherTexture.m_oData.m_iSize);
	for (int iFace = 0; iFace < _E_FACE_COUNT; ++iFace)
	{
		for (int iMip = 0; iMip < c_iMaxMip; ++iMip)
		{
			std::swap(m_oData.m_oFaceData[iFace][iMip], oOtherTexture.m_oData.m_oFaceData[iFace][iMip]);
		}
	}
}

Texture& Texture::operator=(const Texture& /*oTexture*/)
{
	CORE_NOT_IMPLEMENTED();
	return *this;
}
