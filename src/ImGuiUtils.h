
namespace GraphicResources
{
	class SamplerState;
}

namespace ImGuiUtils
{
	void SetSampler(GraphicResources::SamplerState* pSampler);
	void PushSampler(GraphicResources::SamplerState* pSampler);
	void PopSampler();

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
}
//namespace ImGuiUtils