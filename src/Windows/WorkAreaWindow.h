#ifndef __WINDOWS_WORK_AREA_WINDOW_H__
#define __WINDOWS_WORK_AREA_WINDOW_H__

#include "ImWindow/ImwWindow.h"

#include "Program.h"

#include "GraphicResources/ShaderUtils.h"

struct ID3D11PixelShader;
struct ID3D11Buffer;

namespace GraphicResources
{
	class Texture2D;
	class SamplerState;
}

namespace Windows
{
	class WorkAreaWindow : public ImWindow::ImwWindow
	{
		struct BufferData
		{
			float				fChannelsMix[4][4];
			float				fChannelsAdd[4];
			int					iMip;
			int					iFace;
			float				fRange[2];
			float				fGamma;

			void				BuildChannelMix(ChannelFlags eShowChannel);
		};
	public:
		WorkAreaWindow();
		virtual ~WorkAreaWindow();

		virtual void			OnGui();
		virtual void			OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos) override;
	protected:
		double					m_fZoom;
		ImVec2					m_oOffset;

		double					m_fCurrentZoom;
		ImVec2					m_oCurrentOffset;

		GraphicResources::Texture2D*				m_pCheckboardTexture2DRes;
		GraphicResources::SamplerState*				m_pSamplerStatePointRepeat;
		GraphicResources::SamplerState*				m_pSamplerStatePointClamp;
		GraphicResources::SamplerState*				m_pSamplerStateLinearRepeat;
		GraphicResources::SamplerState*				m_pSamplerStateLinearClamp;
		ID3D11PixelShader*							m_pPixelShader;
		GraphicResources::ConstantBufferDesc		m_oGlobalConstantBufferDesc;
		ID3D11Buffer*								m_pGlobalConstantBuffer;
		BufferData									m_oBufferData;
		ChannelFlags								m_eCurrentShowChannels;
	};
}
//namespace Windows

#endif //__WINDOWS_WORK_AREA_WINDOW_H__