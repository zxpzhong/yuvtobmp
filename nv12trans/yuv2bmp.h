
#ifndef _YUV2BMP_H_
#define _YUV2BMP_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(2)

#ifndef NULL
#define NULL ((void *)0)
#endif

    typedef unsigned char U8;
    typedef unsigned short int U16;
    typedef unsigned long int U32;

    /* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define BITMAP_HEADER_TYPE "BM"

    typedef struct tagBitmapFileHeader
    {
        U16 bfType;
        U32 bfSize;
        U16 bfReserved1;
        U16 bfReserved2;
        U32 bfOffBits;
    } BitmapFileHeader;

    typedef struct tagBitmapInfoHeader
    {
        U32 biSize;
        U32 biWidth;
        U32 biHeight;
        U16 biPlanes;
        U16 biBitCount;
        U32 biCompression;
        U32 biSizeImage;
        U32 biXPelsPerMeter;
        U32 biYPelsPerMeter;
        U32 biClrUsed;
        U32 biClrImportant;
    } BitmapInfoHeader;

    typedef struct tagBitmapInfo
    {
        BitmapFileHeader bfHeader;
        BitmapInfoHeader biHeader;
    } BitmapInfo;

    typedef struct tagRGBQuad
    {
        U8 rgbBlue;
        U8 rgbGreen;
        U8 rgbRed;
        U8 rgbReserved;
    } RGBQuad;

    typedef union tagBitmapData
    {
        RGBQuad rgbQuad[0];
        U8 rgb[0];
    } BitmapData;

    typedef struct tagBitmap
    {
        BitmapInfo bInfo;
        BitmapData bData;
    } Bitmap;

    typedef enum
    {
        BMP_RGB1555 = 0,
        BMP_RGB888 = 1,
        BMP_RGB8888 = 2,
        BMP_RGB565 = 3,
    }BMP_TYPE_E;

    /**
    Convert YUV raw data to RGB bitmap.
    @return: bitmap raw data or NULL if any error.
    */
    Bitmap * yuv2bmp(int bmp_type, char * bmp_buf, int size, U8 yuv[], int width, int height);


    /**
    Convert YUV raw data to RGB bitmap and save to <file>.
    @return: bmp file size or 0 if any error.
    */
    int yuv2bmpfile(int type, U8 yuv[], int width, int height, const char * file);

#ifdef __cplusplus
}
#endif

#endif /* _YUV2BMP_H_ */

