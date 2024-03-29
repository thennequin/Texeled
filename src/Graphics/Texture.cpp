#include "Graphics/Texture.h"

#include "Math/Arithmetic.h"

using namespace Graphics;

////////////////////////////////////////////////////////////////
// Enums
////////////////////////////////////////////////////////////////

const char* const Texture::FaceFlagString[FaceFlag::_MAX_VALUE] = {
	NULL,
	"+X",
	"-X",
	NULL,
	"+Y",
	NULL, NULL, NULL,
	"-Y",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"+Z",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"-Z",
};

Texture::FaceFlag Texture::GetFace(FaceFlags iFaces, int iIndex)
{
	if ((iFaces & (1 << iIndex)) != 0)
		return (FaceFlag)(1 << iIndex);
	return FaceFlag::NONE;
}

uint16_t Texture::GetFaceIndex(FaceFlags iFaces, FaceFlag eFace)
{
	uint16_t iIndex = 0;
	uint8_t iFaceBit = 0;
	while ((iFaceBit = Math::HighBitNext(iFaceBit, iFaces)) != 0)
	{
		if (((1 << (iFaceBit - 1)) & eFace) != 0)
			return iIndex;

		++iIndex;
	}
	return (uint16_t)-1;
}

////////////////////////////////////////////////////////////////
// Texture::SliceData
////////////////////////////////////////////////////////////////

ErrorCode Texture::SliceData::CopyTo(SliceData& oTo)
{
	CORE_TEST_RETURN(ePixelFormat == oTo.ePixelFormat, ErrorCode::InvalidArgument);
	CORE_TEST_RETURN(iWidth == oTo.iWidth, ErrorCode::InvalidArgument);
	CORE_TEST_RETURN(iHeight == oTo.iHeight, ErrorCode::InvalidArgument);
	Core::MemCpy(oTo.pData, pData, iSize);
	return ErrorCode::Ok;
}

CORE_PTR_VOID Texture::SliceData::GetBlockAtCoord(uint16_t iX, uint16_t iY)
{
	CORE_TEST_RETURN(pData != NULL, CORE_PTR_NULL);
	CORE_TEST_RETURN(iX < iWidth && iY < iHeight, CORE_PTR_NULL);

	const PixelFormatInfos& oFormatInfos = PixelFormatEnumInfos[ePixelFormat];

	uint32_t iBlockCountX = ((iWidth + (oFormatInfos.iBlockWidth - 1)) / oFormatInfos.iBlockWidth);
	uint32_t iBlockCountY = ((iHeight + (oFormatInfos.iBlockHeight - 1)) / oFormatInfos.iBlockHeight);

	uint32_t iBlockX = ((iX + (oFormatInfos.iBlockWidth - 1)) / oFormatInfos.iBlockWidth);
	uint32_t iBlockY = ((iY + (oFormatInfos.iBlockHeight - 1)) / oFormatInfos.iBlockHeight);

	return CORE_PTR_CAST_VOID(CORE_PTR_CAST(uint8_t, pData)
		+ iBlockX * oFormatInfos.iBlockSize
		+ iBlockY * iBlockCountX * oFormatInfos.iBlockSize);
}

////////////////////////////////////////////////////////////////
// Texture::ComponentAccessor
////////////////////////////////////////////////////////////////

Texture::ComponentAccessor::ComponentAccessor(SliceData oSliceData, ComponentFlag eComponent)
	: oSliceData(oSliceData)
	, eComponent(eComponent)
{
}

