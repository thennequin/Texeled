#ifndef __GRAPHICS_TEXTURE_LOADER_DDS_H__
#define __GRAPHICS_TEXTURE_LOADER_DDS_H__

#include "Graphics/Texture.h"

namespace Graphics
{
	namespace TextureLoader
	{
		void RegisterLoaderDDS();
		bool TextureLoaderDDS(Core::Stream* pStream, Texture* pTexture);
	}
	//namespace TextureLoader
}
//namespace Graphics

#endif //__GRAPHICS_TEXTURE_LOADER_DDS_H__