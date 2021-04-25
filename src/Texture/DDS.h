// Refs
// https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-pguide
// https://docs.microsoft.com/en-us/windows/desktop/direct3d10/d3d10-graphics-programming-guide-resources-block-compression
// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/texture-block-compression-in-direct3d-11
// https://docs.microsoft.com/en-us/windows/desktop/direct3d9/opaque-and-1-bit-alpha-textures
// https://github.com/HeliumProject/NvidiaTextureTools/blob/master/src/nvimage/BlockDXT.cpp
// https://github.com/baldurk/renderdoc/blob/v1.x/renderdoc/common/dds_readwrite.cpp

#ifndef __TEXTURE_DDS_H__
#define __TEXTURE_DDS_H__

#include <stdint.h> //uint32_t

#pragma pack(push,1)

#ifndef DXGI_FORMAT_DEFINED

typedef enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
} DXGI_FORMAT;

#endif //DXGI_FORMAT_DEFINED

#ifndef _D3D10_CONSTANTS

typedef enum D3D10_RESOURCE_DIMENSION
{
	D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
	D3D10_RESOURCE_DIMENSION_BUFFER = 1,
	D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
	D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
	D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
} D3D10_RESOURCE_DIMENSION;

#endif //_D3D10_CONSTANTS

typedef struct
{
	uint32_t  iSize;
	uint32_t  iFlags;
	uint32_t  iFourCC;
	uint32_t  iRGBBitCount;
	uint32_t  iRBitMask;
	uint32_t  iGBitMask;
	uint32_t  iBBitMask;
	uint32_t  iABitMask;
} DDS_PIXELFORMAT;

typedef struct
{
	uint32_t iSize;
	uint32_t iHeaderFlags;
	uint32_t iHeight;
	uint32_t iWidth;
	uint32_t iPitchOrLinearSize;
	uint32_t iDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
	uint32_t iMipMapCount;
	uint32_t iReserved1[11];
	DDS_PIXELFORMAT oPixelFormat;
	uint32_t iSurfaceFlags;
	uint32_t iCubemapFlags;
	uint32_t iReserved2[3];
} DDS_HEADER;

typedef struct
{
	DXGI_FORMAT oDxgiFormat;
	D3D10_RESOURCE_DIMENSION oResourceDimension;
	uint32_t iMiscFlag;
	uint32_t iArraySize;
	uint32_t iReserved;
} DDS_HEADER_DXT10;

static_assert(sizeof(DDS_HEADER) == 124, "DDS Header size mismatch");
static_assert(sizeof(DDS_HEADER_DXT10) == 20, "DDS DX10 Extended Header size mismatch");

#ifndef MAKEFOURCC
#   define MAKEFOURCC(ch0, ch1, ch2, ch3) ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

const uint32_t DDS_MAGIC                              = 0x20534444; // "DDS "

const uint32_t DDS_HEADER_FLAGS_CAPS                  = 0x00000001; // DDSD_CAPS
const uint32_t DDS_HEADER_FLAGS_HEIGHT                = 0x00000002; // DDSD_HEIGHT
const uint32_t DDS_HEADER_FLAGS_WIDTH                 = 0x00000004; // DDSD_WIDTH
const uint32_t DDS_HEADER_FLAGS_PITCH                 = 0x00000008; // DDSD_PITCH
const uint32_t DDS_HEADER_FLAGS_PIXELFORMAT           = 0x00001000; // DDSD_PIXELFORMAT
const uint32_t DDS_HEADER_FLAGS_MIPMAPCOUNT           = 0x00020000; // DDSD_MIPMAPCOUNT
const uint32_t DDS_HEADER_FLAGS_LINEARSIZE            = 0x00080000; // DDSD_LINEARSIZE
const uint32_t DDS_HEADER_FLAGS_DEPTH                 = 0x00800000; // DDSD_DEPTH

const uint32_t DDS_HEADER_FLAGS_TEXTURE               = 0x00001007; // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT

const uint32_t DDS_SURFACE_FLAGS_TEXTURE              = 0x00001000; // DDSCAPS_TEXTURE
const uint32_t DDS_SURFACE_FLAGS_MIPMAP               = 0x00400008; // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
const uint32_t DDS_SURFACE_FLAGS_CUBEMAP              = 0x00000008; // DDSCAPS_COMPLEX
const uint32_t DDS_SURFACE_FLAGS_VOLUME               = 0x00200008; // DDSCAPS_COMPLEX | DDSCAPS2_VOLUME

