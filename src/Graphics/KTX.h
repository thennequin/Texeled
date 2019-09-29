#ifndef _GRAPHICS_KTX_H_
#define _GRAPHICS_KTX_H_

/*
https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
Byte[12] identifier
UInt32 endianness
UInt32 glType
UInt32 glTypeSize
UInt32 glFormat
Uint32 glInternalFormat
Uint32 glBaseInternalFormat
UInt32 pixelWidth
UInt32 pixelHeight
UInt32 pixelDepth
UInt32 numberOfArrayElements
UInt32 numberOfFaces
UInt32 numberOfMipmapLevels
UInt32 bytesOfKeyValueData

for each keyValuePair that fits in bytesOfKeyValueData
	UInt32   keyAndValueByteSize
	Byte     keyAndValue[keyAndValueByteSize]
	Byte     valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
end

for each mipmap_level in numberOfMipmapLevels1
	UInt32 imageSize;
	for each array_element in numberOfArrayElements2
	   for each face in numberOfFaces3
		   for each z_slice in pixelDepth2
			   for each row or row_of_blocks in pixelHeight2
				   for each pixel or block_of_pixels in pixelWidth
					   Byte data[format-specific-number-of-bytes]4
				   end
			   end
		   end
		   Byte cubePadding[0-3]
	   end
	end
	Byte mipPadding[0-3]
end
*/

#include <stdint.h> // uint8_t / uint16_t / uint32_t

namespace KTX
{
	const uint8_t c_pIdentifier[12] = {
				0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
				//'«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n'
	};

	/* DataType */
	struct _DataTypeEnum
	{
		enum Enum : uint32_t
		{
			GL_BYTE = 0x1400,
			GL_UNSIGNED_BYTE = 0x1401,
			GL_SHORT = 0x1402,
			GL_UNSIGNED_SHORT = 0x1403,
			GL_INT = 0x1404,
			GL_UNSIGNED_INT = 0x1405,
			GL_FLOAT = 0x1406,
			GL_2_BYTES = 0x1407,
			GL_3_BYTES = 0x1408,
			GL_4_BYTES = 0x1409,
			GL_DOUBLE = 0x140A
		};
	};
	typedef _DataTypeEnum::Enum DataTypeEnum;

	/* PixelFormat */
	struct _PixelFormatEnum
	{
		enum Enum : uint32_t
		{
			GL_COLOR_INDEX = 0x1900,
			GL_STENCIL_INDEX = 0x1901,
			GL_DEPTH_COMPONENT = 0x1902,
			GL_RED = 0x1903,
			GL_GREEN = 0x1904,
			GL_BLUE = 0x1905,
			GL_ALPHA = 0x1906,
			GL_RGB = 0x1907,
			GL_RGBA = 0x1908,
			GL_LUMINANCE = 0x1909,
			GL_LUMINANCE_ALPHA = 0x190A,

			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1,
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2,
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3
		};
	};
	typedef _PixelFormatEnum::Enum PixelFormatEnum;

	struct KTXHeader
	{
		uint8_t				pIdentifier[12];
		uint32_t			iEndianness;
		DataTypeEnum		iGLType;
		uint32_t			iGLTypeSize;
		PixelFormatEnum		iGLFormat;
		PixelFormatEnum		iGLInternalFormat;
		PixelFormatEnum		iGLBaseInternalFormat;
		uint32_t			iPixelWidth;
		uint32_t			iPixelHeight;
		uint32_t			iPixelDepth;
		uint32_t			iNumberOfArrayElements;
		uint32_t			iNumberOfFaces;
		uint32_t			iNumberOfMipmapLevels;
		uint32_t			iBytesOfKeyValueData;
	};
}

#endif //_GRAPHICS_KTX_H_