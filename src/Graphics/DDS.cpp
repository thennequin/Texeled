#include "Graphics/DDS.h"

// Refs
// https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-pguide
// https://docs.microsoft.com/en-us/windows/desktop/direct3d10/d3d10-graphics-programming-guide-resources-block-compression
// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/texture-block-compression-in-direct3d-11
// https://docs.microsoft.com/en-us/windows/desktop/direct3d9/opaque-and-1-bit-alpha-textures
// https://github.com/HeliumProject/NvidiaTextureTools/blob/master/src/nvimage/BlockDXT.cpp
// https://github.com/baldurk/renderdoc/blob/v1.x/renderdoc/common/dds_readwrite.cpp

//Convert Big endian to Little
//i32 = ((i32 & 0x000000ff) << 24u) | ((i32 & 0x0000ff00) << 8u) | ((i32 & 0x00ff0000) >> 8u) | ((i32 & 0xff000000) >> 24u);

inline void BGR565ToRGBA888(B5G6R5 i565, RGBA8888* p8888)
{
	p8888->b = (i565.b << 3) | (i565.b >> 2);
	p8888->g = (i565.g << 2) | (i565.g >> 4);
	p8888->r = (i565.r << 3) | (i565.r >> 2);
}

inline void ColorLerp(const RGBA8888& iColorA, const RGBA8888& iColorB, RGBA8888* pOut, int iMulA, int iMulB, int iDiv, int iAdd)
{
	pOut->r = (iColorA.r * iMulA + iColorB.r * iMulB + iAdd) / iDiv;
	pOut->g = (iColorA.g * iMulA + iColorB.g * iMulB + iAdd) / iDiv;
	pOut->b = (iColorA.b * iMulA + iColorB.b * iMulB + iAdd) / iDiv;
}

int DecodeBlockBC1(BlockBC1* pBlock, uint8_t* pOut4x4RGBA)
{
	RGBA8888 iColors[4];
	BGR565ToRGBA888(pBlock->iColor[0], &iColors[0]);
	BGR565ToRGBA888(pBlock->iColor[1], &iColors[1]);
	iColors[0].a = iColors[1].a = 255;

	int iComp = 3;

	if (pBlock->iColor[0].u > pBlock->iColor[1].u)
	{
		ColorLerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
		ColorLerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);

		iColors[2].a = iColors[3].a = 255;
	}
	else
	{
		iColors[2].r = (iColors[0].r + iColors[1].r) / 2;
		iColors[2].g = (iColors[0].g + iColors[1].g) / 2;
		iColors[2].b = (iColors[0].b + iColors[1].b) / 2;

		//iColors[3] = iColors[2]; // More sense but not in specs
		iColors[3].r = iColors[3].g = iColors[3].b = 0;
		iColors[3].a = 0;

		iComp = 4;
	}

	RGBA8888* pOut = (RGBA8888*)pOut4x4RGBA;

	for (int i = 0; i < 16; ++i)
	{
		int y = i / 4;
		int x = i - y * 4;
		RGBA8888& oOutPixel = pOut[y * 4 + x];
		int iColorIndex = pBlock->iPixelIndex[i];
		RGBA8888& oInPixel = iColors[iColorIndex];
		oOutPixel = oInPixel;
	}

	return iComp;
}

void DecodeBlockBC2(BlockBC2* pBlock, uint8_t* pOut4x4RGBA)
{
	RGBA8888 iColors[4];
	BGR565ToRGBA888(pBlock->iColor[0], &iColors[0]);
	BGR565ToRGBA888(pBlock->iColor[1], &iColors[1]);

	ColorLerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
	ColorLerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);

	RGBA8888* pOut = (RGBA8888*)pOut4x4RGBA;
	for (int i = 0; i < 16; ++i)
	{
		RGBA8888& oOutPixel = pOut[i];
		RGBA8888& oInPixel = iColors[pBlock->iPixelIndex[i]];
		oOutPixel.r = oInPixel.r;
		oOutPixel.g = oInPixel.g;
		oOutPixel.b = oInPixel.b;
		oOutPixel.a = pBlock->iAlpha[i] * 255 / 15;
	}
}

void DecodeBlockBC3(BlockBC3* pBlock, uint8_t* pOut4x4RGBA)
{
	RGBA8888 iColors[4];
	BGR565ToRGBA888(pBlock->iColor[0], &iColors[0]);
	BGR565ToRGBA888(pBlock->iColor[1], &iColors[1]);

	ColorLerp(iColors[0], iColors[1], &iColors[2], 2, 1, 3, 0);
	ColorLerp(iColors[0], iColors[1], &iColors[3], 1, 2, 3, 0);


	uint8_t iAlphas[8];
	iAlphas[0] = pBlock->iAlpha[0];
	iAlphas[1] = pBlock->iAlpha[1];

	iAlphas[2] = (iAlphas[0] * 6 + iAlphas[1] * 1) / 7;
	iAlphas[3] = (iAlphas[0] * 5 + iAlphas[1] * 2) / 7;
	iAlphas[4] = (iAlphas[0] * 4 + iAlphas[1] * 3) / 7;
	iAlphas[5] = (iAlphas[0] * 3 + iAlphas[1] * 4) / 7;
	iAlphas[6] = (iAlphas[0] * 2 + iAlphas[1] * 5) / 7;
	iAlphas[6] = (iAlphas[0] * 1 + iAlphas[1] * 6) / 7;

	RGBA8888* pOut = (RGBA8888*)pOut4x4RGBA;
	for (int i = 0; i < 16; ++i)
	{
		RGBA8888& oOutPixel = pOut[i];
		RGBA8888& oInPixel = iColors[pBlock->iPixelIndex[i]];
		oOutPixel.r = oInPixel.r;
		oOutPixel.g = oInPixel.g;
		oOutPixel.b = oInPixel.b;
		oOutPixel.a = iAlphas[pBlock->iAlphaIndex[i]];
	}
}
