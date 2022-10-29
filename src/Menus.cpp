#define IMGUI_DEFINE_MATH_OPERATORS
#include "Menus.h"

#include "Program.h"
#include "ShortKeyManager.h"

#include "Math/Arithmetic.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

#include "Windows/AboutWindow.h"
#include "Windows/LoggerWindow.h"

#include "IO/MemoryStream.h"
#include "IO/Clipboard.h"

#include "ImGuiUtils.h"

#include "Resources/Icons/New_16_png.h"
#include "Resources/Icons/Open_16_png.h"
#include "Resources/Icons/Save_16_png.h"
#include "Resources/Icons/SaveAs_16_png.h"
#include "Resources/Icons/Previous_16_png.h"
#include "Resources/Icons/Next_16_png.h"
#include "Resources/Icons/Exit_16_png.h"
#include "Resources/Icons/Convert_16_png.h"
#include "Resources/Icons/Resize_16_png.h"
#include "Resources/Icons/MipMap_16_png.h"
#include "Resources/Icons/MissingMipMap_16_png.h"
#include "Resources/Icons/CurrentMipMap_16_png.h"
#include "Resources/Icons/Cut_16_png.h"
#include "Resources/Icons/Copy_16_png.h"
#include "Resources/Icons/Paste_16_png.h"
#include "Resources/Icons/Log_16_png.h"
#include "Resources/Icons/Help_16_png.h"

Menus::Menus()
	: ImWindow::ImwMenu()
	, m_bResizeKeepRatio(false)
	, m_iResizeNewWidth(0)
	, m_iResizeNewHeight(0)
	, m_fResizeRatio(0.f)
	, m_pIconNew(NULL)
	, m_pIconOpen(NULL)
	, m_pIconSave(NULL)
	, m_pIconSaveAs( NULL )
	, m_pIconPrevious(NULL)
	, m_pIconNext(NULL)
	, m_pIconExit(NULL)
	, m_pIconConvert(NULL)
	, m_pIconResize(NULL)
	, m_pIconMipMap(NULL)
	, m_pIconMissingMipMap(NULL)
	, m_pIconCurrentMipMap(NULL)
	, m_pIconCut(NULL)
	, m_pIconCopy(NULL)
	, m_pIconPaste(NULL)
	, m_pIconLog(NULL)
	, m_pIconHelp(NULL)
{

	//Load icons
	Graphics::Texture oTexture;

	// New
	{
		IO::MemoryStream oMemStream(Resources::Icons::New_16_png::Data, Resources::Icons::New_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconNew));
		}
	}
	// Open
	{
		IO::MemoryStream oMemStream(Resources::Icons::Open_16_png::Data, Resources::Icons::Open_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconOpen));
		}
	}
	// Save
	{
		IO::MemoryStream oMemStream(Resources::Icons::Save_16_png::Data, Resources::Icons::Save_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconSave));
		}
	}
	// SaveAs
	{
		IO::MemoryStream oMemStream( Resources::Icons::SaveAs_16_png::Data, Resources::Icons::SaveAs_16_png::Size );
		CORE_VERIFY( Texture::LoadFromStream( &oTexture, &oMemStream ) == ErrorCode::Ok );
		if( oTexture.IsValid() )
		{
			CORE_VERIFY_OK( GraphicResources::Texture2D::CreateFromTexture( &oTexture, &m_pIconSaveAs ) );
		}
	}
	// Previous
	{
		IO::MemoryStream oMemStream(Resources::Icons::Previous_16_png::Data, Resources::Icons::Previous_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconPrevious));
		}
	}
	// Next
	{
		IO::MemoryStream oMemStream(Resources::Icons::Next_16_png::Data, Resources::Icons::Next_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconNext));
		}
	}
	// Exit
	{
		IO::MemoryStream oMemStream(Resources::Icons::Exit_16_png::Data, Resources::Icons::Exit_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconExit));
		}
	}
	// Convert
	{
		IO::MemoryStream oMemStream(Resources::Icons::Convert_16_png::Data, Resources::Icons::Convert_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconConvert));
		}
	}
	// Resize
	{
		IO::MemoryStream oMemStream(Resources::Icons::Resize_16_png::Data, Resources::Icons::Resize_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconResize));
		}
	}
	// MipMap
	{
		IO::MemoryStream oMemStream(Resources::Icons::MipMap_16_png::Data, Resources::Icons::MipMap_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconMipMap));
		}
	}
	// MissingMipMap
	{
		IO::MemoryStream oMemStream(Resources::Icons::MissingMipMap_16_png::Data, Resources::Icons::MissingMipMap_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconMissingMipMap));
		}
	}
	// CurrentMipMap
	{
		IO::MemoryStream oMemStream(Resources::Icons::CurrentMipMap_16_png::Data, Resources::Icons::CurrentMipMap_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconCurrentMipMap));
		}
	}
	// Cut
	{
		IO::MemoryStream oMemStream(Resources::Icons::Cut_16_png::Data, Resources::Icons::Cut_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconCut));
		}
	}
	// Copy
	{
		IO::MemoryStream oMemStream(Resources::Icons::Copy_16_png::Data, Resources::Icons::Copy_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconCopy));
		}
	}
	// Paste
	{
		IO::MemoryStream oMemStream(Resources::Icons::Paste_16_png::Data, Resources::Icons::Paste_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconPaste));
		}
	}
	// Log
	{
		IO::MemoryStream oMemStream(Resources::Icons::Log_16_png::Data, Resources::Icons::Log_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconLog));
		}
	}
	// Help
	{
		IO::MemoryStream oMemStream(Resources::Icons::Help_16_png::Data, Resources::Icons::Help_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconHelp));
		}
	}

}

