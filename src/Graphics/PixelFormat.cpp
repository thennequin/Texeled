#include "Graphics/PixelFormat.h"

#include "Graphics/Texture.h"

#include "Graphics/PixelFormatConverters.h"
#include "Graphics/PixelFormatConvertersCompressed.h"

namespace Graphics
{
	extern const char* const ComponentEncodingEnumString[ComponentEncodingEnum::_COUNT] = {
		"None",

		"UNorm",
		"Int",
		"UInt",
		"Float",
		"SNorm"
	};

#define MASKZERO() { 0, 0, 0, 0 }
#define MASK(iMantisBits, iMantisShift) { iMantisBits, iMantisShift, 0, 0 }
#define MASKEXP(iMantisBits, iMantisShift, iExpBits, iExpShift) { iMantisBits, iMantisShift, iExpBits, iExpShift }

	const PixelFormatInfos PixelFormatEnumInfos[PixelFormatEnum::_COUNT] =
	{
		// .---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- eFormat
		// |
		// |                                       .------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ iBitsPerPixel
		// |                                       |
		// |                                       |    .------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- iBlockWidth
		// |                                       |    |
		// |                                       |    |    .-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- iBlockHeight
		// |                                       |    |    |
		// |                                       |    |    |    .--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- iBlockSize (bytes)
		// |                                       |    |    |    |
		// |                                       |    |    |    |    .---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- iComponentCount
		// |                                       |    |    |    |    |
		// |                                       |    |    |    |    |    .----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- iComponents
		// |                                       |    |    |    |    |    |
		// |                                       |    |    |    |    |    |                       .----------------------------------------------------------------------------------------------------------------------------------------------------------------------- eEncoding
		// |                                       |    |    |    |    |    |                       |
		// |                                       |    |    |    |    |    |                       |                              .---------------------------------------------------------------------------------------------------------------------------------------- pShortName
		// |                                       |    |    |    |    |    |                       |                              |
		// |                                       |    |    |    |    |    |                       |                              |                .----------------------------------------------------------------------------------------------------------------------- pName
		// |                                       |    |    |    |    |    |                       |                              |                |
		// |                                       |    |    |    |    |    |                       |                              |                |                               .--------------------------------------------------------------------------------------- iMaskRed
		// |                                       |    |    |    |    |    |                       |                              |                |                               |
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       .--------------------------------------------------------------- iMaskGreen
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       |
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       |                        .-------------------------------------- iMaskBlue
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       |                        |
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       |                        |                        .------------- iMaskAlpha
		// |                                       |    |    |    |    |    |                       |                              |                |                               |                       |                        |                        |
		{  PixelFormatEnum::_NONE,                 0,   0,   0,   0,   0,   ComponentFlag::_NONE,   ComponentEncodingEnum::_NONE,  "None",          "None",                         MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },

		{  PixelFormatEnum::R8_UNORM,              8,   1,   1,   1,   1,   ComponentFlag::RED,     ComponentEncodingEnum::UNORM,  "R8",            "R8 UNorm",                     MASK(8, 0),             MASKZERO(),              MASKZERO(),              MASKZERO()    },

		{  PixelFormatEnum::RG8_UNORM,            16,   1,   1,   2,   2,   ComponentFlag::RG,      ComponentEncodingEnum::UNORM,  "RG8",           "RG8 UNorm",                    MASK(8, 0),             MASK(8, 8),              MASKZERO(),              MASKZERO()    },

		{  PixelFormatEnum::RGB8_UNORM,           24,   1,   1,   3,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "RGB8",          "RGB8 UNorm",                   MASK(8, 0),             MASK(8, 8),              MASK(8, 16),             MASKZERO()    },
		{  PixelFormatEnum::BGR8_UNORM,           24,   1,   1,   3,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "BGR8",          "BGR8 UNorm",                   MASK(8, 16),            MASK(8, 8),              MASK(8, 0),              MASKZERO()    },

		{  PixelFormatEnum::RGBA8_UNORM,          32,   1,   1,   4,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "RGBA8",         "RGBA8 UNorm",                  MASK(8, 0),             MASK(8, 8),              MASK(8, 16),             MASK(8, 24)   },
		{  PixelFormatEnum::BGRA8_UNORM,          32,   1,   1,   4,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "BGRA8",         "BGRA8 UNorm",                  MASK(8, 16),            MASK(8, 8),              MASK(8, 0),              MASK(8, 24)   },

		{  PixelFormatEnum::R5G6B5_UNORM,         16,   1,   1,   2,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "R5G6B5",        "R5G6B5 UNorm",                 MASK(5, 0),             MASK(6, 5),              MASK(5, 11),             MASKZERO()    },
		{  PixelFormatEnum::B5G6BR_UNORM,         16,   1,   1,   2,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "B5G6R5",        "B5G6R5 UNorm",                 MASK(5, 11),            MASK(6, 5),              MASK(5, 0),              MASKZERO()    },

		{  PixelFormatEnum::RGB16_UNORM,          48,   1,   1,   6,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "RGB16",         "RGB16 UNorm",                  MASK(16, 0),            MASK(16, 16),            MASK(16, 32),            MASKZERO()    },
		{  PixelFormatEnum::RGB16_FLOAT,          48,   1,   1,   6,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::FLOAT,  "RGB16",         "RGB16 Float",                  MASK(16, 0),            MASK(16, 16),            MASK(16, 32),            MASKZERO()    },

		{  PixelFormatEnum::RGBA16_UNORM,         64,   1,   1,   8,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "RGBA16",        "RGBA16 UNorm",                 MASK(16, 0),            MASK(16, 16),            MASK(16, 32),            MASK(16, 48)  },
		{  PixelFormatEnum::RGBA16_FLOAT,         64,   1,   1,   8,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::FLOAT,  "RGBA16F",       "RGBA16 Float",                 MASK(16, 0),            MASK(16, 16),            MASK(16, 32),            MASK(16, 48)  },

		{  PixelFormatEnum::R32_FLOAT,            32,   1,   1,   4,   1,   ComponentFlag::RED,     ComponentEncodingEnum::FLOAT,  "R32F",          "R32 Float",                    MASK(32, 0),            MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::RG32_FLOAT,           64,   1,   1,   8,   2,   ComponentFlag::RG,      ComponentEncodingEnum::FLOAT,  "RG32F",         "RG32 Float",                   MASK(32, 0),            MASK(32, 32),            MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::RGB32_FLOAT,          96,   1,   1,  12,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::FLOAT,  "RGB32F",        "RGB32 Float",                  MASK(32, 0),            MASK(32, 32),            MASK(32, 64),            MASKZERO()    },
		{  PixelFormatEnum::RGBA32_FLOAT,        128,   1,   1,  16,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::FLOAT,  "RGBA32F",       "RGBA32 Float",                 MASK(32, 0),            MASK(32, 32),            MASK(32, 64),            MASK(32, 96)  },

		{  PixelFormatEnum::R10G10B10A2_UNORM,    32,   1,   1,   4,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "R10G10B10A2",   "R10G10B10A2 UNorm",            MASKEXP(10, 0, 2, 30),  MASKEXP(10, 10, 2, 30),  MASKEXP(10, 20, 2, 30),  MASKZERO()    },

		{  PixelFormatEnum::R9G9B9E5_SHAREDEXP,   32,   1,   1,   4,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::FLOAT,  "R9G9B9E5F",     "R9G9B9E5 Float (shared exp)",  MASKEXP(9, 0, 5, 27),   MASKEXP(9, 9, 5, 27),    MASKEXP(9, 18, 5, 27),   MASKZERO()    },

		{  PixelFormatEnum::D16_UNORM,            16,   1,   1,   2,   1,   ComponentFlag::DEPTH,   ComponentEncodingEnum::UNORM,  "D16",           "D16 UNorm",                    MASK(16, 0),            MASKZERO(),              MASKZERO(),              MASKZERO()   },

		{  PixelFormatEnum::BC1,                   4,   4,   4,   8,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "BC1",           "BC1 (DXT1)",                   MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC2,                   8,   4,   4,  16,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "BC2",           "BC2 (DXT2/3)",                 MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC3,                   8,   4,   4,  16,   4,   ComponentFlag::RGBA,    ComponentEncodingEnum::UNORM,  "BC3",           "BC3 (DXT4/5)",                 MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC4,                   4,   4,   4,   8,   1,   ComponentFlag::RED,     ComponentEncodingEnum::UNORM,  "BC4",           "BC4",                          MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC5,                   8,   4,   4,  16,   2,   ComponentFlag::RG,      ComponentEncodingEnum::UNORM,  "BC5",           "BC5",                          MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC6H,                  8,   4,   4,  16,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::FLOAT,  "BC6H",          "BC6H",                         MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    },
		{  PixelFormatEnum::BC7,                   8,   4,   4,  16,   3,   ComponentFlag::RGB,     ComponentEncodingEnum::UNORM,  "BC7",           "BC7",                          MASKZERO(),             MASKZERO(),              MASKZERO(),              MASKZERO()    }
	};

#undef MASKZERO
#undef MASK
#undef MASKEXP

