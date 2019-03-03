#include "Graphics/Texture.h"

#include "Core/FileStream.h"

#include <stdlib.h> //malloc/free/NULL
#include <string.h> //memcpy/memset
#include <assert.h> //asert

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
	pData = NULL;
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
	ePixelFormat = E_PIXELFORMAT_NONE;
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
	m_pData = NULL;
	m_iSize = 0;
}

Texture::TextureData::~TextureData()
{
	Destroy();
}

ErrorCode Texture::TextureData::Create(Desc& oDesc)
{
	Destroy();

	size_t iOffset = 0;
	size_t iBits = PixelFormat::BitPerPixel(oDesc.ePixelFormat);
	for (int iMipIndex = 0; iMipIndex < oDesc.iMipCount; ++iMipIndex)
	{
		size_t iMipWidth = oDesc.iWidth >> iMipIndex;
		iMipWidth = iMipWidth > 0 ? iMipWidth : 1;
		size_t iMipHeight = oDesc.iHeight >> iMipIndex;
		iMipHeight = iMipHeight > 0 ? iMipHeight : 1;
		size_t iPadMipWidth;
		size_t iPadMipHeight;
		
		if (PixelFormat::IsCompressed(oDesc.ePixelFormat))
		{
			iPadMipWidth = ((iMipWidth + 3) / 4) * 4;
			iPadMipHeight = ((iMipHeight + 3) / 4) * 4;
		}
		else
		{
			iPadMipWidth = iMipWidth;
			iPadMipHeight = iMipHeight;
		}

		for (int iFaceIndex = 0; iFaceIndex < oDesc.iFaceCount; ++iFaceIndex)
		{
			size_t iSize = iPadMipWidth * iPadMipHeight * iBits / 8;
			
			m_oFaceData[iFaceIndex][iMipIndex].iWidth = (int)iMipWidth;
			m_oFaceData[iFaceIndex][iMipIndex].iHeight = (int)iMipHeight;
			m_oFaceData[iFaceIndex][iMipIndex].iSize = iSize;
			m_oFaceData[iFaceIndex][iMipIndex].iPitch = iPadMipWidth * iBits / 8;
			m_oFaceData[iFaceIndex][iMipIndex].pData = (void*)iOffset;
			iOffset += iSize;
		}
	}
	m_iSize = iOffset;

	if (iOffset == 0 || (m_pData = malloc(iOffset)) == NULL)
	{
		Destroy();
		return ErrorCode::Fail;
	}

	for (int iMipIndex = 0; iMipIndex < oDesc.iMipCount; ++iMipIndex)
	{
		for (int iFaceIndex = 0; iFaceIndex < oDesc.iFaceCount; ++iFaceIndex)
		{
			m_oFaceData[iFaceIndex][iMipIndex].pData = (void*)((size_t)m_pData + (size_t)m_oFaceData[iFaceIndex][iMipIndex].pData);
		}
	}

	return ErrorCode::Ok;
}

void Texture::TextureData::Destroy()
{
	if (m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
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
	: m_ePixelFormat(E_PIXELFORMAT_NONE)
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
	if (!(oDesc.ePixelFormat != E_PIXELFORMAT_NONE
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
		m_ePixelFormat = E_PIXELFORMAT_NONE;
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

Texture& Texture::operator=(const Texture& oTexture)
{
	//TODO
	return *this;
}