Menus::~Menus()
{
	if (m_pIconNew != NULL)
	{
		delete m_pIconNew;
		m_pIconNew = NULL;
	}

	if (m_pIconOpen != NULL)
	{
		delete m_pIconOpen;
		m_pIconOpen = NULL;
	}

	if (m_pIconSave != NULL)
	{
		delete m_pIconSave;
		m_pIconSave = NULL;
	}

	if( m_pIconSaveAs != NULL )
	{
		delete m_pIconSaveAs;
		m_pIconSaveAs = NULL;
	}

	if (m_pIconPrevious != NULL)
	{
		delete m_pIconPrevious;
		m_pIconPrevious = NULL;
	}

	if (m_pIconNext != NULL)
	{
		delete m_pIconNext;
		m_pIconNext = NULL;
	}

	if (m_pIconExit != NULL)
	{
		delete m_pIconExit;
		m_pIconExit = NULL;
	}

	if (m_pIconConvert != NULL)
	{
		delete m_pIconConvert;
		m_pIconConvert = NULL;
	}

	if (m_pIconResize != NULL)
	{
		delete m_pIconResize;
		m_pIconResize = NULL;
	}

	if (m_pIconMipMap != NULL)
	{
		delete m_pIconMipMap;
		m_pIconMipMap = NULL;
	}

	if (m_pIconMissingMipMap != NULL)
	{
		delete m_pIconMissingMipMap;
		m_pIconMissingMipMap = NULL;
	}

	if (m_pIconCurrentMipMap != NULL)
	{
		delete m_pIconCurrentMipMap;
		m_pIconCurrentMipMap = NULL;
	}

	if (m_pIconCut != NULL)
	{
		delete m_pIconCut;
		m_pIconCut = NULL;
	}

	if (m_pIconCopy != NULL)
	{
		delete m_pIconCopy;
		m_pIconCopy = NULL;
	}

	if (m_pIconPaste != NULL)
	{
		delete m_pIconPaste;
		m_pIconPaste = NULL;
	}

	if (m_pIconLog != NULL)
	{
		delete m_pIconLog;
		m_pIconLog = NULL;
	}

	if (m_pIconHelp != NULL)
	{
		delete m_pIconHelp;
		m_pIconHelp = NULL;
	}
}