const uint32_t DDS_FOURCC                             = 0x00000004;  // DDPF_FOURCC
const uint32_t DDS_RGB                                = 0x00000040;  // DDPF_RGB
const uint32_t DDS_RGBA                               = 0x00000041;  // DDPF_RGB | DDPF_ALPHAPIXELS
const uint32_t DDS_LUMINANCE                          = 0x00020000;  // DDPF_LUMINANCE
const uint32_t DDS_ALPHA                              = 0x00000002;  // DDPF_ALPHA

const uint32_t DDS_CUBEMAP_POSITIVEX                  = 0x00000600; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
const uint32_t DDS_CUBEMAP_NEGATIVEX                  = 0x00000a00; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
const uint32_t DDS_CUBEMAP_POSITIVEY                  = 0x00001200; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
const uint32_t DDS_CUBEMAP_NEGATIVEY                  = 0x00002200; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
const uint32_t DDS_CUBEMAP_POSITIVEZ                  = 0x00004200; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
const uint32_t DDS_CUBEMAP_NEGATIVEZ                  = 0x00008200; // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

const uint32_t DDS_CUBEMAP_ALLFACES                   = ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |
                                                          DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |
                                                          DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ );

const uint32_t DDS_FLAGS_VOLUME                       = 0x00200000; // DDSCAPS2_VOLUME

const DDS_PIXELFORMAT DDSPF_DXT1                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 }; // BC1
const DDS_PIXELFORMAT DDSPF_DXT2                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 }; // BC2
const DDS_PIXELFORMAT DDSPF_DXT3                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 }; // BC2
const DDS_PIXELFORMAT DDSPF_DXT4                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 }; // BC3
const DDS_PIXELFORMAT DDSPF_DXT5                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 }; // BC3
const DDS_PIXELFORMAT DDSPF_BC5U                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','5','U'), 0, 0, 0, 0, 0 }; // BC5
const DDS_PIXELFORMAT DDSPF_BC6H                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','6','H'), 0, 0, 0, 0, 0 }; // BC6H
const DDS_PIXELFORMAT DDSPF_ATI2                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A','T','I','2'), 0, 0, 0, 0, 0 }; // BC5
const DDS_PIXELFORMAT DDSPF_A8R8G8B8                  = { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };
const DDS_PIXELFORMAT DDSPF_A1R5G5B5                  = { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 };
const DDS_PIXELFORMAT DDSPF_A4R4G4B4                  = { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 };
const DDS_PIXELFORMAT DDSPF_R8G8B8                    = { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 };
const DDS_PIXELFORMAT DDSPF_R8G8B8A8                  = { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };
const DDS_PIXELFORMAT DDSPF_B8G8R8                    = { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 };
const DDS_PIXELFORMAT DDSPF_R5G6B5                    = { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000001f, 0x000007e0, 0x0000f800, 0x00000000 };
const DDS_PIXELFORMAT DDSPF_B5G6R5                    = { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 };
// This indicates the DDS_HEADER_DXT10 extension is present (the format is in dxgiFormat)
const DDS_PIXELFORMAT DDSPF_DX10                      = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','1','0'), 0, 0, 0, 0, 0 };
const DDS_PIXELFORMAT DDSPF_R16G16B16A16_UNORM        = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 36, 0, 0, 0, 0, 0 };
const DDS_PIXELFORMAT DDSPF_R16G16B16A16_FLOAT        = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 113, 0, 0, 0, 0, 0 }; // D3DFMT_A16B16G16R16F
const DDS_PIXELFORMAT DDSPF_R32_FLOAT                 = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 114, 0, 0, 0, 0, 0 }; // D3DFMT_R32F
const DDS_PIXELFORMAT DDSPF_R32G32_FLOAT              = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 115, 0, 0, 0, 0, 0 }; // D3DFMT_G32R32F
const DDS_PIXELFORMAT DDSPF_R32G32B32A32_FLOAT        = { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 116, 0, 0, 0, 0, 0 }; // D3DFMT_A32B32G32R32F

static_assert((0xA0B70708 & 0xFF) == 8, "Platform is not LittleEndian");

#pragma pack(pop)

#endif // __TEXTURE_DDS_H__
