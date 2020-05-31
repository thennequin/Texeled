#ifndef __GRAPHICS_TEXTURE_H__
#define __GRAPHICS_TEXTURE_H__

#include "Core/ErrorCode.h"
#include "Core/Memory.h"

#include "Graphics/PixelFormat.h"

namespace Graphics
{
	class Texture
	{
	public:
		static const int c_iMaxSize = 32768;
		static const int c_iMaxMip = 16;

		struct _FaceFlag
		{
			enum Enum
			{
				NONE = 0,
				POS_X = (1 << 0),
				NEG_X = (1 << 1),
				POS_Y = (1 << 2),
				NEG_Y = (1 << 3),
				POS_Z = (1 << 4),
				NEG_Z = (1 << 5),

				ALL = (POS_X | NEG_X | POS_Y | NEG_Y | POS_Z | NEG_Z),

				_MAX_VALUE = NEG_Z + 1,
				_COUNT = 6
			};
		};
		typedef _FaceFlag::Enum FaceFlag;
		typedef uint8_t FaceFlags;
		static const char* const Face_string[FaceFlag::_MAX_VALUE];
		static FaceFlag GetFace(FaceFlags iFaces, int iIndex);

		struct SliceData
		{
			PixelFormatEnum				ePixelFormat;
			uint16_t					iWidth;
			uint16_t					iHeight;
			FaceFlag					eFace;

			size_t						iPitch;
			size_t						iSize;
			CORE_PTR_VOID				pData;
		};

		struct MipData
		{
			PixelFormatEnum				ePixelFormat;
			uint16_t					iWidth;
			uint16_t					iHeight;
			uint16_t					iSliceCount;
			FaceFlags					eFaces;

			size_t						iSize;
			CORE_PTR_VOID				pData;

			SliceData					GetSlice(uint16_t iSlice) const;
		};

		struct LayerData
		{
			PixelFormatEnum				ePixelFormat;
			uint16_t					iWidth;
			uint16_t					iHeight;
			uint8_t						iMipCount;
			uint16_t					iSliceCount;
			FaceFlags					eFaces;

			size_t						iSize;
			CORE_PTR_VOID				pData;

			MipData						GetMip(uint8_t iMip) const;
			SliceData					GetSlice(uint8_t iMip, uint16_t iSlice) const;
		};

		struct Desc
		{
			Desc();
			Desc(PixelFormatEnum ePixelFormat, uint16_t iWidth, uint16_t iHeight, uint16_t iLayerCount, uint8_t iMipCount, uint16_t iSliceCount, FaceFlags eFaces );
			PixelFormatEnum				ePixelFormat;
			uint16_t					iWidth;
			uint16_t					iHeight;
			uint16_t					iLayerCount;
			uint8_t						iMipCount;
			uint16_t					iSliceCount; // depth / faces
			FaceFlags					eFaces; // If eFaces not equal FaceFlag::None, faces count should be equal to iSliceCount
		};
		static const Desc				DescNull;
	public:
		Texture();
		~Texture();

		bool							IsValid() const;

		ErrorCode						Create(Desc& oDesc);
		ErrorCode 						Destroy();

		Desc							GetDesc() const					{ return m_oDesc; }

		uint16_t						GetWidth() const				{ return m_oDesc.iWidth; }
		uint16_t						GetHeight() const				{ return m_oDesc.iHeight; }
		PixelFormatEnum					GetPixelFormat() const			{ return m_oDesc.ePixelFormat; }
		uint16_t						GetLayerCount() const			{ return m_oDesc.iLayerCount; }
		uint8_t							GetMipCount() const				{ return m_oDesc.iMipCount; }
		uint16_t						GetSliceCount() const			{ return m_oDesc.iSliceCount; }

		size_t							GetDataSize() const				{ return m_iSize; }
		size_t							GetLayerDataSize() const		{ return m_iLayerSize; }

		const LayerData					GetLayerData(uint16_t iLayer) const;
		const MipData					GetMipData(uint16_t iLayer, uint8_t iMip) const;
		const SliceData					GetSliceData(uint16_t iLayer, uint8_t iMip, uint16_t iSlice) const;

		void							Swap(Texture& oOtherTexture);

		Texture&						operator=(const Texture& oTexture);

		static void						GetMipSizeAndOffset(PixelFormatEnum ePixelFormat, uint16_t  iWidth, uint16_t  iHeight, uint16_t iSliceCount, uint8_t iMip, size_t* pOutMipSize, size_t* pOutMipOffset);
	protected:
		/* Data layout :
			for each layer
				for each mip
					for each slice (depth/face)
						for each line (y)
							for each column (x)
		*/
		Desc							m_oDesc;

		CORE_PTR_VOID					m_pData;
		size_t							m_iSize;
		size_t							m_iLayerSize;
	};
} // namespace Graphics

#endif //__GRAPHICS_TEXTURE_H__
