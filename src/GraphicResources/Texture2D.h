#ifndef __GRAPHIC_RESOURCE_TEXTURE_DATA_H__
#define __GRAPHIC_RESOURCE_TEXTURE_DATA_H__

#include "Graphics/PixelFormat.h"

#include <DXGIFormat.h>

class ID3D11Texture2D;
class ID3D11ShaderResourceView;

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

		static Texture2D*				CreateFromTexture(Graphics::Texture* pTexture);

		static DXGI_FORMAT				GetDXGIFormatFromPixelFormat(Graphics::EPixelFormat ePixelFormat);
		static Graphics::EPixelFormat	GetPixelFormatFromDXGIFromat(DXGI_FORMAT eDXGIFormat);
	protected:
		ID3D11Texture2D*				m_pTexture;
		ID3D11Texture2D*				m_pStagingTexture;
		ID3D11ShaderResourceView*		m_pTextureView;
	};
}
//namespace GraphicResources

#endif // __GRAPHIC_RESOURCE_TEXTURE_DATA_H__