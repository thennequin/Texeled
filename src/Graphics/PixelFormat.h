#ifndef __GRAPHICS_PIXELFORMAT_H__
#define __GRAPHICS_PIXELFORMAT_H__

#include <stdint.h>

namespace Graphics
{
	struct _ComponentEncodingEnum
	{
		enum Enum
		{
			_NONE = 0,

			UNORM,
			INT,
			UINT,
			FLOAT,
			SNORM,

			_COUNT
		};
	};
	typedef _ComponentEncodingEnum::Enum ComponentEncodingEnum;
	extern const char* const ComponentEncodingEnumString[ComponentEncodingEnum::_COUNT];

	//https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkFormat.html
	struct _PixelFormatEnum
	{
		enum Enum
		{
			_NONE = 0,

			R8_UNORM,

			RG8_UNORM,

			RGB8_UNORM,
			BGR8_UNORM,

			RGBA8_UNORM,
			BGRA8_UNORM,

			R5G6B5_UNORM,
			B5G6BR_UNORM,

			RGB16_UNORM,
			RGB16_FLOAT,

			RGBA16_UNORM,
			RGBA16_FLOAT,

			R32_FLOAT,
			RG32_FLOAT,
			RGB32_FLOAT,
			RGBA32_FLOAT,

			R10G10B10A2_UNORM,

			R9G9B9E5_SHAREDEXP,

			D16_UNORM,

			BC1, // DXT1
			BC2, // DXT2 / DXT3
			BC3, // DXT4 / DXT5
			BC4,
			BC5,
			BC6H,
			BC7,

			_COUNT,
			_LAST = _COUNT - 1
		};
	};
	typedef _PixelFormatEnum::Enum PixelFormatEnum;

	typedef struct
	{
		PixelFormatEnum			eFormat;
		int						iBitsPerPixel;
		int						iBlockWidth;
		int						iBlockHeight;
		int						iBlockSize;
		int						iComponentCount;
		ComponentFlags			iComponents;
		ComponentEncodingEnum	eEncoding;
		const char* const		pShortName;
		const char* const		pName;
	} PixelFormatInfos;

	extern const PixelFormatInfos PixelFormatEnumInfos[PixelFormatEnum::_COUNT];

	namespace PixelFormat
	{
		typedef void(*ConvertionFunc)(void*, void*, size_t, size_t);
		typedef struct
		{
			ConvertionFunc pFunc;
			PixelFormatEnum eFormat;
		} ConvertionFuncInfo;

		typedef ConvertionFuncInfo ConvertionFuncChain[PixelFormatEnum::_COUNT];
		typedef uint8_t ConvertionTemporaryData[16*4*4]; // 16 pixels RGBA Float

		typedef struct
		{
			PixelFormatEnum eFormat;
			int iAdditionalBits;
		} ConvertionInfo;

		typedef ConvertionInfo ConvertionInfoList[PixelFormatEnum::_COUNT];

		bool					GetConvertionChain(PixelFormatEnum eSourcePixelFormat, PixelFormatEnum eDestPixelFormat, ConvertionFuncChain* pOutChain, int* pOutChainLength, int* pOutAdditionalBits);
		int						GetAvailableConvertion(PixelFormatEnum eSourcePixelFormat, bool bIncludeChains, ConvertionInfoList* pOutAvailablePixelFormat);
		bool					IsCompressed(PixelFormatEnum ePixelFormat);
		int						BitPerPixel(PixelFormatEnum ePixelFormat);
		int						BlockSize(PixelFormatEnum ePixelFormat);
		int						ComponentCount(PixelFormatEnum ePixelFormat);
		void					GetBlockCount(PixelFormatEnum ePixelFormat, uint32_t iWidth, uint32_t iHeight, uint32_t* pOutCountX, uint32_t* pOutCountY);
		uint32_t				GetPitch(PixelFormatEnum ePixelFormat, uint32_t iWidth);

#pragma pack(push,1)

		typedef union
		{
			struct _
			{
				uint16_t mantissa : 10;
				uint16_t exponent : 5;
				uint16_t sign : 1;
			} v;
			uint16_t u;
		} half;

		template<typename T>
		struct RG
		{
			T r;
			T g;
		};

