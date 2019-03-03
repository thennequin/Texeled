
#include "GraphicResources/Texture2D.h"

#include "Graphics/Texture.h"

#include "Program.h"

#include <d3dcommon.h>
#include <d3d11.h>

namespace GraphicResources
{
	////////////////////////////////////////////////////////////////
	// Texture2D::LockData
	////////////////////////////////////////////////////////////////

	unsigned char* Texture2D::LockData::GetRow(int iY)
	{
		return &pData[iRowPitch * iY];
	}

	unsigned char* Texture2D::LockData::GetAt(int iX, int iY)
	{
		return &pData[iRowPitch *iY + iX];
	}

	unsigned char* Texture2D::LockData::GetRowSafe(int iY)
	{
		while (iY < 0) iY += iWidth;
		while (iY >= iHeight) iY -= iWidth;

		return &pData[iRowPitch * iY];
	}

	unsigned char* Texture2D::LockData::GetAtSafe(int iX, int iY)
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

	Texture2D* Texture2D::CreateFromTexture(Graphics::Texture* pTexture)
	{
		ID3D11Texture2D* pDX11Texture = NULL;
		ID3D11ShaderResourceView* pDX11TextureView = NULL;

		if (pTexture == NULL)
			return NULL;

		DXGI_FORMAT eDXGIFormat = GetDXGIFormatFromPixelFormat(pTexture->GetPixelFormat());

		if (eDXGIFormat == DXGI_FORMAT_UNKNOWN)
			return NULL;

		D3D11_SUBRESOURCE_DATA oInitData;
		oInitData.pSysMem = pTexture->GetData().GetData();
		oInitData.SysMemPitch = Graphics::PixelFormat::BitPerPixel(pTexture->GetPixelFormat()) / 8 * pTexture->GetWidth();
		oInitData.SysMemSlicePitch = 0;


		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = pTexture->GetWidth();
		desc.Height = pTexture->GetHeight();
		desc.MipLevels = pTexture->GetMipCount();
		desc.ArraySize = 1;
		desc.Format = eDXGIFormat;
		desc.SampleDesc.Count = 1;
		//desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		Program::GetInstance()->GetDX11Device()->CreateTexture2D(&desc, &oInitData, &pDX11Texture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = eDXGIFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Program::GetInstance()->GetDX11Device()->CreateShaderResourceView(pDX11Texture, &srvDesc, &pDX11TextureView);

		/*D3D11_MAPPED_SUBRESOURCE oMappedSubResource;
		ZeroMemory(&oMappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hRes = Program::GetInstance()->GetDX11DeviceContext()->Map(pDX11Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubResource); //TODO TEST result

		IM_ASSERT(oMappedSubResource.pData != NULL);

		Program::GetInstance()->GetDX11DeviceContext()->Unmap(pDX11Texture, 0);*/

		Texture2D* pNewTexture = new Texture2D();

		pNewTexture->m_pTexture = pDX11Texture;
		pNewTexture->m_pTextureView = pDX11TextureView;

		return pNewTexture;
	}

	DXGI_FORMAT Texture2D::GetDXGIFormatFromPixelFormat(Graphics::EPixelFormat ePixelFormat)
	{
		switch (ePixelFormat)
		{
			case Graphics::E_PIXELFORMAT_R8_UNORM:
				return DXGI_FORMAT_R8_UNORM;

			case Graphics::E_PIXELFORMAT_RG8_UNORM:
				return DXGI_FORMAT_R8G8_UNORM;

			//case Graphics::E_PIXELFORMAT_RGB8_UNORM:
				//return DXGI_FORMAT_R8G8B8_UNORM

			//case Graphics::E_PIXELFORMAT_BGR8_UNORM:
				//return DXGI_FORMAT_B8G8R8_UNORM;

			case Graphics::E_PIXELFORMAT_RGBA8_UNORM:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Graphics::E_PIXELFORMAT_BGRA8_UNORM:
				return DXGI_FORMAT_B8G8R8A8_UNORM;

			//case Graphics::E_PIXELFORMAT_R5G6B5_UNORM:
				//return DXGI_FORMAT_R5G6B5_UNORM;
			//case Graphics::E_PIXELFORMAT_B5G6BR_UNORM:
				//return DXGI_FORMAT_B5G6BR_UNORM;

			//case Graphics::E_PIXELFORMAT_RGB16_SFLOAT:
				//return DXGI_FORMAT_R16G16B16_SFLOAT;

			case Graphics::E_PIXELFORMAT_RGBA16_UNORM:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Graphics::E_PIXELFORMAT_RGBA16_SFLOAT:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case Graphics::E_PIXELFORMAT_RGB32_SFLOAT:
				//return DXGI_FORMAT_R32G32B32_SFLOAT;
			case Graphics::E_PIXELFORMAT_RGBA32_SFLOAT:
				return DXGI_FORMAT_R32G32B32A32_FLOAT; //TODO

			case Graphics::E_PIXELFORMAT_R10G10B10A2_UNORM:
				return DXGI_FORMAT_R10G10B10A2_UNORM;

			case Graphics::E_PIXELFORMAT_R9G9B9E5_SHAREDEXP:
				return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;

			case Graphics::E_PIXELFORMAT_BC1: // DXT1
				return DXGI_FORMAT_BC1_UNORM; // DXT;
			case Graphics::E_PIXELFORMAT_BC2: // DXT2 / DXT3
				return DXGI_FORMAT_BC2_UNORM; // DXT2 / DXT
			case Graphics::E_PIXELFORMAT_BC3: // DXT4 / DXT5
				return DXGI_FORMAT_BC3_UNORM; // DXT4 / DXT
			case Graphics::E_PIXELFORMAT_BC4:
				return DXGI_FORMAT_BC4_UNORM;
			/*case Graphics::E_PIXELFORMAT_BC5:
				return DXGI_FORMAT_BC5_UNORM;
			case Graphics::E_PIXELFORMAT_BC6H:
				return DXGI_FORMAT_BC6H_SF16;
			case Graphics::E_PIXELFORMAT_BC7:
				return DXGI_FORMAT_BC7_UNORM;*/

			default:
				return DXGI_FORMAT_UNKNOWN;
		}
	}

	Graphics::EPixelFormat Texture2D::GetPixelFormatFromDXGIFromat(DXGI_FORMAT eDXGIFormat)
	{
		return Graphics::E_PIXELFORMAT_NONE;
	}
}