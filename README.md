# Texeled

[![Build status](https://ci.appveyor.com/api/projects/status/4oxhg62kiwrglghb/branch/master?svg=true)](https://ci.appveyor.com/project/thennequin/texeled/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3994ad06b426497191628431c945fa5e)](https://www.codacy.com/app/thennequin/Texeled?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=thennequin/Texeled&amp;utm_campaign=Badge_Grade)

Simple texture viewer and editor

![Demo](https://raw.githubusercontent.com/wiki/thennequin/Texeled/images/Texeled.gif)

Support:
- Features:
  - Fast launch
  - Small (less than 1 MB)
  - Conversion beetween pixel formats
  - Create simple mip maps
    
- Loading:
  - DDS
  - PNG (via **stb_image**)
  - JPG (via **stb_image**)
  - TGA (via **stb_image**)
  - BMP (via **stb_image**)
  - EXT (via **tinyexr**)
    
- Writing:
  - DDS
  - PNG
  
- Pixel formats:
  - R8 UNorm
  - RG8 UNorm

  - RGB8 UNorm
  - BGR8 UNorm

  - RGBA8 UNorm
  - BGRA8 UNorm

  - R5G6B5 UNorm
  - B5G6R5 UNorm

  - RGB16 SHalf

  - RGBA16 UNorm
  - RGBA16 Half

  - R32 Float
  - RG32 Float
  - RGB32 Float
  - RGBA32 Float

  - R10G10B10A2 UNorm

  - R9G9B9E5 Float (shared exp)

  - BC1 (DXT1)
  - BC2 (DXT2/3) *
  - BC3 (DXT4/5) *
  
  \* Display only, no conversion
  
  ## TODO
  - Improving BC1 compression
  - Adding BC2/3 compression
  - Adding support of BC4/5/6H/7
