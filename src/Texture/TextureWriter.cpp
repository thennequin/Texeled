#include "Texture/TextureWriter.h"

#include "Core/StringUtils.h"

#include "IO/FileStream.h"

namespace Texture
{
	static Core::Array<TextureWriterInfo>	s_oTextureWriters;
	static bool s_bRegisteringTextureWriter = false;

	TextureWriterHandle BeginRegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester, size_t iSizeofStruct)
	{
		CORE_ASSERT(pName != NULL);
		CORE_ASSERT(pExts != NULL);
		CORE_ASSERT(pWriter != NULL);
		CORE_ASSERT(pWriterTester != NULL);
		CORE_ASSERT(s_bRegisteringTextureWriter == false);

		if (pName == NULL || pExts == NULL || pWriter == NULL || pWriterTester == NULL || s_bRegisteringTextureWriter)
			return TextureWriterHandleInvalid;

		s_bRegisteringTextureWriter = true;

		TextureWriterInfo oInfo;
		oInfo.pName = pName;
		oInfo.pExt = pExts;
		oInfo.pWriter = pWriter;
		oInfo.pTester = pWriterTester;
		oInfo.iSettingsStructSize = iSizeofStruct;
		s_oTextureWriters.push_back(oInfo);
		return s_oTextureWriters.size() - 1;
	}

	void AddSettingBool(const char* pName, intptr_t iOffset, bool bDefault)
	{
		CORE_ASSERT(s_bRegisteringTextureWriter == true);
		s_oTextureWriters.back().oSettings.push_back(WriterSetting());
		WriterSetting& oSetting = s_oTextureWriters.back().oSettings.back();
		oSetting.sName = pName;
		oSetting.iOffset = iOffset;
		oSetting.eType = SettingType::BOOL;
		oSetting.oDefault.Bool = bDefault;
	}

	void AddSettingFloat(const char* pName, intptr_t iOffset, float fDefault, float fMin, float fMax)
	{
		CORE_ASSERT(s_bRegisteringTextureWriter == true);
		s_oTextureWriters.back().oSettings.push_back(WriterSetting());
		WriterSetting& oSetting = s_oTextureWriters.back().oSettings.back();
		oSetting.sName = pName;
		oSetting.iOffset = iOffset;
		oSetting.eType = SettingType::FLOAT;
		oSetting.oDefault.Float = fDefault;
		oSetting.oMin.Float = fMin;
		oSetting.oMax.Float = fMax;
	}

	void AddSettingInteger(const char* pName, intptr_t iOffset, int iDefault, int iMin, int iMax)
	{
		CORE_ASSERT(s_bRegisteringTextureWriter == true);
		s_oTextureWriters.back().oSettings.push_back(WriterSetting());
		WriterSetting& oSetting = s_oTextureWriters.back().oSettings.back();
		oSetting.sName = pName;
		oSetting.iOffset = iOffset;
		oSetting.eType = SettingType::INTEGER;
		oSetting.oDefault.Integer = iDefault;
		oSetting.oMin.Integer = iMin;
		oSetting.oMax.Integer = iMax;
	}

	void EndRegisterTextureWriter()
	{
		CORE_ASSERT(s_bRegisteringTextureWriter == true);
		s_bRegisteringTextureWriter = false;
	}

	TextureWriterHandle RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester)
	{
		TextureWriterHandle hWriter = BeginRegisterTextureWriter(pName, pExts, pWriter, pWriterTester, 0);
		EndRegisterTextureWriter();
		return hWriter;
	}

	const TextureWriterInfo* GetTextureWriter(TextureWriterHandle hWriter)
	{
		if (hWriter == (size_t)-1)
			return NULL;

		CORE_ASSERT(hWriter < s_oTextureWriters.size());

		return &s_oTextureWriters[hWriter];
	}

	void* CreateSettingsStruct(const TextureWriterInfo* pWriterInfo)
	{
		if (pWriterInfo->iSettingsStructSize == 0)
			return NULL;

		char* pSettings = (char*)malloc(pWriterInfo->iSettingsStructSize);
		if (pSettings == NULL)
			return NULL;

		for (size_t iSettingIndex = 0; iSettingIndex < pWriterInfo->oSettings.size(); ++iSettingIndex)
		{
			const WriterSetting& oSettingInfo = pWriterInfo->oSettings[iSettingIndex];
			void* pSetting = pSettings + oSettingInfo.iOffset;
			switch (oSettingInfo.eType)
			{
			case SettingType::BOOL:
				*(bool*)pSetting = oSettingInfo.oDefault.Bool;
				break;
			case SettingType::FLOAT:
				*(float*)pSetting = oSettingInfo.oDefault.Float;
				break;
			case SettingType::INTEGER:
				*(int*)pSetting = oSettingInfo.oDefault.Integer;
				break;
			}
		}

		return pSettings;
	}

	ErrorCode SaveToStream(Graphics::Texture* pTexture, const void* pSettings, IO::Stream* pStream, const char* pFilename, const TextureWriterInfo* pUseWriter)
	{
		if (pTexture != NULL)
		{
			if (pUseWriter == NULL)
			{
				for (Core::Array<TextureWriterInfo>::iterator it = s_oTextureWriters.begin(), itEnd = s_oTextureWriters.end(); it != itEnd; ++it)
				{
					if (Core::StringUtils::Wildcard(it->pExt, pFilename, false))
					{
						pUseWriter = &*it;
						break;
					}
				}
			}

			if (pUseWriter != NULL)
			{
				if (pUseWriter->pTester != NULL && pUseWriter->pTester(pTexture) == E_SUPPORTED_WRITER_FALSE)
					return ErrorCode(1, "Texture not supported by writer");

				if (pUseWriter->pWriter(pTexture, pSettings, pStream))
				{
					return ErrorCode::Ok;
				}
				else
				{
					return ErrorCode::Fail;
				}
			}
			else
			{
				return ErrorCode(1, "Extension not supported");
			}
		}
		return ErrorCode(1, "Invalid arguments");
	}

	ErrorCode SaveToFile(Graphics::Texture* pTexture, const void* pSettings, const char* pFilename, const TextureWriterInfo* pUseWriter)
	{
		if (pTexture != NULL)
		{
			IO::FileStream oFileStream;
			if (oFileStream.Open(pFilename, IO::FileStream::AccessModeEnum::WRITE_SAFE))
			{
				ErrorCode oErr = SaveToStream(pTexture, pSettings, &oFileStream, pFilename, pUseWriter);

				if (oErr == ErrorCode::Ok)
				{
					oFileStream.Close();
				}
				else
				{
					oFileStream.Cancel();
				}

				return oErr;
			}
		}
		return ErrorCode(1, "Invalid arguments");
	}

	void GetTextureWriters(const TextureWriterInfo** pOutWriters, int* pOutCount)
	{
		*pOutWriters = s_oTextureWriters.begin();
		*pOutCount = (int)s_oTextureWriters.size();
	}
}