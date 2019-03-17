#ifndef __GRAPHIC_RESOURCE_SAMPLER_STATE_H__
#define __GRAPHIC_RESOURCE_SAMPLER_STATE_H__

#include "Core/ErrorCode.h"

struct ID3D11SamplerState;

namespace GraphicResources
{
	struct _SamplerFilterEnum
	{
		enum Enum
		{
			NEAREST,
			LINEAR,
			NEAREST_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_NEAREST,
			LINEAR_MIPMAP_LINEAR
		};
	};
	typedef _SamplerFilterEnum::Enum SamplerFilterEnum;

	struct _WrapEnum
	{
		enum Enum
		{
			REPEAT,
			CLAMP_TO_EDGE,
			MIRRORED_REPEAT,
		};
	};
	typedef _WrapEnum::Enum WrapEnum;

	class SamplerState
	{
	public:
		typedef struct
		{
			SamplerFilterEnum	eFilter;
			WrapEnum			eWrapU;
			WrapEnum			eWrapV;
			WrapEnum			eWrapW;
		}Desc;
	public:
		SamplerState();
		~SamplerState();

		ErrorCode				Create(const Desc& oDesc);

		ID3D11SamplerState*		GetResource() { return m_pSamplerState; }
	protected:
		ID3D11SamplerState*		m_pSamplerState;
	};
}
//namespace GraphicResources

#endif // __GRAPHIC_RESOURCE_SAMPLER_STATE_H__