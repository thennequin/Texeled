#ifndef __GRAPHIC_RESOURCE_TEXTURE_DATA_H__
#define __GRAPHIC_RESOURCE_TEXTURE_DATA_H__

#include "Core/ErrorCode.h"

#include "Graphics/PixelFormat.h"

#include <DXGIFormat.h>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace Graphics
{
	class Texture;
}

namespace GraphicResources
{
	class Texture2D
	{
	public:
		struct LockData
		{
			unsigned char*	pData;
			unsigned int	iRowPitch;
			unsigned int	iWidth;
			unsigned int	iHeight;

			unsigned char*	GetRow(int iY);
			unsigned char*	GetAt(int iX, int iY);
			unsigned char*	GetRowSafe(int iY);
			unsigned char*	GetAtSafe(int iX, int iY);
		};
	public:
		Texture2D();
		~Texture2D();

		ID3D11Texture2D*				GetTexture() { return m_pTexture; }
		ID3D11ShaderResourceView*		GetTextureView() { return m_pTextureView; }

		//LockData						Lock();
		//void							Unlock();

		Texture2D*						CreateCopy();

		//bool							CopyTo(Texture2D* pTexture);

		static ErrorCode				CreateFromTexture(Graphics::Texture* pTexture, Texture2D** pOutTexture2D);

		static bool						GetDXGIFormatFromPixelFormat(Graphics::EPixelFormat ePixelFormat, DXGI_FORMAT* pOutDXGIFormat, Graphics::EPixelFormat* pOutConvertionFormatRequired);
		static Graphics::EPixelFormat	GetPixelFormatFromDXGIFromat(DXGI_FORMAT eDXGIFormat);
	protected:
		ID3D11Texture2D*				m_pTexture;
		ID3D11Texture2D*				m_pStagingTexture;
		ID3D11ShaderResourceView*		m_pTextureView;
	};
}
//namespace GraphicResources

#endif // __GRAPHIC_RESOURCE_TEXTURE_DATA_H__