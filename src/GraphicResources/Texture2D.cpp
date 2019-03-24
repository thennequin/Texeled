
#include "GraphicResources/Texture2D.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

#include "Program.h"

#include <d3dcommon.h>
#include <d3d11.h>

namespace GraphicResources
{
	////////////////////////////////////////////////////////////////
	// Texture2D::LockData
	////////////////////////////////////////////////////////////////

	unsigned char* Texture2D::LockData::GetRow(unsigned int iY)
	{
		return &pData[iRowPitch * iY];
	}

	unsigned char* Texture2D::LockData::GetAt(unsigned int iX, unsigned int iY)
	{
		return &pData[iRowPitch *iY + iX];
	}

	unsigned char* Texture2D::LockData::GetRowSafe(unsigned int iY)
	{
		while (iY < 0) iY += iWidth;
		while (iY >= iHeight) iY -= iWidth;

		return &pData[iRowPitch * iY];
	}

	unsigned char* Texture2D::LockData::GetAtSafe(unsigned int iX, unsigned int iY)
	{
		while (iX < 0) iX += iWidth;
		while (iX >= iWidth) iX -= iWidth;

		while (iY < 0) iY += iWidth;
		while (iY >= iHeight) iY -= iWidth;

		return &pData[iRowPitch *iY + iX];
	}

	////////////////////////////////////////////////////////////////
	// Texture2D
	////////////////////////////////////////////////////////////////

	Texture2D::Texture2D()
	{
		m_pTexture = NULL;
		m_pTextureView = NULL;
	}

	Texture2D::~Texture2D()
	{
		ImwIsSafe(m_pTextureView)->Release();
		ImwIsSafe(m_pTexture)->Release();
	}


	/*Texture2D::LockData Texture2D::Lock()
	{
		//ZeroMemory(&oMappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//Program::GetInstance()->GetDX11DeviceContext()->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubResource);
		//return (unsigned char*)oMappedSubResource.pData;

		D3D11_MAPPED_SUBRESOURCE oMappedSubResource;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_iWidth;
		desc.Height = m_iHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		m_pStagingTexture = NULL;
		HRESULT res = Program::GetInstance()->GetDX11Device()->CreateTexture2D(&desc, NULL, &m_pStagingTexture);

		Program::GetInstance()->GetDX11DeviceContext()->CopyResource(m_pStagingTexture, m_pTexture);

		ZeroMemory(&oMappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Program::GetInstance()->GetDX11DeviceContext()->Map(m_pStagingTexture, 0, D3D11_MAP_READ_WRITE, 0, &oMappedSubResource);

		IM_ASSERT(oMappedSubResource.pData != NULL);

		Texture2D::LockData oLock;
		oLock.pData = (unsigned char*)oMappedSubResource.pData;
		oLock.iRowPitch = oMappedSubResource.RowPitch;
		oLock.iWidth = m_iWidth;
		oLock.iHeight = m_iHeight;
		return oLock;
		//return (unsigned char*)oMappedSubResource.pData;
	}

	void Texture2D::Unlock()
	{
		//Program::GetInstance()->GetDX11DeviceContext()->Unmap(m_pTexture, 0);

		Program::GetInstance()->GetDX11DeviceContext()->Unmap(m_pStagingTexture, 0);
		Program::GetInstance()->GetDX11DeviceContext()->CopyResource(m_pTexture, m_pStagingTexture);
		m_pStagingTexture->Release();
	}*/

	Texture2D* Texture2D::CreateCopy()
	{
		Texture2D* pNew = new Texture2D();
		Program::GetInstance()->GetDX11DeviceContext()->CopyResource(pNew->m_pTexture, m_pTexture);
		return pNew;
	}

	/*bool Texture2D::CopyTo(Texture2D* pTexture)
	{
		if (NULL != pTexture && pTexture->m_iWidth == m_iWidth && pTexture->m_iHeight == m_iHeight)
		{
			Texture2D::LockData oLock = Lock();
			Texture2D::LockData oTexLock = pTexture->Lock();

			for (int i = 0; i < m_iHeight; ++i)
				memcpy(oTexLock.GetRow(i), oLock.GetRow(i), m_iWidth * 4);

			pTexture->Unlock();
			Unlock();
			return true;
		}
		return false;
	}*/

