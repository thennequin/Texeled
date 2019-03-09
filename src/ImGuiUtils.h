
namespace GraphicResources
{
	class SamplerState;
}

namespace ImGuiUtils
{
	void SetSampler(GraphicResources::SamplerState* pSampler);
	void PushSampler(GraphicResources::SamplerState* pSampler);
	void PopSampler();
}
//namespace ImGuiUtils