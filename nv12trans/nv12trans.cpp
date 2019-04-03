// nv12trans.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <fstream>
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <tchar.h>
#include "yuv2bmp.h"

using namespace std;

string BMP_TYPE_ARGB8888 = "ARGB8888";
string BMP_TYPE_RGB888 = "RGB888";
string BMP_TYPE_ARGB1555 = "ARGB1555";
string BMP_TYPE_RGB565 = "RGB565";

int _yuv2bmp_file(int type, char * yuv_file, char * bmp_file, int width, int height)
{
    int ret = 0;
    unsigned char * yuv_buf = NULL;
    FILE * yuv_fp = NULL;
    int count;
    int yuv_size = width * height * 3 / 2;

    yuv_fp = fopen(yuv_file, "rb");
    if (yuv_fp == NULL)
    {
        printf("failed to open file\n");
        return -1;
    }

    yuv_buf = (unsigned char *)malloc(yuv_size);
    if (yuv_buf == NULL)
    {
        printf("failed to malloc the yuv buffer\n");
        return -2;
    }
    memset(yuv_buf, 0, yuv_size);

    count = fread(yuv_buf, yuv_size, 1, yuv_fp);
    if (count != 1)
    {
        printf("failed to read yuv data!\n");
        free(yuv_buf);
        return -3;
    }

    ret = yuv2bmpfile(type, yuv_buf, width, height, bmp_file);
    if (ret == 0)
    {
        free(yuv_buf);
        return -4;
    }

    free(yuv_buf);

    return ret;
}


int main()
{
    int size = 0, nIndex = 0, type = 0;
    char yuv_path[] = "F://002-04.nv12";
    char bmp_path[] = "test.bmp";
    string str(BMP_TYPE_ARGB8888);

    if (strlen(yuv_path) == 0)
    {
        cout << "input file not exist" << endl;;
        return 0;
    }


    if (str == BMP_TYPE_ARGB8888)
    {
        type = BMP_RGB8888;
    }
    else if (str == BMP_TYPE_RGB888)
    {
        type = BMP_RGB888;
    }
    else if (str == BMP_TYPE_RGB565)
    {
        type = BMP_RGB565;
    }
    else if (str == BMP_TYPE_ARGB1555)
    {
        type = BMP_RGB1555;
    }
    else
    {
        type = BMP_RGB8888;
    }


    size = _yuv2bmp_file(type, yuv_path, bmp_path, 1920, 1080);

    if (size < 0)
    {
        cout << "transform error" << endl;;
        cout << str.c_str() << endl;
    }
    return 0;
}

