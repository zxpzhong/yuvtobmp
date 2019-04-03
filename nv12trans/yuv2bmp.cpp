#include "yuv2bmp.h"

#define inline __inline
typedef int(*RGB_FUNC)(U8 * rgb, int r, int g, int b);

typedef struct
{
    U16 b : 5;
    U16 g : 5;
    U16 r : 5;
    U16 a : 1;
} BMP_BIT16_1555;

typedef struct
{
    U16 b : 5;
    U16 g : 6;
    U16 r : 5;
} BMP_BIT16_565;

int v1475871[256] = { 0 };
int v751724[256] = { 0 };
int u362283[256] = { 0 };
int u1865417[256] = { 0 };

void init_convert_table()
{
    int i = 0;
    static int init = 0;

    if (init)
    {
        return;
    }

    for (i = 0; i < 256; i++)
    {
        v1475871[i] = (i - 128) * 1475871;
        v751724[i] = (i - 128) * 751724;
        u362283[i] = (i - 128) * 362283;
        u1865417[i] = (i - 128) * 1865417;
    }

    init = 1;
}

inline int rgb16_1555(U8 * rgb, int r, int g, int b)
{
    BMP_BIT16_1555 * bmp1555 = NULL;

    bmp1555 = (BMP_BIT16_1555 *)rgb;
    bmp1555->a = 1;
    bmp1555->r = r >> 23;
    bmp1555->g = g >> 23;
    bmp1555->b = b >> 23;

    return 2;
}

inline int rgb16_565(U8 * rgb, int r, int g, int b)
{
    BMP_BIT16_565 * bmp565 = NULL;

    bmp565 = (BMP_BIT16_565 *)rgb;
    bmp565->r = r >> 23;
    bmp565->g = g >> 22;    /* g分量用6位表示 */
    bmp565->b = b >> 23;

    return 2;
}

inline int rgb24_888(U8 * rgb, int r, int g, int b)
{
    *rgb++ = b >> 20;
    *rgb++ = g >> 20;
    *rgb++ = r >> 20;

    return 3;
}

inline int rgb32_8888(U8 * rgb, int r, int g, int b)
{
    *rgb++ = b >> 20;
    *rgb++ = g >> 20;
    *rgb++ = r >> 20;
    *rgb++ = 255;

    return 4;
}

/*
R = Y + 1.4075 *（V-128）
G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
B = Y + 1.779 *（U – 128）
*/
void decodeI420(U8 yuv[], int width, int height, int pitch, U8 rgb[], RGB_FUNC func)
{
    int frameSize = width * height;
    int w, h;
    int yp, y, y1m, up, u = 0, vp, v = 0, rgbp, r, g, b;
    BMP_BIT16_1555 * bmp1555 = NULL;

    for (h = height - 1; h >= 0; h--)
    {
        yp = h * width;
        up = frameSize + (h >> 1) * (width >> 1);
        vp = frameSize + (frameSize >> 2) + (h >> 1) * (width >> 1);
        rgbp = (height - h - 1) * pitch;
        for (w = 0; w < width; w++, yp++)
        {
            y = (int)yuv[yp];
            if ((w & 1) == 0)
            {
                u = ((int)yuv[up++]) - 128;
                v = ((int)yuv[vp++]) - 128;
            }

            y1m = y << 20;
            r = (y1m + 1475871 * v);
            g = (y1m - 751724 * v - 362283 * u);
            b = (y1m + 1865417 * u);

            r = (r + 524288);
            g = (g + 524288);
            b = (b + 524288);
            if (r < 0) r = 0;
            else if (r > 268435455) r = 268435455;
            if (g < 0) g = 0;
            else if (g > 268435455) g = 268435455;
            if (b < 0) b = 0;
            else if (b > 268435455) b = 268435455;

            /* 函数调用会慢很多,追求速度慎用 */
            rgbp += func(&rgb[rgbp], r, g, b);
        }
    }
}

U32 get_data_size(int type, int width, int height, U32 * pitch)
{
    U32 dataSize = 0, palette_size = 0, bit = 0;

    if (BMP_RGB1555 == type || BMP_RGB565 == type)
    {
        bit = 16;
        palette_size = 3 * sizeof(int); /* 16位色带有3个int的调色信息 */
    }
    else if (BMP_RGB888 == type)
    {
        bit = 24;
    }
    else
    {
        bit = 32;
    }

    *pitch = ((width * bit + 31) >> 5) << 2; /* size of a row, include pad. */

    return ((*pitch * height) + palette_size);
}

