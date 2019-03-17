# Texeled

[![Build status](https://ci.appveyor.com/api/projects/status/4oxhg62kiwrglghb/branch/master?svg=true)](https://ci.appveyor.com/project/thennequin/texeled/branch/master)

Simple texture viewer and editor

![Demo](https://raw.githubusercontent.com/wiki/thennequin/Texeled/images/Texeled.gif)

Support:
  * Loading:
    * DDS
    * PNG (via **stb_image**)
    * JPG (via **stb_image**)
    * TGA (via **stb_image**)
    * BMP (via **stb_image**)
    
  * Writing:
    * DDS
    
  * Pixel formats:
    * R8 UNorm
    * RG8 UNorm

    * RGB8 UNorm
    * BGR8 UNorm

    * RGBA8 UNorm
    * BGRA8 UNorm

    * R5G6B5 UNorm
    * B5G6R5 UNorm

    * RGB16 SHalf

    * RGBA16 UNorm
    * RGBA16 Half

    * RGB32 Float
    * RGBA32 Float

    * R10G10B10A2 UNorm

    * R9G9B9E5 Float (shared exp)

    * BC1 (DXT1)
    * BC2 (DXT2/3)
    * BC3 (DXT4/5)
    * BC4
    * BC5
    * BC6H
    * BC7
