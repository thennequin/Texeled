# Texeled

[![Build status](https://ci.appveyor.com/api/projects/status/4oxhg62kiwrglghb/branch/master?svg=true)](https://ci.appveyor.com/project/thennequin/texeled/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3994ad06b426497191628431c945fa5e)](https://www.codacy.com/app/thennequin/Texeled?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=thennequin/Texeled&amp;utm_campaign=Badge_Grade)

Simple texture viewer and editor

![Demo](https://raw.githubusercontent.com/wiki/thennequin/Texeled/images/Texeled.gif)

Support:
- Features:
    - [x] Fast launch
    - [x] Small (less than 1 MB)
    - [x] Conversion beetween pixel formats
    - [x] Create simple mip maps
    
- Loading:
    - [x] DDS
    - [x] KTX (Work In Progress)
    - [x] PNG (via **libpng**)
    - [x] JPG (via **stb_image**)
    - [x] TGA (via **stb_image**)
    - [x] BMP (via **stb_image**)
    - [x] EXT (via **tinyexr**)
    
- Writing:
    - [x] DDS
    - [x] PNG (via **stb_image_write**)
  
- Pixel formats:
    - [x] R8 UNorm
    - [x] RG8 UNorm

    - [x] RGB8 UNorm
    - [x] BGR8 UNorm

    - [x] RGBA8 UNorm
    - [x] BGRA8 UNorm

    - [x] R5G6B5 UNorm
    - [x] B5G6R5 UNorm

    - [x] RGB16 UNorm
    - [x] RGB16 Half

    - [x] RGBA16 UNorm
    - [x] RGBA16 Half

    - [x] R32 Float
    - [x] RG32 Float
    - [x] RGB32 Float
    - [x] RGBA32 Float

    - [x] R10G10B10A2 UNorm

    - [x] R9G9B9E5 Float (shared exp)

    - [x] D16 UNorm

    - [x] BC1 (DXT1) *
    - [x] BC2 (DXT2/3) *
    - [x] BC3 (DXT4/5) *
    - [x] BC4 *
    - [x] BC5 *
    - [x] BC6H * 
    - [x] BC7 *
 
  \* BCX compression/uncompression use Compressonator with default settings