	namespace PixelFormat
	{
		bool IsCompressed(PixelFormatEnum ePixelFormat)
		{
			return (ePixelFormat >= PixelFormatEnum::BC1 && ePixelFormat <= PixelFormatEnum::BC7);
		}

		int BitPerPixel(PixelFormatEnum ePixelFormat)
		{
			return PixelFormatEnumInfos[ePixelFormat].iBitsPerPixel;
		}

		//Return Bytes
		int BlockSize(PixelFormatEnum ePixelFormat)
		{
			return PixelFormatEnumInfos[ePixelFormat].iBlockSize;
		}

		int ComponentCount(PixelFormatEnum ePixelFormat)
		{
			return PixelFormatEnumInfos[ePixelFormat].iComponentCount;
		}

		void GetBlockCount(PixelFormatEnum ePixelFormat, uint32_t iWidth, uint32_t iHeight, uint32_t* pOutCountX, uint32_t* pOutCountY)
		{
			if (pOutCountX != NULL)
			{
				const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];
				uint32_t iBlockCount = ((iWidth + (oInfos.iBlockWidth - 1)) / oInfos.iBlockWidth);
				if (iBlockCount == 0)
					iBlockCount = 1;
				*pOutCountX = iBlockCount;
			}

			if (pOutCountY != NULL)
			{
				const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];
				uint32_t iBlockCount = ((iHeight + (oInfos.iBlockHeight - 1)) / oInfos.iBlockHeight);
				if (iBlockCount == 0)
					iBlockCount = 1;
				*pOutCountY = iBlockCount;
			}
		}

		uint32_t GetPitch(PixelFormatEnum ePixelFormat, uint32_t iWidth)
		{
			const PixelFormatInfos& oInfos = PixelFormatEnumInfos[ePixelFormat];
			uint32_t iBlockCount = ((iWidth + (oInfos.iBlockWidth - 1)) / oInfos.iBlockWidth);
			if (iBlockCount == 0)
				iBlockCount = 1;
			return iBlockCount * oInfos.iBlockSize;
		}

		static bool s_bConvertionMatrixInit = false;

		typedef struct
		{
			ConvertionFunc pFunc;
			int iAdditionalBits;
		} ConvertionFuncDeclaration;

		ConvertionFuncDeclaration s_pConvertionMatrix[PixelFormatEnum::_COUNT][PixelFormatEnum::_COUNT] = {};

		void InitConvertionMatrix()
		{
			if (s_bConvertionMatrixInit)
				return;

			s_bConvertionMatrixInit = true;

			memset(s_pConvertionMatrix, 0, PixelFormatEnum::_COUNT * PixelFormatEnum::_COUNT * sizeof(ConvertionFunc));

			// RGB8 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_RGB8_To_RGBA8, 8 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_RGBA8_To_RGB8, -8 };

			// R8 <=> RGB8
			s_pConvertionMatrix[PixelFormatEnum::R8_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_R8_To_RGB8, 16 };
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::R8_UNORM] = { Converters::Convert_RGB8_To_R8, -16 };

			// RG8 <=> RGB8
			s_pConvertionMatrix[PixelFormatEnum::RG8_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_RG8_To_RGB8, 8 };
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::RG8_UNORM] = { Converters::Convert_RGB8_To_RG8, -8 };

			// BGR8 <=> RGB8
			s_pConvertionMatrix[PixelFormatEnum::BGR8_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_BGR8_To_RGB8, 1 }; // Not added bits but different
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::BGR8_UNORM] = { Converters::Convert_RGB8_To_BGR8, 1 };

			// BGRA8 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::BGRA8_UNORM][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_BGRA8_To_RGBA8, 1 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::BGRA8_UNORM] = { Converters::Convert_RGBA8_To_BGRA8, 1 };

			// RGB565 <=> RGB8
			s_pConvertionMatrix[PixelFormatEnum::R5G6B5_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_RGB565_To_RGB8, 8 };
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::R5G6B5_UNORM] = { Converters::Convert_RGB8_To_RGB565, -8 };

			// BGR565 <=> RGB8
			s_pConvertionMatrix[PixelFormatEnum::B5G6BR_UNORM][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_BGR565_To_RGB8, 8 };
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::B5G6BR_UNORM] = { Converters::Convert_RGB8_To_BGR565, -8 };

			// RGBA8 <=> RGBA16
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::RGBA16_UNORM] = { (ConvertionFunc)Converters::Convert_RGBA8_To_RGBA16, 64 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA16_UNORM][PixelFormatEnum::RGBA8_UNORM] = { (ConvertionFunc)Converters::Convert_RGBA16_To_RGBA8, -64 };

			// RGB16 <=> RGBA16
			s_pConvertionMatrix[PixelFormatEnum::RGB16_UNORM][PixelFormatEnum::RGBA16_UNORM] = { (ConvertionFunc)Converters::Convert_RGB16_To_RGBA16, 16 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA16_UNORM][PixelFormatEnum::RGB16_UNORM] = { (ConvertionFunc)Converters::Convert_RGBA16_To_RGB16, -16 };

			// RGB16F <=> RGB32F
			s_pConvertionMatrix[PixelFormatEnum::RGB16_FLOAT][PixelFormatEnum::RGB32_FLOAT] = { Converters::Convert_RGB16F_To_RGB32F, 48 };
			s_pConvertionMatrix[PixelFormatEnum::RGB32_FLOAT][PixelFormatEnum::RGB16_FLOAT] = { Converters::Convert_RGB32F_To_RGB16F, -48 };

			// RGBA16F <=> RGBA32F
			s_pConvertionMatrix[PixelFormatEnum::RGBA16_FLOAT][PixelFormatEnum::RGBA32_FLOAT] = { Converters::Convert_RGBA16F_To_RGBA32F, 64 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA32_FLOAT][PixelFormatEnum::RGBA16_FLOAT] = { Converters::Convert_RGBA32F_To_RGBA16F, -64 };

			// R32F <=> RG32F
			s_pConvertionMatrix[PixelFormatEnum::R32_FLOAT][PixelFormatEnum::RG32_FLOAT] = { Converters::Convert_R32F_To_RG32F, 32 };
			s_pConvertionMatrix[PixelFormatEnum::RG32_FLOAT][PixelFormatEnum::R32_FLOAT] = { Converters::Convert_RG32F_To_R32F, -32 };

			// RG32F <=> RGB32F
			s_pConvertionMatrix[PixelFormatEnum::RG32_FLOAT][PixelFormatEnum::RGB32_FLOAT] = { Converters::Convert_RG32F_To_RGB32F, 32 };
			s_pConvertionMatrix[PixelFormatEnum::RGB32_FLOAT][PixelFormatEnum::RG32_FLOAT] = { Converters::Convert_RGB32F_To_RG32F, -32 };

			// RGB32F <=> RGBA32F
			s_pConvertionMatrix[PixelFormatEnum::RGB32_FLOAT][PixelFormatEnum::RGBA32_FLOAT] = { Converters::Convert_RGB32F_To_RGBA32F, 32 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA32_FLOAT][PixelFormatEnum::RGB32_FLOAT] = { Converters::Convert_RGBA32F_To_RGB32F, -32 };

			// RGBA16 => RGBA16F
			s_pConvertionMatrix[PixelFormatEnum::RGBA16_UNORM][PixelFormatEnum::RGBA16_FLOAT] = { Converters::Convert_RGBA16_To_RGBA16F, -1 }; //Losing quality?

			// RGB16F <=> RGBA16F
			s_pConvertionMatrix[PixelFormatEnum::RGB16_FLOAT][PixelFormatEnum::RGBA16_FLOAT] = { Converters::Convert_RGB16F_To_RGBA16F, 16 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA16_FLOAT][PixelFormatEnum::RGB16_FLOAT] = { Converters::Convert_RGBA16F_To_RGB16F, -16 };

			// RGBA8 <=> RGBA32F
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::RGBA32_FLOAT] = { Converters::Convert_RGBA8_To_RGBA32F, 96 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA32_FLOAT][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_RGBA32F_To_RGBA8, -96 };

			// RGB8 >=> RGB32F
			s_pConvertionMatrix[PixelFormatEnum::RGB8_UNORM][PixelFormatEnum::RGB32_FLOAT] = { Converters::Convert_RGB8_To_RGB32F, 72 };
			s_pConvertionMatrix[PixelFormatEnum::RGB32_FLOAT][PixelFormatEnum::RGB8_UNORM] = { Converters::Convert_RGB32F_To_RGB8, -72 };

			// R10G10B10A2 <=> RGBA32F
			s_pConvertionMatrix[PixelFormatEnum::R10G10B10A2_UNORM][PixelFormatEnum::RGBA32_FLOAT] = { Converters::Convert_R10G10B10A2_To_RGBA32F, 96 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA32_FLOAT][PixelFormatEnum::R10G10B10A2_UNORM] = { Converters::Convert_RGBA32F_To_R10G10B10A2, -96 };

			// R9G9B9E5 => RGB32F
			s_pConvertionMatrix[PixelFormatEnum::R9G9B9E5_SHAREDEXP][PixelFormatEnum::RGB32_FLOAT] = { Converters::Convert_R9G9B9E5_To_RGB32F, 64 };

			// BC1 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::BC1][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_BC1_To_RGBA8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::BC1] = { Converters::Convert_RGBA8_To_BC1, -28 };

			// BC2 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::BC2][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_BC2_To_RGBA8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::BC2] = { Converters::Convert_RGBA8_To_BC2, -28 };

			// BC3 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::BC3][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_BC3_To_RGBA8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::BC3] = { Converters::Convert_RGBA8_To_BC3, -28 };

			// BC4 <=> R8
			s_pConvertionMatrix[PixelFormatEnum::BC4][PixelFormatEnum::R8_UNORM] = { Converters::Convert_BC4_To_R8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::R8_UNORM][PixelFormatEnum::BC4] = { Converters::Convert_R8_To_BC4, -28 };

			// BC5 <=> RG8
			s_pConvertionMatrix[PixelFormatEnum::BC5][PixelFormatEnum::RG8_UNORM] = { Converters::Convert_BC5_To_RG8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RG8_UNORM][PixelFormatEnum::BC5] = { Converters::Convert_RG8_To_BC5, -28 };

			// BC6H <=> RGB16F
			s_pConvertionMatrix[PixelFormatEnum::BC6H][PixelFormatEnum::RGB16_FLOAT] = { Converters::Convert_BC6H_To_RGB16F, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RGB16_FLOAT][PixelFormatEnum::BC6H] = { Converters::Convert_RGB16F_To_BC6H, -28 };

			// BC7 <=> RGBA8
			s_pConvertionMatrix[PixelFormatEnum::BC7][PixelFormatEnum::RGBA8_UNORM] = { Converters::Convert_BC7_To_RGBA8, 28 };
			s_pConvertionMatrix[PixelFormatEnum::RGBA8_UNORM][PixelFormatEnum::BC7] = { Converters::Convert_RGBA8_To_BC7, -28 };
		}

		bool GetConvertionChain(PixelFormatEnum eSourcePixelFormat, PixelFormatEnum eDestPixelFormat, ConvertionFuncChain* pOutChain, int* pOutChainLength, int* pOutAdditionalBits)
		{
			InitConvertionMatrix();

			CORE_ASSERT(pOutChain != NULL && pOutChainLength != NULL && pOutAdditionalBits != NULL);

			if (pOutChain == NULL || pOutChainLength == NULL || pOutAdditionalBits == NULL)
				return false;

			memset(pOutChain, 0, sizeof(ConvertionFuncChain));

			if (s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].pFunc != NULL)
			{
				(*pOutChain)[0].pFunc = s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].pFunc;
				(*pOutChain)[0].eFormat = eDestPixelFormat;
				*pOutChainLength = 1;
				*pOutAdditionalBits = s_pConvertionMatrix[eSourcePixelFormat][eDestPixelFormat].iAdditionalBits;
				return true;
			}

			PixelFormatEnum eCurrentFormat = eSourcePixelFormat;
			PixelFormatEnum eChain[32] = { PixelFormatEnum::_NONE };
			int iCurrentChain = 0;

			while (iCurrentChain < 32)
			{
				ConvertionFuncDeclaration* pInfos = s_pConvertionMatrix[eCurrentFormat];

				PixelFormatEnum eBestFormat = PixelFormatEnum::_NONE;
				int iBestAddBits = -999;
				if (pInfos[eDestPixelFormat].pFunc != NULL)
				{
					eBestFormat = eDestPixelFormat;
				}
				else
				{
					for (int i = 0; i < PixelFormatEnum::_COUNT; ++i)
					{
						if (pInfos[i].pFunc != NULL)
						{
							bool bAlreadyInChain = false;
							for (int iChain = 0; iChain < iCurrentChain; ++iChain)
							{
								if (eChain[iChain] == i)
								{
									bAlreadyInChain = true;
									break;
								}
							}

							if (bAlreadyInChain)
								continue;

							if (eBestFormat == PixelFormatEnum::_NONE || pInfos[i].iAdditionalBits >= 0)
							{
								eBestFormat = (PixelFormatEnum)i;
								if (pInfos[i].iAdditionalBits == 0)
									break;
							}
						}
					}
				}

				if (eBestFormat != PixelFormatEnum::_NONE)
				{
					eChain[iCurrentChain] = eBestFormat;
					++iCurrentChain;
					eCurrentFormat = eBestFormat;
					if (eBestFormat == eDestPixelFormat)
					{
						break;
					}
					continue;
				}

				break;
			}

			if (iCurrentChain > 0 && eChain[iCurrentChain - 1] == eDestPixelFormat)
			{
				int iAdditionalBits = 0;
				eCurrentFormat = eSourcePixelFormat;
				for (int i = 0; i < iCurrentChain; ++i)
				{
					ConvertionFuncDeclaration& oInfo = s_pConvertionMatrix[eCurrentFormat][eChain[i]];
					(*pOutChain)[i].pFunc = oInfo.pFunc;
					(*pOutChain)[i].eFormat = eChain[i];
					iAdditionalBits += oInfo.iAdditionalBits;
					eCurrentFormat = eChain[i];
				}
				*pOutChainLength = iCurrentChain;
				*pOutAdditionalBits = iAdditionalBits;
				return true;
			}

			return false;
		}

		int GetAvailableConvertion(PixelFormatEnum eSourcePixelFormat, bool bIncludeChains, ConvertionInfoList* pOutAvailablePixelFormat)
		{
			InitConvertionMatrix();
			memset(pOutAvailablePixelFormat, 0, sizeof(ConvertionInfoList));
			int iCurrentOutIndex = 0;

			Graphics::PixelFormat::ConvertionFuncChain oConvertionFuncChain;
			int iConvertionChainLength;
			int iAdditionalBits;

			ConvertionFuncDeclaration* pConvertionList = s_pConvertionMatrix[eSourcePixelFormat];
			for (int i = 0; i < PixelFormatEnum::_COUNT; ++i)
			{
				if (i == eSourcePixelFormat)
					continue;

				if (pConvertionList[i].pFunc != NULL) //TODO check bLosingQuality
				{
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].eFormat = (PixelFormatEnum)i;
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].iAdditionalBits = pConvertionList[i].iAdditionalBits;
					++iCurrentOutIndex;
				}
				else if (bIncludeChains && GetConvertionChain(eSourcePixelFormat, (PixelFormatEnum)i, &oConvertionFuncChain, &iConvertionChainLength, &iAdditionalBits))
				{
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].eFormat = (PixelFormatEnum)i;
					(*pOutAvailablePixelFormat)[iCurrentOutIndex].iAdditionalBits = iAdditionalBits;
					++iCurrentOutIndex;
				}
			}
			return iCurrentOutIndex;
		}
	}
}
