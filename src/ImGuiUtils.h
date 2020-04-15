#ifndef _IMGUI_UTILS_H_
#define _IMGUI_UTILS_H_

struct ID3D11PixelShader;
struct ID3D11Buffer;
namespace GraphicResources
{
	class SamplerState;
	class Shader;
}

namespace ImGuiUtils
{
	void PushSampler(GraphicResources::SamplerState* pSampler);
	void PopSampler();

	void PushPixelShader(ID3D11PixelShader* pShader);
	void PopPixelShader();

	void PushPixelShaderConstantBuffer(ID3D11Buffer* pBuffer);
	void PopPixelShaderConstantBuffer();

	struct GroupedPart
	{
		enum Enum
		{
			LEFT,
			MIDDLE,
			RIGHT
		};
	};
	typedef GroupedPart::Enum GroupedPartEnum;

	bool GroupedButton( const char* label, bool bSelected, GroupedPartEnum ePart );

	void TextPathWrapped(const char* pPath);
}
//namespace ImGuiUtils

#endif //_IMGUI_UTILS_H_