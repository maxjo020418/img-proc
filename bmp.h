#ifdef BMP_H
    // 호옥시나 하니
    #pragma message("Warning: BMP.h 이미 존재함!")
#endif

#ifndef BMP_H
#define BMP_H

#include <cstdint>

/*
The original Windows headers define BITMAPINFOHEADER and RGBQUAD 
in a way that matches the BMP file layout (often with internal packing already handled).

When you create your own definitions on Linux, 
you must ensure that these structures are packed 
(using #pragma pack(push, 1) / #pragma pack(pop) or equivalent attributes) 
so that no extra padding is added. 

Otherwise, the layout might not match the BMP file, 
and reading the file directly into the structure would result in incorrect out.
*/

// 헤더니까 패딩없이 struct 바이트 우겨넣기
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
} RGBQUAD;

typedef unsigned char BYTE;

#endif
