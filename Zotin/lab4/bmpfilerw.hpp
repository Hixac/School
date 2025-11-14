#pragma once

#include <fstream>
#include <inttypes.h>

// Переименованные структуры для BMP файлов чтобы избежать конфликтов
#pragma pack(push, 1)
struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t bfReserved;
    uint32_t bfOffBits;
};

struct RGBTRIPLE {
    uint8_t rgbtBlue;
    uint8_t rgbtGreen;
    uint8_t rgbtRed;
};

struct BITMAPINFOHEADER {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

void BMPRead(RGBTRIPLE**&, BITMAPFILEHEADER&, BITMAPINFOHEADER&, const char*);
void BMPWrite(RGBTRIPLE**& rgb, int imWidth, int imHeight, const char* fout);

unsigned char get_row_data_padding(unsigned int width);
unsigned int bmp24b_file_size_calc(unsigned int width, unsigned int height);

