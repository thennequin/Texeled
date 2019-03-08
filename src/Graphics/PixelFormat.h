#ifndef __GRAPHICS_PIXELFORMAT_H__
#define __GRAPHICS_PIXELFORMAT_H__

#include <stdint.h>

namespace Graphics
{
	enum EComponentEncoding
	{
		E_COMPONENT_ENCODING_NONE = 0,

		E_COMPONENT_ENCODING_UNORM,
		E_COMPONENT_ENCODING_INT,
		E_COMPONENT_ENCODING_UINT,
		E_COMPONENT_ENCODING_FLOAT,
		E_COMPONENT_ENCODING_SNORM,

		_E_COMPONENT_ENCODING_COUNT
	};
	extern const char* const EComponentEncoding_string[_E_COMPONENT_ENCODING_COUNT];

	//https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkFormat.html
	enum EPixelFormat
	{
		E_PIXELFORMAT_NONE = 0,

		E_PIXELFORMAT_R8_UNORM,

		E_PIXELFORMAT_RG8_UNORM,

		E_PIXELFORMAT_RGB8_UNORM,
		E_PIXELFORMAT_BGR8_UNORM,

		E_PIXELFORMAT_RGBA8_UNORM,
		E_PIXELFORMAT_BGRA8_UNORM,

		E_PIXELFORMAT_R5G6B5_UNORM,
		E_PIXELFORMAT_B5G6BR_UNORM,

		E_PIXELFORMAT_RGB16_FLOAT,

		E_PIXELFORMAT_RGBA16_UNORM,
		E_PIXELFORMAT_RGBA16_FLOAT,

		E_PIXELFORMAT_RGB32_FLOAT,
		E_PIXELFORMAT_RGBA32_FLOAT,

		E_PIXELFORMAT_R10G10B10A2_UNORM,

		E_PIXELFORMAT_R9G9B9E5_SHAREDEXP,

		E_PIXELFORMAT_BC1, // DXT1
		E_PIXELFORMAT_BC2, // DXT2 / DXT3
		E_PIXELFORMAT_BC3, // DXT4 / DXT5
		E_PIXELFORMAT_BC4,
		E_PIXELFORMAT_BC5,
		E_PIXELFORMAT_BC6H,
		E_PIXELFORMAT_BC7,

		_E_PIXELFORMAT_COUNT
	};
	extern const char* const EPixelFormat_string[_E_PIXELFORMAT_COUNT];

	typedef struct
	{
		int iBitsPerPixel;
		int iComponents;
		int iBlockWidth;
		int iBlockHeight;
		int iBlockSize;
		EComponentEncoding eEncoding;
	} PixelFormatInfos;

	extern PixelFormatInfos EPixelFormatInfos[_E_PIXELFORMAT_COUNT];

	namespace PixelFormat
	{
		typedef void(*ConvertionFunc)(void*, void*, size_t, size_t);
		typedef struct
		{
			ConvertionFunc pFunc;
			EPixelFormat eFormat;
		} ConvertionFuncInfo;

		typedef ConvertionFuncInfo ConvertionFuncChain[_E_PIXELFORMAT_COUNT];
		typedef uint8_t ConvertionTemporaryData[16*4*4]; // 16 pixels RGBA Float

		typedef struct
		{
			EPixelFormat eFormat;
			int iAdditionalBits;
		} ConvertionInfo;

		typedef ConvertionInfo ConvertionInfoList[_E_PIXELFORMAT_COUNT];

		bool					GetConvertionChain(EPixelFormat eSourcePixelFormat, EPixelFormat eDestPixelFormat, ConvertionFuncChain* pOutChain, int* pOutChainLength, int* pOutAdditionalBits);
		int						GetAvailableConvertion(EPixelFormat eSourcePixelFormat, bool bIncludeChains, ConvertionInfoList* pOutAvailablePixelFormat);
		bool					IsCompressed(EPixelFormat ePixelFormat);
		int						BitPerPixel(EPixelFormat ePixelFormat);
		int						BlockSize(EPixelFormat ePixelFormat);
		int						ComponentCount(EPixelFormat ePixelFormat);
		bool					GetDataSize(EPixelFormat ePixelFormat, uint32_t* pInOutWidth, uint32_t* pInOutHeight, uint32_t* pOutXPadding, uint32_t* pOutYPadding);

#pragma pack(push,1)

		typedef union
		{
			struct
			{
				uint16_t mantissa : 10;
				uint16_t exponent : 5;
				uint16_t sign : 1;
			};
			uint16_t u;
		} half;

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
			struct
			{
				T r : iR;
				T g : iG;
				T b : iB;
			};
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
			struct
			{
				T b : iB;
				T g : iG;
				T r : iR;
			};
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
			struct
			{
				T b : iB;
				T g : iG;
				T r : iR;
				T a : iA;
			};
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
		static_assert(sizeof(RGBA8) == 4, "Size of BGR565 is not correct");
		static_assert(sizeof(R5G6B5) == 2, "Size of RGB565 is not correct");
		static_assert(sizeof(B5G6R5) == 2, "Size of BGR565 is not correct");

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