#include "GraphicResources/SamplerState.h"

#include "Program.h"

#include <D3D11.h>

namespace GraphicResources
{
	D3D11_FILTER ConvertFilter(SamplerFilterEnum eFilter)
	{
		switch (eFilter)
		{
		case SamplerFilter::NEAREST:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case SamplerFilter::LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case SamplerFilter::NEAREST_MIPMAP_NEAREST:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case SamplerFilter::NEAREST_MIPMAP_LINEAR:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case SamplerFilter::LINEAR_MIPMAP_NEAREST:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case SamplerFilter::LINEAR_MIPMAP_LINEAR:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}


	D3D11_TEXTURE_ADDRESS_MODE ConvertWrap(WrapEnum eWrap)
	{
		switch (eWrap)
		{
		case Wrap::REPEAT:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case Wrap::CLAMP_TO_EDGE:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case Wrap::MIRRORED_REPEAT:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		}
	}

	SamplerState::SamplerState()
		: m_pSamplerState(NULL)
	{
	}

	SamplerState::~SamplerState()
	{
		if (m_pSamplerState != NULL)
			m_pSamplerState->Release();
	}

	ErrorCode SamplerState::Create(const Desc& oDesc)
	{
		if (m_pSamplerState != NULL)
			m_pSamplerState->Release();

		// Create texture sampler
		D3D11_SAMPLER_DESC oSamplerDesc;
		ZeroMemory(&oSamplerDesc, sizeof(oSamplerDesc));
		oSamplerDesc.Filter = ConvertFilter(oDesc.eFilter);
		oSamplerDesc.AddressU = ConvertWrap(oDesc.eWrapU);
		oSamplerDesc.AddressV = ConvertWrap(oDesc.eWrapV);
		oSamplerDesc.AddressW = ConvertWrap(oDesc.eWrapW);
		oSamplerDesc.MipLODBias = 0.f;
		oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		oSamplerDesc.MinLOD = 0.f;
		oSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		if (Program::GetInstance()->GetDX11Device()->CreateSamplerState(&oSamplerDesc, &m_pSamplerState) == S_OK)
		{
			return ErrorCode::Ok;
		}

		return ErrorCode::Fail;
	}
}
