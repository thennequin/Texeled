#ifndef __TEXTURE_TEXTURE_WRITER_H__
#define __TEXTURE_TEXTURE_WRITER_H__

#include "Core/Array.h"
#include "Core/String.h"

#include "Graphics/Texture.h"

namespace Texture
{
	enum ESupportedWriter
	{
		E_SUPPORTED_WRITER_FALSE,
		E_SUPPORTED_WRITER_PARTIAL,
		E_SUPPORTED_WRITER_FULL,
	};

	struct _SettingType
	{
		enum Enum
		{
			BOOL,
			FLOAT,
			INTEGER
		};
	};
	typedef _SettingType::Enum SettingType;

	typedef ESupportedWriter(*TextureWriterSupportedFunc)(Graphics::Texture* pTexture);
	typedef bool(*TextureWriterFunc)(Graphics::Texture* pTexture, const void* pSettings, IO::Stream* pStream);

	typedef struct
	{
		Core::String				sName;
		intptr_t					iOffset;
		SettingType					eType;
		union
		{
			bool					Bool;
			float					Float;
			int						Integer;
		} oDefault;

		union
		{
			float					Float;
			int						Integer;
		} oMin;

		union
		{
			float					Float;
			int						Integer;
		} oMax;
	} WriterSetting;


	typedef struct
	{
		const char*					pName;
		const char*					pExt;
		TextureWriterFunc			pWriter;
		TextureWriterSupportedFunc	pTester;
		size_t						iSettingsStructSize;
		Core::Array<WriterSetting>	oSettings;
	} TextureWriterInfo;

	typedef size_t TextureWriterHandle;
	const TextureWriterHandle TextureWriterHandleInvalid = (TextureWriterHandle)-1;

#define STRUCT_MEMBER_OFFSET(Struct, Member) ((intptr_t)(&((Struct*)0)->Member))

	/* */
	TextureWriterHandle				BeginRegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester, size_t iSizeofStruct);
	void							AddSettingBool(const char* pName, intptr_t iOffset, bool bDefault);
	void							AddSettingFloat(const char* pName, intptr_t iOffset, float fDefault, float fMin, float fMax);
	void							AddSettingInteger(const char* pName, intptr_t iOffset, int iDefault, int iMin, int iMax);
	void							EndRegisterTextureWriter();

	// Simple writer without settings
	TextureWriterHandle				RegisterTextureWriter(const char* pName, const char* pExts, TextureWriterFunc pWriter, TextureWriterSupportedFunc pWriterTester);

	const TextureWriterInfo*		GetTextureWriter(TextureWriterHandle hWriter);

	void*							CreateSettingsStruct(const TextureWriterInfo* pWriterInfo);

	ErrorCode						SaveToStream(Graphics::Texture* pTexture, const void* pSettings, IO::Stream* pStream, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	ErrorCode						SaveToFile(Graphics::Texture* pTexture, const void* pSettings, const char* pFilename, const TextureWriterInfo* pUseWriter = NULL);
	void							GetTextureWriters(const TextureWriterInfo** pOutWriters, int* pOutCount);
}
//namspace Texture

#endif //__TEXTURE_TEXTURE_WRITER_H__