		template<typename T>
		struct RGB
		{
			T r;
			T g;
			T b;
		};

		template<typename T, int iR, int iG, int iB>
		union RGBBits
		{
			struct _
			{
				T r : iR;
				T g : iG;
				T b : iB;
			} v;
			T u;
		};

		template<typename T>
		struct BGR
		{
			T b;
			T g;
			T r;
		};

		template<typename T, int iB, int iG, int iR>
		union BGRBits
		{
			struct _
			{
				T b : iB;
				T g : iG;
				T r : iR;
			} v;
			T u;
		};

		template<typename T>
		struct RGBA
		{
			T r;
			T g;
			T b;
			T a;
		};

		template<typename T, int iR, int iG, int iB, int iA>
		struct RGBABits
		{
			T r : iR;
			T g : iG;
			T b : iB;
			T a : iA;
		};

		template<typename T>
		struct BGRA
		{
			T b;
			T g;
			T r;
			T a;
		};

		template<typename T, int iB, int iG, int iR, int iA>
		union BGRABits
		{
			struct _
			{
				T b : iB;
				T g : iG;
				T r : iR;
				T a : iA;
			} v;
			T u;
		};

		template<typename T, int iR, int iG, int iB, int iE>
		struct RGBEBits
		{
			T r : iR;
			T g : iG;
			T b : iB;
			T e : iE;
		};


		typedef RGB<uint8_t> RGB8;
		typedef BGR<uint8_t> BGR8;

		typedef RGBA<uint8_t> RGBA8;
		typedef BGRA<uint8_t> BGRA8;

		typedef RGB<uint16_t> RGB16;
		typedef BGR<uint16_t> BGR16;
		typedef RGBA<uint16_t> RGBA16;
		typedef BGRA<uint16_t> BGRA16;

		typedef RGB<half> RGB16F;
		typedef RGBA<half> RGBA16F;

		typedef RG<float> RG32F;
		typedef RGB<float> RGB32F;
		typedef RGBA<float> RGBA32F;

		typedef RGBBits<uint16_t, 5, 6, 5> R5G6B5;
		typedef BGRBits<uint16_t, 5, 6, 5> B5G6R5;

		typedef RGBABits<uint32_t, 10, 10, 10, 2> R10G10B10A2;

		typedef RGBEBits<uint32_t, 9, 9, 9, 5> R9G9B9E5;

		typedef struct
		{
			B5G6R5		iColor[2];
			uint32_t	iPixelIndex;
		} BlockBC1;

		typedef struct
		{
			uint64_t	iAlphaIndex;

			B5G6R5		iColor[2];
			uint32_t	iPixelIndex;
		} BlockBC2;

		typedef struct
		{
			uint8_t		iAlpha[2];
			uint32_t	iAlphaIndex;
			uint16_t	iAlphaIndex2;

			B5G6R5		iColor[2];
			uint32_t	iPixelIndex;
		} BlockBC3;

#pragma pack(pop)

		static_assert(sizeof(RGB8) == 3, "Size of RGB888 is not correct");
		static_assert(sizeof(RGBA8) == 4, "Size of RGBA8 is not correct");
		static_assert(sizeof(R5G6B5) == 2, "Size of RGB565 is not correct");
		static_assert(sizeof(B5G6R5) == 2, "Size of BGR565 is not correct");

		static_assert(sizeof(RG32F) == 8, "Size of RG32F is not correct");
		static_assert(sizeof(RGB32F) == 12, "Size of RGB32F is not correct");
		static_assert(sizeof(RGBA32F) == 16, "Size of RGBA32F is not correct");
		static_assert(sizeof(RGB16F) == 6, "Size of RGB16F is not correct");
		static_assert(sizeof(RGBA16F) == 8, "Size of RGBA16F is not correct");
		static_assert(sizeof(R9G9B9E5) == 4, "Size of R9G9B9E5 is not correct");

		static_assert(sizeof(BlockBC1) == 8, "Size of BlockBC1 is not correct");
		static_assert(sizeof(BlockBC2) == 16, "Size of BlockBC2 is not correct");
		static_assert(sizeof(BlockBC3) == 16, "Size of BlockBC3 is not correct");
	}
}

#endif // __GRAPHICS_PIXELFORMAT_H__