void Menus::OnMenu()
{
	Program* pProgram = Program::GetInstance();
	const DisplayOptions& oDisplayOptions = pProgram->GetDisplayOptions();
	Graphics::Texture& oTexture = pProgram->GetTexture();
	const Shortkeys& oShortkeys = pProgram->GetShortkeys();
	const Fonts& oFonts = pProgram->GetFonts();

	bool bOpenNewMenu = false;

	if (ImGui::BeginMenu("File"))
	{
		if (ImGuiUtils::MenuItemPlus("New", NULL, oShortkeys.pOpen->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true, (ImTextureID)m_pIconNew->GetTextureView()))
		{
			bOpenNewMenu = true;
		}
		if (ImGuiUtils::MenuItemPlus("Open", NULL, oShortkeys.pOpen->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true, (ImTextureID)m_pIconOpen->GetTextureView()))
		{
			pProgram->Open();
		}
		if (ImGuiUtils::MenuItemPlus("Save", NULL, oShortkeys.pSave->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid(), (ImTextureID)m_pIconSave->GetTextureView()))
		{
			pProgram->Save();
		}
		if (ImGuiUtils::MenuItemPlus("Save as", NULL, oShortkeys.pSaveAs->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid(), (ImTextureID)m_pIconSaveAs->GetTextureView() ))
		{
			pProgram->SaveAs();
		}
		ImGui::Separator();
		if (ImGuiUtils::MenuItemPlus("Open previous file", NULL, oShortkeys.pOpenPreviousFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath().empty() == false, (ImTextureID)m_pIconPrevious->GetTextureView()))
		{
			pProgram->OpenPreviousFile();
		}
		if (ImGuiUtils::MenuItemPlus("Open next file", NULL, oShortkeys.pOpenNextFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath().empty() == false, (ImTextureID)m_pIconNext->GetTextureView()))
		{
			pProgram->OpenNextFile();
		}
		ImGui::Separator();
		if (ImGuiUtils::MenuItemPlus("Exit", NULL, oShortkeys.pClose->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true, (ImTextureID)m_pIconExit->GetTextureView()))
		{
			pProgram->AskExit();
		}
		ImGui::EndMenu();
	}

	if (bOpenNewMenu)
	{
		if (oTexture.IsValid())
		{
			// Copy current texture desc
			m_oNewTextureDesc = oTexture.GetDesc();
		}
		else
		{
			//Reset texture format
			m_oNewTextureDesc = Graphics::Texture::Desc();
			m_oNewTextureDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
			m_oNewTextureDesc.iWidth = 128;
			m_oNewTextureDesc.iHeight = 128;
			m_oNewTextureDesc.iSliceCount = 1;
			m_oNewTextureDesc.iMipCount = 1;
		}
		ImGui::OpenPopup("TextureNew");
	}

	if (ImGui::BeginPopupModal("TextureNew", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::BeginCombo("Pixel format", Graphics::PixelFormatEnumInfos[m_oNewTextureDesc.ePixelFormat].pName))
		{
			for (int iPixelFormat = Graphics::PixelFormatEnum::_NONE + 1; iPixelFormat < Graphics::PixelFormatEnum::_COUNT; ++iPixelFormat)
			{
				if (ImGui::MenuItem(Graphics::PixelFormatEnumInfos[iPixelFormat].pName, NULL, iPixelFormat == m_oNewTextureDesc.ePixelFormat))
				{
					m_oNewTextureDesc.ePixelFormat = (Graphics::PixelFormatEnum)iPixelFormat;
				}
			}
			//ImGui::MenuItem()
			ImGui::EndCombo();
		}
		bool bIsCubeMap = m_oNewTextureDesc.eFaces != Graphics::Texture::FaceFlag::NONE;
		if (ImGui::Checkbox("IsCubeMap", &bIsCubeMap))
		{
			m_oNewTextureDesc.eFaces = bIsCubeMap ? Graphics::Texture::FaceFlag::ALL : Graphics::Texture::FaceFlag::NONE;
			if (bIsCubeMap)
			{
				m_oNewTextureDesc.iSliceCount = 6;
				m_oNewTextureDesc.iHeight = m_oNewTextureDesc.iWidth;
			}
		}

		if (bIsCubeMap)
		{
			int iSize = m_oNewTextureDesc.iWidth;
			if (ImGui::DragInt("Size", &iSize, 0.5f, 1, Graphics::Texture::c_iMaxSize))
			{
				iSize = Math::Clamp(iSize, 1, Graphics::Texture::c_iMaxSize);

				m_oNewTextureDesc.iWidth = iSize;
				m_oNewTextureDesc.iHeight = iSize;
			}
		}
		else
		{
			int iWidth = m_oNewTextureDesc.iWidth;
			if (ImGui::DragInt("Width", &iWidth, 0.5f, 1, Graphics::Texture::c_iMaxSize))
			{
				iWidth = Math::Clamp(iWidth, 1, Graphics::Texture::c_iMaxSize);

				m_oNewTextureDesc.iWidth = iWidth;
			}

			int iHeight = m_oNewTextureDesc.iHeight;
			if (ImGui::DragInt("Height", &iHeight, 0.5f, 1, Graphics::Texture::c_iMaxSize))
			{
				iHeight = Math::Clamp(iHeight, 1, Graphics::Texture::c_iMaxSize);

				m_oNewTextureDesc.iHeight = iHeight;
			}

			int iDepth = m_oNewTextureDesc.iSliceCount;
			if (ImGui::DragInt("Slice/Depth", &iDepth, 0.5f, 1, Graphics::Texture::c_iMaxSize))
			{
				iDepth = Math::Clamp(iDepth, 1, Graphics::Texture::c_iMaxSize);

				m_oNewTextureDesc.iSliceCount = iDepth;
			}
		}

		bool bIsSizeValid = true;
		bIsSizeValid &= (m_oNewTextureDesc.iWidth % Graphics::PixelFormatEnumInfos[m_oNewTextureDesc.ePixelFormat].iBlockWidth) == 0;
		bIsSizeValid &= (m_oNewTextureDesc.iHeight % Graphics::PixelFormatEnumInfos[m_oNewTextureDesc.ePixelFormat].iBlockHeight) == 0;

		if (bIsSizeValid == false)
		{
			ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "Size not compatible with selected Pixel format");
		}

		uint16_t iMaxMipCount = Math::HighBitLast(Math::Max<uint16_t>(Math::Max<uint16_t>(m_oNewTextureDesc.iWidth, m_oNewTextureDesc.iHeight), m_oNewTextureDesc.iSliceCount));

		int iMipCount = m_oNewTextureDesc.iMipCount;
		if (ImGui::SliderInt("Mip count", &iMipCount, 1, iMaxMipCount))
		{
			iMipCount = Math::Clamp(iMipCount, 1, (int)iMaxMipCount);

			m_oNewTextureDesc.iMipCount= iMipCount;
		}

		if (ImGui::Button("Create") && bIsSizeValid)
		{
			ErrorCode oErr = oTexture.Create(m_oNewTextureDesc);
			CORE_VERIFY_OK(oErr);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->ClearTextureFilePath();
				Program::GetInstance()->UpdateTexture2DRes();
				Core::LogInfo("Menus", "New texture created");
			}
			else
			{
				Core::LogError("Menus", "Can't a create new texture : %s", oErr.ToString());
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	bool bOpenResizeMenu = false;

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGuiUtils::MenuItemPlus("Copy current slice", NULL, NULL, NULL, NULL, oTexture.IsValid(), (ImTextureID)m_pIconCopy->GetTextureView()))
		{
			if (IO::Clipboard::SetTexturePNG(oTexture, 0, oDisplayOptions.iMip, oDisplayOptions.iSlice) == ErrorCode::Ok)
			{
				Core::LogInfo("Menus", "Current Slice copied to clipboard");
			}
			else
			{
				Core::LogError("Menus", "Can't copy current Slice to clipboard");
			}
		}

		if (ImGuiUtils::BeginMenu("Copy component", oTexture.IsValid(), (ImTextureID)m_pIconCopy->GetTextureView()))
		{
			ImGui::PushID("CopyComponent");
			ImVec2 oWhitePixelUV = ImGui::GetFontTexUvWhitePixel();
			ImTextureID hWhiteTexture = ImGui::GetIO().Fonts->TexID;

			const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()];
			
			for (Graphics::ComponentFlag eComponent = Graphics::ComponentFlag::_BEGIN; eComponent <= Graphics::ComponentFlag::_END; eComponent = (Graphics::ComponentFlag)(eComponent << 1))
			{
				if ((oPixelFormatInfos.iComponents & eComponent) == 0 || Graphics::ComponentFlagString[eComponent] == NULL)
					continue;

				const uint8_t* pColor = Graphics::ComponentFlagColor[eComponent];
				if (ImGuiUtils::MenuItemPlus(Graphics::ComponentFlagString[eComponent], NULL, NULL, NULL, NULL, oTexture.IsValid(), hWhiteTexture, ImVec4(pColor[0] / 255.f, pColor[1] / 255.f, pColor[2] / 255.f, 1.f), oWhitePixelUV, oWhitePixelUV))
				{
					Graphics::Texture oNewTexture;
					if (Graphics::ExtractChannel(&oTexture, &oNewTexture, eComponent) == ErrorCode::Ok
						&& IO::Clipboard::SetTexturePNG(oNewTexture, 0, oDisplayOptions.iMip, oDisplayOptions.iSlice) == ErrorCode::Ok)
					{
						Core::LogInfo("Menus", "Current Slice component copied to clipboard");
					}
					else
					{
						Core::LogError("Menus", "Can't copy current Slice component to clipboard");
					}
				}
			}

			ImGui::PopID();
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGuiUtils::BeginMenu("Paste to component", oTexture.IsValid(), (ImTextureID)m_pIconPaste->GetTextureView()))
		{
			ImGui::PushID("PasteComponent");
			ImVec2 oWhitePixelUV = ImGui::GetFontTexUvWhitePixel();
			ImTextureID hWhiteTexture = ImGui::GetIO().Fonts->TexID;

			const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()];

			for (Graphics::ComponentFlag eComponent = Graphics::ComponentFlag::_BEGIN; eComponent <= Graphics::ComponentFlag::_END; eComponent = (Graphics::ComponentFlag)(eComponent << 1))
			{
				if ((oPixelFormatInfos.iComponents & eComponent) == 0 || Graphics::ComponentFlagString[eComponent] == NULL)
					continue;

				const uint8_t* pColor = Graphics::ComponentFlagColor[eComponent];
				if (ImGuiUtils::MenuItemPlus(Graphics::ComponentFlagString[eComponent], NULL, NULL, NULL, NULL, oTexture.IsValid(), hWhiteTexture, ImVec4(pColor[0] / 255.f, pColor[1] / 255.f, pColor[2] / 255.f, 1.f), oWhitePixelUV, oWhitePixelUV))
				{
					Graphics::Texture::SliceData oDestSliceData = oTexture.GetSliceData(0, oDisplayOptions.iMip, oDisplayOptions.iSlice);
					Graphics::Texture::ComponentAccessor oDestComponent = oDestSliceData.GetComponentAccesor(eComponent);

					Graphics::Texture oClipboardTexture;
					if (IO::Clipboard::GetTexture(&oClipboardTexture))
					{
						Graphics::Texture::SliceData oSourceSliceData = oClipboardTexture.GetSliceData(0, 0, 0);
						Graphics::Texture::ComponentAccessor oSourceCompoent = oSourceSliceData.GetComponentAccesor(Graphics::ComponentFlag::R);

						if (oSourceCompoent.CopyTo(oDestComponent))
						{
							Program::GetInstance()->UpdateTexture2DRes();
							Core::LogInfo("Menus", "Image paste to current Slice component from clipboard");
						}
						else
						{
							Core::LogError("Menus", "Can't copy image from clipboard to current Slice component");
						}
					}
					else
					{
						Core::LogError("Menus", "No image in clipboard");
					}
				}
			}

			ImGui::PopID();
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGuiUtils::BeginMenu("Convert pixel format to", oTexture.IsValid(), (ImTextureID)m_pIconConvert->GetTextureView()))
		{
			static Graphics::PixelFormat::ConvertionInfoList s_oAvailableConvertionFormats;
			static int s_iAvailableConvertionFormatCount = 0;
			static Graphics::PixelFormatEnum s_iCurrentPixelFormat = Graphics::PixelFormatEnum::_NONE;
			if (s_iCurrentPixelFormat != oTexture.GetPixelFormat())
			{
				s_iCurrentPixelFormat = oTexture.GetPixelFormat();
				s_iAvailableConvertionFormatCount = Graphics::PixelFormat::GetAvailableConvertion(oTexture.GetPixelFormat(), true, &s_oAvailableConvertionFormats);
			}

			int iTextureWidth = oTexture.GetWidth();
			int iTextureHeight = oTexture.GetHeight();
			for (int iIndex = 0; iIndex < s_iAvailableConvertionFormatCount; ++iIndex)
			{
				Graphics::PixelFormatEnum ePixelFormat = s_oAvailableConvertionFormats[iIndex].eFormat;
				const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[ePixelFormat];

				bool bGoodWidth = ((iTextureWidth / oPixelFormatInfos.iBlockWidth) * oPixelFormatInfos.iBlockWidth == iTextureWidth);
				bool bGoodHeight = ((iTextureHeight / oPixelFormatInfos.iBlockHeight) * oPixelFormatInfos.iBlockHeight == iTextureHeight);
				bool bAvailable = bGoodWidth && bGoodHeight;

				if (ImGuiUtils::MenuItemPlus(oPixelFormatInfos.pName, NULL, NULL, NULL, false, bAvailable))
				{
					const Graphics::PixelFormatInfos& oPreviousPixelFormatInfos = Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()];
					ErrorCode oErr = Graphics::ConvertPixelFormat(&oTexture, &oTexture, ePixelFormat);
					if (oErr == ErrorCode::Ok)
					{
						Program::GetInstance()->UpdateTexture2DRes();
						Core::LogInfo("Menus", "Pixel format converted from '%s' to '%s'", oPreviousPixelFormatInfos.pName, oPixelFormatInfos.pName);
					}
					else
					{
						Core::LogError("Menus", "Can't convet pixel format from '%s' to '%s' : %s", oPreviousPixelFormatInfos.pName, oPixelFormatInfos.pName, oErr.ToString());
					}
				}
				if (bAvailable == false && ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted("Pixel format not available");
					if (bGoodWidth == false)
						ImGui::Text("Texture width multiplier should be %d", oPixelFormatInfos.iBlockWidth);
					if (bGoodHeight == false)
						ImGui::Text("Texture height multiplier should be %d", oPixelFormatInfos.iBlockWidth);
					ImGui::EndTooltip();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		bool bIsResizablePixelFormat = oTexture.IsValid() && Graphics::IsPixelFormatResizable(oTexture.GetPixelFormat());
		if (ImGuiUtils::MenuItemPlus("Resize", NULL, NULL, NULL, false, bIsResizablePixelFormat, (ImTextureID)m_pIconResize->GetTextureView()))
		{
			bOpenResizeMenu = true;
		}
		if (oTexture.IsValid() && bIsResizablePixelFormat == false && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Resize not supported for this pixel format");
		}

		ImGui::Separator();

		if (ImGuiUtils::MenuItemPlus("Generate all mips", NULL, NULL, NULL, false, bIsResizablePixelFormat, (ImTextureID)m_pIconMipMap->GetTextureView()))
		{
			ErrorCode oErr = Graphics::GenerateMips(&oTexture, &oTexture);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->UpdateTexture2DRes();
				Core::LogInfo("Menus", "Generated all mips");
			}
			else
			{
				Core::LogError("Menus", "Can't generate all mips : %s", oErr.ToString());
			}
		}

		if (ImGuiUtils::MenuItemPlus("Generate missing mips", NULL, NULL, NULL, false, bIsResizablePixelFormat, (ImTextureID)m_pIconMissingMipMap->GetTextureView()))
		{
			ErrorCode oErr = Graphics::GenerateMissingMips(&oTexture, &oTexture);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->UpdateTexture2DRes();
				Core::LogInfo("Menus", "Generated missing mips");
			}
			else
			{
				Core::LogError("Menus", "Can't generate missing mips : %s", oErr.ToString());
			}
		}

		if (ImGuiUtils::MenuItemPlus("Regenerate current mip map", NULL, NULL, NULL, false, bIsResizablePixelFormat && oDisplayOptions.iMip > 0, (ImTextureID)m_pIconCurrentMipMap->GetTextureView()))
		{
			uint16_t iCurrentMipMask = 1 << oDisplayOptions.iMip;

			ErrorCode oErr = Graphics::GenerateMips(&oTexture, &oTexture, iCurrentMipMask);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->UpdateTexture2DRes();
				Core::LogInfo("Menus", "Regenerate mip map %d", oDisplayOptions.iMip);
			}
			else
			{
				Core::LogError("Menus", "Can't generate mip map %d : %s", oDisplayOptions.iMip, oErr.ToString());
			}
		}

		if (ImGuiUtils::MenuItemPlus("Delete mips after current mip", NULL, NULL, NULL, false, bIsResizablePixelFormat && oDisplayOptions.iMip < (oTexture.GetMipCount() - 1), (ImTextureID)m_pIconCut->GetTextureView()))
		{
			Graphics::Texture::Desc oDesc = oTexture.GetDesc();
			oDesc.iMipCount = oDisplayOptions.iMip + 1;
			Graphics::Texture oNewTexture;
			ErrorCode oErr = oNewTexture.Create(oDesc);
			if (oErr == ErrorCode::Ok)
			{
				for (uint16_t iLayer = 0; iLayer < oDesc.iLayerCount; ++iLayer)
				{
					const Graphics::Texture::LayerData oSrcLayerData = oTexture.GetLayerData(iLayer);
					const Graphics::Texture::LayerData oDstLayerData = oNewTexture.GetLayerData(iLayer);
					for (uint16_t iMip = 0; iMip < oDesc.iMipCount; ++iMip)
					{
						const Graphics::Texture::MipData oSrcMipData = oSrcLayerData.GetMipData(iMip);
						const Graphics::Texture::MipData oDstMipData = oDstLayerData.GetMipData(iMip);
						for (uint16_t iSlice = 0; iSlice < oDesc.iSliceCount; ++iSlice)
						{
							const Graphics::Texture::SliceData oSrcSliceData = oSrcMipData.GetSliceData(iSlice);
							const Graphics::Texture::SliceData oDstSliceData = oDstMipData.GetSliceData(iSlice);

							Core::MemCpy(oDstSliceData.pData, oSrcSliceData.pData, oSrcSliceData.iSize);
						}
					}
				}
				oTexture.Swap(oNewTexture);
				Program::GetInstance()->UpdateTexture2DRes();

				Core::LogInfo("Menus", "Delete mips after mip %d", oDisplayOptions.iMip);
			}
			else
			{
				Core::LogError("Menus", "Can't delete mips after mip map %d : %s", oDisplayOptions.iMip, oErr.ToString());
			}
		}

		ImGui::EndMenu();
	}

	if (bOpenResizeMenu)
	{
		if (oTexture.IsValid())
		{
			ImGui::OpenPopup("TextureResize");
			m_bResizeKeepRatio = true;
			m_iResizeNewWidth = oTexture.GetWidth();
			m_iResizeNewHeight = oTexture.GetHeight();
			m_fResizeRatio = (double)m_iResizeNewWidth / (double)m_iResizeNewHeight;
		}
	}

	if (ImGui::BeginPopupModal("TextureResize", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Checkbox("Keep ratio", &m_bResizeKeepRatio);
		if (ImGui::DragInt("Width", &m_iResizeNewWidth, 0.5f))
		{
			if (m_iResizeNewWidth < 1)
				m_iResizeNewWidth = 1;
			if (m_iResizeNewWidth > 65536)
				m_iResizeNewWidth = 65536;

			if (m_bResizeKeepRatio)
				m_iResizeNewHeight = (int)(m_iResizeNewWidth / m_fResizeRatio);
		}

		if (ImGui::DragInt("Height", &m_iResizeNewHeight, 0.5f))
		{
			if (m_iResizeNewHeight < 1)
				m_iResizeNewHeight = 1;
			if (m_iResizeNewHeight > 16384)
				m_iResizeNewHeight = 16384;

			if (m_bResizeKeepRatio)
				m_iResizeNewWidth = (int)(m_iResizeNewHeight * m_fResizeRatio);
		}

		if (oTexture.GetMipCount() > 1)
		{
			ImGui::TextDisabled("Mip maps will be erased");
		}

		if (ImGui::Button("Resize"))
		{
			ErrorCode oErr = Graphics::ResizeTexture(&oTexture, &oTexture, m_iResizeNewWidth, m_iResizeNewHeight);
			CORE_VERIFY_OK(oErr);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->UpdateTexture2DRes();
				Core::LogInfo("Menus", "Resized to %d x %d", m_iResizeNewWidth, m_iResizeNewHeight);
			}
			else
			{
				Core::LogError("Menus", "Can't resize to %d x %d : %s", m_iResizeNewWidth, m_iResizeNewHeight, oErr.ToString());
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGuiUtils::MenuItemPlus("Logs", NULL, NULL, NULL, false, true, (ImTextureID)m_pIconLog->GetTextureView()))
		{
			ImWindow::ImwWindowManager* pWindowManager = Program::GetInstance()->GetWindowManager();
			pWindowManager->Dock(new Windows::LoggerWindow(), ImWindow::E_DOCK_ORIENTATION_BOTTOM, 0.3f);
		}

		ImGui::Separator();

		if (ImGuiUtils::MenuItemPlus("About", NULL, NULL, NULL, false, true, (ImTextureID)m_pIconHelp->GetTextureView()))
		{
			const ImVec2 c_vAboutBoxSize = ImVec2(600.f, 350.f);

			// Centering floating window
			ImWindow::ImwWindowManager* pWindowManager = Program::GetInstance()->GetWindowManager();
			ImVec2 vMainPos = pWindowManager->GetMainPlatformWindow()->GetPosition();
			ImVec2 vMainSize = pWindowManager->GetMainPlatformWindow()->GetSize();
			ImVec2 vCenter = vMainPos + vMainSize / 2.f - c_vAboutBoxSize / 2.f;

			pWindowManager->Float(new Windows::AboutWindow(), vCenter, c_vAboutBoxSize);
		}
		ImGui::EndMenu();
	}
}