bool Texture::ComponentAccessor::CopyTo(ComponentAccessor& oDest)
{
	CORE_TEST_RETURN(PixelFormat::IsCompressed(oSliceData.ePixelFormat) == false, false);
	CORE_TEST_RETURN(PixelFormat::IsCompressed(oDest.oSliceData.ePixelFormat) == false, false);
	CORE_TEST_RETURN(oSliceData.iWidth == oDest.oSliceData.iWidth, false);
	CORE_TEST_RETURN(oSliceData.iHeight == oDest.oSliceData.iHeight, false);

	const PixelFormatInfos& oSourceInfos = PixelFormatEnumInfos[oSliceData.ePixelFormat];
	const PixelFormatInfos& oDestInfos = PixelFormatEnumInfos[oDest.oSliceData.ePixelFormat];

	CORE_TEST_RETURN(oSourceInfos.eEncoding == oDestInfos.eEncoding, false);
	CORE_TEST_RETURN((oSourceInfos.iComponents & eComponent) != 0, false);
	CORE_TEST_RETURN((oDestInfos.iComponents & oDest.eComponent) != 0, false);

	const uint8_t iSourceComponentIndex = Math::HighBitFirst(eComponent);
	CORE_TEST_RETURN(iSourceComponentIndex != 0, false);

	const uint8_t iDestComponentIndex = Math::HighBitFirst(oDest.eComponent);
	CORE_TEST_RETURN(iDestComponentIndex != 0, false);

	const ComponentMask& oSourceMask = oSourceInfos.iMasks[iSourceComponentIndex - 1];
	const ComponentMask& oDestMask = oDestInfos.iMasks[iDestComponentIndex - 1];

	switch (oSourceInfos.eEncoding)
	{
	case ComponentEncodingEnum::UNORM:
	case ComponentEncodingEnum::INT:
	case ComponentEncodingEnum::UINT:
	case ComponentEncodingEnum::SNORM:
		CORE_TEST_RETURN(oSourceMask.iExpBits == 0, false);
		CORE_TEST_RETURN(oSourceMask.iMantisBits == oDestMask.iMantisBits, false);
		break;
	case ComponentEncodingEnum::FLOAT:
		CORE_TEST_RETURN(oSourceMask.iExpBits == oDestMask.iExpBits, false);
		CORE_TEST_RETURN(oSourceMask.iMantisBits == oDestMask.iMantisBits, false);
		break;
	}
	
	for (uint16_t iY = 0; iY < oSliceData.iHeight; ++iY)
	{
		for (uint16_t iX = 0; iX < oSliceData.iWidth; ++iX)
		{
			CORE_PTR(uint8_t) pSourceData = CORE_PTR_CAST(uint8_t, oSliceData.pData);
			pSourceData += iY * oSliceData.iPitch + iX * oSourceInfos.iBlockSize;
			
			CORE_PTR(uint8_t) pDestData = CORE_PTR_CAST(uint8_t, oDest.oSliceData.pData);
			pDestData += iY * oDest.oSliceData.iPitch + iX * (uint32_t)oDestInfos.iBlockSize;

			uint64_t iSourceValue = 0;
			CORE_PTR(uint64_t) pSourceValue = (CORE_PTR(uint64_t))Core::ToPointer(&iSourceValue, sizeof(iSourceValue));
			Core::MemCpy(pSourceValue, pSourceData, oSourceInfos.iBlockSize);

			uint64_t iDestValue = 0;
			CORE_PTR(uint64_t) pDestValue = (CORE_PTR(uint64_t))Core::ToPointer(&iDestValue, sizeof(iDestValue));
			Core::MemCpy(pDestValue, pDestData, oDestInfos.iBlockSize);

			if (oDestMask.iMantisBits > 0)
			{
				iDestValue = Math::ReplaceBits(iDestValue, iSourceValue, oDestMask.iMantisBits, oDestMask.iMantisShift, oSourceMask.iMantisShift);
			}
			if (oDestMask.iExpBits > 0)
			{
				iDestValue = Math::ReplaceBits(iDestValue, iSourceValue, oDestMask.iExpBits, oDestMask.iExpShift, oSourceMask.iExpShift);
			}

			Core::MemCpy(pDestData, pDestValue, oDestInfos.iBlockSize);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////
// Texture::MipData
////////////////////////////////////////////////////////////////

Texture::SliceData Texture::MipData::GetSliceData(uint16_t iSlice) const
{
	CORE_ASSERT(iSlice >= 0 && iSlice < iSliceCount);

	Texture::SliceData oData;

	oData.ePixelFormat	= ePixelFormat;
	oData.iWidth		= iWidth;
	oData.iHeight		= iHeight;
	oData.eFace			= GetFace(eFaces, iSlice);

	uint16_t iBlockCountX, iBlockCountY;
	PixelFormat::GetBlockCount(ePixelFormat, oData.iWidth, oData.iHeight, &iBlockCountX, &iBlockCountY);

	const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];

	size_t iSlicePitch	= iBlockCountX * oInfos.iBlockSize;
	size_t iSliceSize	= iSlicePitch * iBlockCountY;
	size_t iSliceOffset	= iSliceSize * iSlice;

	oData.iPitch		= iSlicePitch;
	oData.iSize			= iSliceSize;
	oData.pData			= CORE_PTR_CAST(char, pData) + iSliceOffset;

	return oData;
}

////////////////////////////////////////////////////////////////
// Texture::LayerData
////////////////////////////////////////////////////////////////

Texture::MipData Texture::LayerData::GetMipData(uint8_t iMip) const
{
	CORE_ASSERT(iMip >= 0 && iMip < iMipCount);

	Texture::MipData oData;

	oData.ePixelFormat	= ePixelFormat;
	oData.iWidth		= Math::Max<uint16_t>(1, iWidth >> iMip);
	oData.iHeight		= Math::Max<uint16_t>(1, iHeight >> iMip);
	oData.iSliceCount	= iSliceCount;
	oData.eFaces		= eFaces;

	size_t iMipSize, iMipOffset;
	GetMipSizeAndOffset(ePixelFormat, iWidth, iHeight, iSliceCount, iMip, &iMipSize, &iMipOffset);

	oData.iSize			= iMipSize;
	oData.pData			= CORE_PTR_CAST(char, pData) + iMipOffset;

	return oData;
}

Texture::SliceData Texture::LayerData::GetSliceData(uint8_t iMip, uint16_t iSlice) const
{
	CORE_ASSERT(iMip >= 0 && iMip < iMipCount);
	CORE_ASSERT(iSlice >= 0 && iSlice < iSliceCount);

	Texture::SliceData oData;

	oData.ePixelFormat	= ePixelFormat;
	oData.iWidth		= Math::Max<uint16_t>(1, iWidth >> iMip);
	oData.iHeight		= Math::Max<uint16_t>(1, iHeight >> iMip);
	oData.eFace			= GetFace(eFaces, iSlice);

	size_t iMipOffset;
	GetMipSizeAndOffset(ePixelFormat, iWidth, iHeight, iSliceCount, iMip, NULL, &iMipOffset);

	uint16_t iBlockCountX, iBlockCountY;
	PixelFormat::GetBlockCount(ePixelFormat, oData.iWidth, oData.iHeight, &iBlockCountX, &iBlockCountY);

	const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];

	size_t iSlicePitch	= iBlockCountX * oInfos.iBlockSize;
	size_t iSliceSize	= iSlicePitch * iBlockCountY;
	size_t iSliceOffset	= iSliceSize * iSlice;

	oData.iPitch		= iSlicePitch;
	oData.iSize			= iSliceSize;
	oData.pData			= CORE_PTR_CAST(char, pData) + iMipOffset + iSliceOffset;

	return oData;
}

////////////////////////////////////////////////////////////////
// Texture::Desc
////////////////////////////////////////////////////////////////

Texture::Desc::Desc()
{
	ePixelFormat	= PixelFormatEnum::_NONE;
	iWidth			= 0;
	iHeight			= 0;
	iLayerCount		= 1;
	iMipCount		= 1;
	iSliceCount		= 1;
	eFaces			= FaceFlag::NONE;
}

Texture::Desc::Desc(PixelFormatEnum ePixelFormat, uint16_t iWidth, uint16_t iHeight, uint16_t iLayerCount, uint8_t iMipCount, uint16_t iSliceCount, FaceFlags eFaces)
{
	this->iWidth		= iWidth;
	this->iHeight		= iHeight;
	this->ePixelFormat	= ePixelFormat;
	this->iLayerCount	= iLayerCount;
	this->iMipCount		= iMipCount;
	this->iSliceCount	= iSliceCount;
	this->eFaces		= eFaces;
}

bool Texture::Desc::operator ==( const Desc& oRight )
{
	return ePixelFormat == oRight.ePixelFormat
		&& iWidth == oRight.iWidth
		&& iHeight == oRight.iHeight
		&& iLayerCount == oRight.iLayerCount
		&& iMipCount == oRight.iMipCount
		&& iSliceCount == oRight.iSliceCount
		&& eFaces == oRight.eFaces;
}

bool Texture::Desc::operator !=( const Desc& oRight )
{
	return ePixelFormat != oRight.ePixelFormat
		|| iWidth != oRight.iWidth
		|| iHeight != oRight.iHeight
		|| iLayerCount != oRight.iLayerCount
		|| iMipCount != oRight.iMipCount
		|| iSliceCount != oRight.iSliceCount
		|| eFaces != oRight.eFaces;
}

/////////////////////////////////////////////////////////////////
// Texture
////////////////////////////////////////////////////////////////

const Texture::Desc Texture::DescNull(PixelFormatEnum::_NONE, 0, 0, 0, 0, 0, FaceFlag::NONE);

Texture::Texture()
	: m_pData(CORE_PTR_NULL)
	, m_iSize(0)
	, m_iLayerSize(0)
{
	m_oDesc = DescNull;
}

Texture::~Texture()
{
	Destroy() == ErrorCode::Ok;
}

bool Texture::IsValid() const
{
	return m_pData != NULL;
}

ErrorCode Texture::Create(Desc& oDesc)
{
	if (!(oDesc.ePixelFormat != PixelFormatEnum::_NONE
		&& oDesc.iWidth > 0 && oDesc.iWidth <= c_iMaxSize
		&& oDesc.iHeight > 0 && oDesc.iHeight <= c_iMaxSize
		&& oDesc.iLayerCount > 0 && oDesc.iLayerCount <= c_iMaxSize
		&& oDesc.iMipCount > 0 && oDesc.iMipCount <= c_iMaxMip
		&& oDesc.iSliceCount > 0 && oDesc.iSliceCount <= c_iMaxSize))
	{
		//Invalid desc
		return ErrorCode(1, "Invalid desc");
	}

	if (m_oDesc.iMipCount > 1
		&& !(oDesc.iWidth && !(oDesc.iWidth & (oDesc.iWidth - 1)))
		&& !(oDesc.iHeight && !(oDesc.iHeight & (oDesc.iHeight - 1))))
	{
		//Need to be power of 2
		return ErrorCode(1, "Need to be power of 2");
	}

	uint8_t iFaceCount = Math::HighBitCount(oDesc.eFaces);

	if (iFaceCount > 0 && oDesc.iSliceCount != iFaceCount)
	{
		// When eFaces setted, iSliceCount must be identical to eFaces count
		return ErrorCode(1, "Slice count must be identical to eFaces count");
	}

	Destroy() == ErrorCode::Ok;

	{
		const PixelFormatInfos& oInfos = PixelFormatEnumInfos[oDesc.ePixelFormat];

		size_t iSize = 0;
		for (int iMipIndex = 0; iMipIndex < oDesc.iMipCount; ++iMipIndex)
		{
			size_t iMipSize;
			GetMipSizeAndOffset(oDesc.ePixelFormat, oDesc.iWidth, oDesc.iHeight, oDesc.iSliceCount, iMipIndex, &iMipSize, NULL);
			iSize += iMipSize;
		}
		m_iSize = iSize * oDesc.iLayerCount;

		if (m_iSize == 0 || (m_pData = Core::Malloc(m_iSize)) == NULL)
		{
			Destroy();
			return ErrorCode(1, "Can't alloc memory");
		}
	}

	m_oDesc = oDesc;

	Core::MemZero(m_pData, m_iSize);

	return ErrorCode::Ok;
}

void Texture::GetMipSizeAndOffset(PixelFormatEnum ePixelFormat, uint16_t iWidth, uint16_t iHeight, uint16_t iSliceCount, uint8_t iMip, size_t* pOutMipSize, size_t* pOutMipOffset)
{
	CORE_ASSERT(pOutMipSize != NULL || pOutMipOffset != NULL);

	const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];

	size_t iMipOffset = 0;
	size_t iMipSize = 0;
	for (int iMipIndex = 0; iMipIndex <= iMip; ++iMipIndex)
	{
		uint16_t iMipWidth = Math::Max<uint16_t>(1, iWidth >> iMipIndex);
		uint16_t iMipHeight = Math::Max<uint16_t>(1, iHeight >> iMipIndex);

		uint16_t iBlockCountX, iBlockCountY;
		PixelFormat::GetBlockCount(ePixelFormat, iMipWidth, iMipHeight, &iBlockCountX, &iBlockCountY);

		iMipSize = iSliceCount * iBlockCountX * iBlockCountY * oInfos.iBlockSize;

		if (iMipIndex < iMip)
			iMipOffset += iMipSize;
	}

	if (pOutMipSize != NULL)
		*pOutMipSize = iMipSize;
	if (pOutMipOffset != NULL)
		*pOutMipOffset = iMipOffset;
}