	ErrorCode Texture2D::CreateFromTexture(Graphics::Texture* pTexture, Texture2D** pOutTexture2D)
	{
		ID3D11Texture2D* pDX11Texture = NULL;
		ID3D11ShaderResourceView* pDX11TextureView = NULL;

		if (pTexture == NULL || pOutTexture2D == NULL)
			return ErrorCode(1, "Invalid argument");

		Graphics::PixelFormatEnum ePixelFormat = pTexture->GetPixelFormat();

		DXGI_FORMAT eDXGIFormat;
		Graphics::PixelFormatEnum eNeededConvertionFormat;
		if (GetDXGIFormatFromPixelFormat(ePixelFormat, &eDXGIFormat, &eNeededConvertionFormat) == false || eDXGIFormat == DXGI_FORMAT_UNKNOWN)
			return ErrorCode(1, "Can't convert format");

		Graphics::Texture oNewTexture;
		Graphics::Texture* pInputTexture = pTexture;

		if (eNeededConvertionFormat != ePixelFormat)
		{
			if (Graphics::ConvertPixelFormat(pTexture, &oNewTexture, eNeededConvertionFormat) != ErrorCode::Ok)
			{
				return ErrorCode(1, "Pixel format conversion '%s to '%s' is not supported", Graphics::PixelFormatEnumInfos[ePixelFormat].pName, Graphics::PixelFormatEnumInfos[eNeededConvertionFormat].pName);
			}
			pInputTexture = &oNewTexture;
		}

		Core::Array<D3D11_SUBRESOURCE_DATA> oInitData;
		oInitData.resize(pInputTexture->GetMipCount() * pInputTexture->GetFaceCount(), false);
		for (int iMip = 0, iMipCount = pInputTexture->GetMipCount(); iMip < iMipCount; ++iMip)
		{
			for (int iFace = 0, iFaceCount = pInputTexture->GetFaceCount(); iFace < iFaceCount; ++iFace)
			{
				const Graphics::Texture::TextureFaceData& oFaceData = pInputTexture->GetData().GetFaceData(iMip, iFace);
				int iIndex = iMip * iFaceCount + iFace;
				iIndex = iMip + iFace * iMipCount;
				oInitData[iIndex].pSysMem = oFaceData.pData;
				oInitData[iIndex].SysMemPitch = Graphics::PixelFormat::GetPitch(pInputTexture->GetPixelFormat(), oFaceData.iWidth);
				oInitData[iIndex].SysMemSlicePitch = (UINT)oFaceData.iSize;
			}
		}

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = pInputTexture->GetWidth();
		desc.Height = pInputTexture->GetHeight();
		desc.MipLevels = pInputTexture->GetMipCount();
		desc.ArraySize = pInputTexture->GetFaceCount();
		desc.Format = eDXGIFormat;
		desc.SampleDesc.Count = 1;
		//desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE /*| D3D11_BIND_RENDER_TARGET*/;
		HRESULT hRes = Program::GetInstance()->GetDX11Device()->CreateTexture2D(&desc, oInitData.begin(), &pDX11Texture);
		if (hRes != S_OK)
		{
			char pErrorMsg[512];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pErrorMsg, sizeof(pErrorMsg), NULL);
			return ErrorCode(1, "CreateTexture2D error: %s", pErrorMsg);
		}

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { 0 };
		srvDesc.Format = eDXGIFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ArraySize = pTexture->GetFaceCount();
		srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		hRes = Program::GetInstance()->GetDX11Device()->CreateShaderResourceView(pDX11Texture, &srvDesc, &pDX11TextureView);
		if (hRes != S_OK)
		{
			char pErrorMsg[512];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pErrorMsg, sizeof(pErrorMsg), NULL);
			return ErrorCode(1, "CreateShaderResourceView error: %s", pErrorMsg);
		}

		Texture2D* pNewTexture = new Texture2D();

		pNewTexture->m_pTexture = pDX11Texture;
		pNewTexture->m_pTextureView = pDX11TextureView;
		*pOutTexture2D = pNewTexture;

		return ErrorCode::Ok;
	}

	bool Texture2D::GetDXGIFormatFromPixelFormat(Graphics::PixelFormatEnum ePixelFormat, DXGI_FORMAT* pOutDXGIFormat, Graphics::PixelFormatEnum* pOutConvertionFormatRequired)
	{
		if (pOutDXGIFormat == NULL)
			return false;

		DXGI_FORMAT eDXGIOutput = DXGI_FORMAT_UNKNOWN;
		Graphics::PixelFormatEnum eConvertionFormat = ePixelFormat;

		switch (ePixelFormat)
		{
			case Graphics::PixelFormatEnum::R8_UNORM:
				eDXGIOutput = DXGI_FORMAT_R8_UNORM;
				break;

			case Graphics::PixelFormatEnum::RG8_UNORM:
				eDXGIOutput =  DXGI_FORMAT_R8G8_UNORM;
				break;

			case Graphics::PixelFormatEnum::RGB8_UNORM:
				eDXGIOutput = DXGI_FORMAT_R8G8B8A8_UNORM;
				eConvertionFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
				break;

			case Graphics::PixelFormatEnum::BGR8_UNORM:
				eDXGIOutput =  DXGI_FORMAT_B8G8R8A8_UNORM;
				eConvertionFormat = Graphics::PixelFormatEnum::BGRA8_UNORM;
				break;

			case Graphics::PixelFormatEnum::RGBA8_UNORM:
				eDXGIOutput =  DXGI_FORMAT_R8G8B8A8_UNORM;
				break;

			case Graphics::PixelFormatEnum::BGRA8_UNORM:
				eDXGIOutput =  DXGI_FORMAT_B8G8R8A8_UNORM;
				break;

			case Graphics::PixelFormatEnum::R5G6B5_UNORM:
				eDXGIOutput = DXGI_FORMAT_B5G6R5_UNORM;
				eConvertionFormat = Graphics::PixelFormatEnum::B5G6BR_UNORM;
				break;

			case Graphics::PixelFormatEnum::B5G6BR_UNORM:
				eDXGIOutput = DXGI_FORMAT_B5G6R5_UNORM;
				break;

			case Graphics::PixelFormatEnum::RGB16_FLOAT:
				eDXGIOutput =  DXGI_FORMAT_R16G16B16A16_FLOAT;
				eConvertionFormat = Graphics::PixelFormatEnum::RGBA16_FLOAT;
				break;

			case Graphics::PixelFormatEnum::RGBA16_UNORM:
				eDXGIOutput =  DXGI_FORMAT_R16G16B16A16_UNORM;
				break;

			case Graphics::PixelFormatEnum::RGBA16_FLOAT:
				eDXGIOutput =  DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;

			case Graphics::PixelFormatEnum::RGB32_FLOAT:
				eDXGIOutput =  DXGI_FORMAT_R32G32B32A32_FLOAT;
				eConvertionFormat = Graphics::PixelFormatEnum::RGBA32_FLOAT;
				break;

			case Graphics::PixelFormatEnum::RGBA32_FLOAT:
				eDXGIOutput =  DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;

			case Graphics::PixelFormatEnum::R10G10B10A2_UNORM:
				eDXGIOutput =  DXGI_FORMAT_R10G10B10A2_UNORM;
				break;

			case Graphics::PixelFormatEnum::R9G9B9E5_SHAREDEXP:
				eDXGIOutput =  DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
				break;

			case Graphics::PixelFormatEnum::BC1: // DXT1
				eDXGIOutput =  DXGI_FORMAT_BC1_UNORM;
				break;

			case Graphics::PixelFormatEnum::BC2: // DXT2 / DXT3
				eDXGIOutput =  DXGI_FORMAT_BC2_UNORM;
				break;

			case Graphics::PixelFormatEnum::BC3: // DXT4 / DXT5
				eDXGIOutput =  DXGI_FORMAT_BC3_UNORM;
				break;

			case Graphics::PixelFormatEnum::BC4:
				eDXGIOutput =  DXGI_FORMAT_BC4_UNORM;
				break;

			case Graphics::PixelFormatEnum::BC5:
				*pOutDXGIFormat =  DXGI_FORMAT_BC5_UNORM;
				break;

			case Graphics::PixelFormatEnum::BC6H:
				*pOutDXGIFormat =  DXGI_FORMAT_BC6H_SF16;
				break;

			case Graphics::PixelFormatEnum::BC7:
				*pOutDXGIFormat =  DXGI_FORMAT_BC7_UNORM;
				break;

			default:
				break;
		}


		if (eDXGIOutput == DXGI_FORMAT_UNKNOWN || (eConvertionFormat != eConvertionFormat && pOutConvertionFormatRequired == NULL))
			return false;

		*pOutDXGIFormat = eDXGIOutput;
		if (pOutConvertionFormatRequired != NULL)
			*pOutConvertionFormatRequired = eConvertionFormat;

		return true;
	}
}