Bitmap * yuv2bmp(int type, char * bmp_buf, int size, U8 yuv[], int width, int height)
{
    int * mask = NULL;
    U32 dataSize = 0, pitch = 0;
    Bitmap * bmp = NULL;
    BitmapInfo * bmpInfo = NULL;

    init_convert_table();

    dataSize = get_data_size(type, width, height, &pitch);
    if ((NULL == bmp_buf) || size < (int)(sizeof(BitmapInfo) + dataSize))
    {
        return NULL;
    }

    bmp = (Bitmap *)bmp_buf;
    memset(bmp, 0, sizeof(BitmapInfo) + dataSize);
    bmpInfo = &bmp->bInfo;

    bmpInfo->bfHeader.bfType = 0x4D42;  /* "BM" */
    bmpInfo->bfHeader.bfSize = sizeof(BitmapInfo) + dataSize;
    bmpInfo->bfHeader.bfReserved1 = 0;
    bmpInfo->bfHeader.bfReserved2 = 0;
    bmpInfo->bfHeader.bfOffBits = sizeof(BitmapInfo);

    bmpInfo->biHeader.biSize = sizeof(BitmapInfoHeader);
    bmpInfo->biHeader.biWidth = width;
    bmpInfo->biHeader.biHeight = height;
    bmpInfo->biHeader.biPlanes = 1;
    bmpInfo->biHeader.biSizeImage = dataSize;
    bmpInfo->biHeader.biXPelsPerMeter = 0;
    bmpInfo->biHeader.biYPelsPerMeter = 0;
    bmpInfo->biHeader.biClrUsed = 0;
    bmpInfo->biHeader.biClrImportant = 0;

    switch (type)
    {
    case BMP_RGB1555:
    {
        /* 16位bmp多了12个字节的调色板信息,用来标识rgb数据位置 */
        bmpInfo->biHeader.biBitCount = 16;
        bmpInfo->biHeader.biCompression = BI_BITFIELDS;
        mask = (int *)&bmp->bData;
        mask[0] = 0x7C00;
        mask[1] = 0x03E0;
        mask[2] = 0x001F;
        decodeI420(yuv, width, height, pitch, bmp->bData.rgb + sizeof(int) * 3, rgb16_1555);
        break;
    }
    case BMP_RGB888:
    {
        bmpInfo->biHeader.biBitCount = 24;
        bmpInfo->biHeader.biCompression = BI_RGB;
        decodeI420(yuv, width, height, pitch, bmp->bData.rgb, rgb24_888);
        break;
    }
    case BMP_RGB8888:
    {
        bmpInfo->biHeader.biBitCount = 32;
        bmpInfo->biHeader.biCompression = BI_RGB;
        decodeI420(yuv, width, height, pitch, bmp->bData.rgb, rgb32_8888);
        break;
    }
    case BMP_RGB565:
    {
        /* 565格式时,biCompression可以填BI_RGB,mask信息可以省略 */
        bmpInfo->biHeader.biBitCount = 16;
        bmpInfo->biHeader.biCompression = BI_BITFIELDS;
        mask = (int *)&bmp->bData;
        mask[0] = 0xF800;
        mask[1] = 0x07E0;
        mask[2] = 0x001F;
        decodeI420(yuv, width, height, pitch, bmp->bData.rgb + sizeof(int) * 3, rgb16_565);
        break;
    }
    default:
    {
        break;
    }
    }

    return bmp;
}

int yuv2bmpfile(int type, U8 yuv[], int width, int height, const char * file)
{
    int size = 0, bmp_size = 0;
    U32 pitch = 0;
    FILE * fp = NULL;
    Bitmap * bmp = NULL;
    char * bmp_buf = NULL;

    bmp_size = get_data_size(type, width, height, &pitch);
    bmp_size += sizeof(BitmapInfo);

    bmp_buf = (char *)malloc(bmp_size);
    if (bmp_buf == NULL)
    {
        return 0;
    }

    bmp = yuv2bmp(type, bmp_buf, bmp_size, yuv, width, height);
    if (bmp == NULL)
    {
        return 0;
    }

    fp = fopen(file, "w+b");
    if (fp == NULL)
    {
        free(bmp);
        return 0;
    }

    size = bmp->bInfo.bfHeader.bfSize;
    fwrite(bmp, 1, size, fp);
    fclose(fp);
    free(bmp);

    return size;
}