Texture::LayerData Texture::GetLayerData(uint16_t iLayer) const
{
	CORE_ASSERT(iLayer >= 0 && iLayer < m_oDesc.iLayerCount);

	Texture::LayerData oData;

	oData.ePixelFormat	= m_oDesc.ePixelFormat;
	oData.iWidth		= m_oDesc.iWidth;
	oData.iHeight		= m_oDesc.iHeight;
	oData.iMipCount		= m_oDesc.iMipCount;
	oData.iSliceCount	= m_oDesc.iSliceCount;
	oData.eFaces		= m_oDesc.eFaces;

	size_t iLayerOffset = m_iLayerSize * iLayer;

	oData.iSize			= m_iLayerSize;
	oData.pData			= CORE_PTR_CAST(char, m_pData) + iLayerOffset;

	return oData;
}

Texture::MipData Texture::GetMipData(uint16_t iLayer, uint8_t iMip) const
{
	return GetLayerData(iLayer).GetMipData(iMip);
}

Texture::SliceData Texture::GetSliceData(uint16_t iLayer, uint8_t iMip, uint16_t iSlice) const
{
	return GetLayerData(iLayer).GetMipData(iMip).GetSliceData(iSlice);
}

ErrorCode Texture::Destroy()
{
	m_oDesc = DescNull;

	if (m_pData != NULL)
	{
		Core::Free(m_pData);
		m_pData = CORE_PTR_NULL;
	}

	m_iSize			= 0;
	m_iLayerSize	= 0;

	return ErrorCode::Ok;
}

void Texture::Swap(Texture& oOtherTexture)
{
	{
		Desc oTemp = oOtherTexture.m_oDesc;
		oOtherTexture.m_oDesc = m_oDesc;
		m_oDesc = oTemp;
	}
	{
		CORE_PTR_VOID oTemp = oOtherTexture.m_pData;
		oOtherTexture.m_pData = m_pData;
		m_pData = oTemp;
	}
	{
		size_t oTemp = oOtherTexture.m_iSize;
		oOtherTexture.m_iSize = m_iSize;
		m_iSize = oTemp;
	}
}

Texture& Texture::operator=(const Texture& oTexture)
{
	Destroy();
	if (oTexture.IsValid())
	{
		Desc oDesc = oTexture.GetDesc();
		CORE_VERIFY(Create(oDesc) == ErrorCode::Ok);
		Core::MemCpy(m_pData, oTexture.m_pData, m_iSize);
	}
	return *this;
}
