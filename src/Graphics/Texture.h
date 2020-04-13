#ifndef __GRAPHICS_TEXTURE_H__
#define __GRAPHICS_TEXTURE_H__

#include "Core/ErrorCode.h"
#include "Core/Array.h"
#include "Core/Memory.h"

#include "IO/Stream.h"

#include "Graphics/PixelFormat.h"

#include <vector>

namespace Graphics
{
	class Texture
	{
	public:
		static const int c_iMaxSize = 32768;
		static const int c_iMaxMip = 16;

		enum EFace
		{
			E_FACE_POS_X = 0,
			E_FACE_NEG_X,
			E_FACE_POS_Y,
			E_FACE_NEG_Y,
			E_FACE_POS_Z,
			E_FACE_NEG_Z,

			_E_FACE_COUNT
		};
		static const char* const EFace_string[_E_FACE_COUNT];

		struct TextureFaceData
		{
			TextureFaceData();
			CORE_PTR_VOID				pData;
			size_t						iSize;
			size_t						iPitch;
			int							iWidth;
			int							iHeight;
		};

		struct TextureData
		{
			friend class Texture;
		public:
			struct Desc
			{
				Desc();
				int						iWidth;
				int						iHeight;
				PixelFormatEnum			ePixelFormat;
				int						iFaceCount;
				int						iMipCount;
			};
			TextureData();
			~TextureData();

			ErrorCode					Create(Desc& oDesc);
			void						Destroy();
			bool						IsValid() const;

			const TextureFaceData&		GetFaceData(int iMip, int iFace) const { return m_oFaceData[iFace][iMip]; }
			CORE_PTR_VOID				GetData() const { return m_pData; }
			size_t						GetDataSize() const { return m_iSize; }
		protected:
			/* Data layout
			for each mip
				for each face
					for each line (y)
						for each column (x)
			*/
			CORE_PTR_VOID				m_pData;
			size_t						m_iSize;

			TextureFaceData				m_oFaceData[_E_FACE_COUNT][c_iMaxMip];
		};

		struct Desc : TextureData::Desc
		{
			Desc();
			const void*					pData[_E_FACE_COUNT][c_iMaxMip];
		};
	public:
		Texture();
		~Texture();

		bool							IsValid() const;

		ErrorCode						Create(Desc& oDesc);
		ErrorCode 						Destroy();

		int								GetWidth() const { return m_iWidth; }
		int								GetHeight() const { return m_iHeight; }
		PixelFormatEnum					GetPixelFormat() const { return m_ePixelFormat; }
		int								GetFaceCount() const { return m_iFaceCount; }
		int								GetMipCount() const { return m_iMipCount; }

		const TextureData&				GetData() const { return m_oData; }

		void							Swap(Texture& oOtherTexture);

		Texture&						operator=(const Texture& oTexture);
	protected:
		int								m_iWidth;
		int								m_iHeight;
		PixelFormatEnum					m_ePixelFormat;
		int								m_iFaceCount;
		int								m_iMipCount;
		TextureData						m_oData;
	};
} // namespace Graphics

#endif //__GRAPHICS_TEXTURE_H__